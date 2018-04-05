.PHONY: all clean

FLAGS:=-std=c++11 -Wall -Wextra -pedantic -march=native -O3 $(CXXFLAGS)
FLAGS:=$(FLAGS) -Iinclude -Iinclude/test

PARSER_COMMON_HEADERS= \
    include/block_info.h \
    include/sse/sse-utils.h \
    include/sse/sse-convert.h \
    include/sse/sse-matcher.h \
    include/sse/sse-matcher-stni.h \
    include/sse/sse-parser-common.h

PARSER_UNSIGNED_HEADERS= \
    include/scalar/scalar-parse-unsigned.h \
    include/sse/sse-parser-unsigned.h \
    include/sse/sse-parser-unsigned-unrolled.h

PARSER_SIGNED_HEADERS= \
    include/scalar/scalar-parse-signed.h \
    include/sse/sse-parser-signed.h \
    include/sse/sse-parser-signed-unrolled.h

PARSER_AVX512_HEADERS= \
    include/scalar/scalar-parse-signed.h \
    include/sse/sse-parser-signed.h \
    include/avx512/avx512-parser-signed.h

PARSER_OBJ= \
    obj/block_info.o

PARSER_UNSIGNED_DEPS=$(PARSER_COMMON_HEADERS) $(PARSER_UNSIGNED_HEADERS) $(PARSER_OBJ)
PARSER_SIGNED_DEPS=$(PARSER_COMMON_HEADERS) $(PARSER_SIGNED_HEADERS) $(PARSER_OBJ)
PARSER_DEPS=$(PARSER_COMMON_HEADERS) $(PARSER_UNSIGNED_HEADERS) $(PARSER_SIGNED_HEADERS) $(PARSER_OBJ)
PARSER_AVX512_DEPS=$(PARSER_COMMON_HEADERS) $(PARSER_AVX512_HEADERS) $(PARSER_OBJ)

CMDLINE_OBJ= \
    obj/application.o \
    obj/command_line.o \
    obj/discrete_distribution.o \
    obj/input_generator.o \
    $(PARSER_OBJ)

CMDLINE_DEPS=include/test/*.h test/utils/*cpp $(CMDLINE_OBJ)


UNITTESTS= \
    bin/test-stni-matcher \
    bin/verify_sse_signed_parser \
    bin/verify_sse_signed_parser_validation \
    bin/verify_sse_unsigned_conversion \
    bin/verify_sse_unsigned_parser \

BENCHMARK= \
    bin/benchmark \
    bin/benchmark-all

TEST= \
    bin/compare-signed \
    bin/compare-unsigned \
    bin/compare-avx512 \
    bin/statistics

ALL=$(UNITTESTS) $(BENCHMARK) $(TEST)

all: $(ALL)

clean:
	$(RM) $(ALL) obj/*.o

run-unittests: $(UNITTESTS)
	./bin/test-stni-matcher
	./bin/verify_sse_signed_parser
	./bin/verify_sse_signed_parser_validation
	./bin/verify_sse_unsigned_conversion
	./bin/verify_sse_unsigned_parser

# --------------------------------------------------------------------------------

obj/block_info.o: src/block_info.cpp include/block_info.h scripts/generator.py scripts/writer.py
	$(CXX) $(FLAGS) -c $< -o $@


# unit tests
# --------------------------------------------------------------------------------
bin/test-stni-matcher: test/unittest/test-stni-matcher.cpp include/sse/sse-matcher-stni.h
	$(CXX) $(FLAGS) $< -o $@

bin/verify_sse_signed_parser: test/unittest/verify_sse_signed_parser.cpp $(PARSER_SIGNED_DEPS)
	$(CXX) $(FLAGS) $(PARSER_OBJ) $< -o $@

bin/verify_sse_signed_parser_validation: test/unittest/verify_sse_signed_parser_validation.cpp $(PARSER_SIGNED_DEPS)
	$(CXX) $(FLAGS) $(PARSER_OBJ) $< -o $@

bin/verify_sse_unsigned_conversion: test/unittest/verify_sse_unsigned_conversion.cpp $(PARSER_UNSIGNED_DEPS)
	$(CXX) $(FLAGS) $(PARSER_OBJ) $< -o $@

bin/verify_sse_unsigned_parser: test/unittest/verify_sse_unsigned_parser.cpp $(PARSER_UNSIGNED_DEPS)
	$(CXX) $(FLAGS) $(PARSER_OBJ) $< -o $@


# test programs
# --------------------------------------------------------------------------------
bin/benchmark: test/benchmark.cpp $(PARSER_DEPS) $(CMDLINE_DEPS)
	$(CXX) $(FLAGS) $(CMDLINE_OBJ) $< -o $@

bin/benchmark-all: test/benchmark-all.cpp $(PARSER_DEPS) $(CMDLINE_DEPS) include/hybrid-parser.h include/hybrid-parser.inl 
	$(CXX) $(FLAGS) $(CMDLINE_OBJ) $< -o $@

bin/compare-signed: test/compare-signed.cpp $(PARSER_SIGNED_DEPS) $(CMDLINE_DEPS)
	$(CXX) $(FLAGS) $(CMDLINE_OBJ) $< -o $@

bin/compare-unsigned: test/compare-unsigned.cpp $(PARSER_SIGNED_DEPS) $(CMDLINE_DEPS)
	$(CXX) $(FLAGS) $(CMDLINE_OBJ) $< -o $@

bin/compare-avx512: test/compare-avx512.cpp $(PARSER_AVX512_DEPS) $(CMDLINE_DEPS)
	$(CXX) $(FLAGS) -mavx512vbmi $(CMDLINE_OBJ) $< -o $@

bin/statistics: test/statistics.cpp $(PARSER_DEPS) $(CMDLINE_DEPS) obj/sse-parser-statistics.o
	$(CXX) $(FLAGS) $(CMDLINE_OBJ) obj/sse-parser-statistics.o $< -o $@

# cmdline utilites
# --------------------------------------------------------------------------------

obj/application.o: test/utils/application.cpp include/test/*.h obj/command_line.o obj/discrete_distribution.o obj/input_generator.o
	$(CXX) $(FLAGS) -c $< -o $@

obj/command_line.o: test/utils/command_line.cpp include/test/command_line.h
	$(CXX) $(FLAGS) -c $< -o $@

obj/discrete_distribution.o: test/utils/discrete_distribution.cpp include/test/discrete_distribution.h
	$(CXX) $(FLAGS) -c $< -o $@

obj/input_generator.o: test/utils/input_generator.cpp include/test/input_generator.h
	$(CXX) $(FLAGS) -c $< -o $@

obj/sse-parser-statistics.o: src/sse-parser-statistics.cpp include/sse/sse-parser-statistics.h
	$(CXX) $(FLAGS) -c $< -o $@


# hybrid parser
# --------------------------------------------------------------------------------

include/hybrid-parser.inl: scripts/hybrid-generator.py
	python $< > $@


# experiments
# --------------------------------------------------------------------------------

measurements.txt: bin/benchmark experiments/experiment.py experiments/testcases.py
	# this is a long-running procedure, it'd be better to see if the program really works
	python experiments/experiment.py | tee /tmp/$@
	mv /tmp/$@ $@

report.rst: measurements.txt experiments/report.py experiments/writer.py experiments/table.py
	python experiments/report.py $< > /tmp/$@
	mv /tmp/$@ $@

