.PHONY: all clean

FLAGS:=-std=c++11 -Wall -Wextra -pedantic -march=native -O3 $(CXXFLAGS)
FLAGS:=$(FLAGS) -Iinclude

ALL=compare verify benchmark statistics

OBJ=obj/block_info.o \
    obj/sse-parser-statistics.o

CMDLINE_OBJ=obj/command_line.o \
            obj/input_generator.o \
            obj/discrete_distribution.o \
            obj/application.o

DEPS=include/scalar/scalar-parse-unsigned.h \
     include/scalar/scalar-parse-signed.h \
     include/hybrid-parser.h \
     include/sse/sse-utils.h \
     include/sse/sse-convert.h \
     include/sse/sse-matcher.h \
     include/sse/sse-parser-common.h \
     include/sse/sse-parser-unsigned.h \
     include/sse/sse-parser-unsigned-unrolled.h \
     include/sse/sse-parser-signed.h

all: $(ALL)

compare: test/compare.cpp $(DEPS) $(OBJ) $(CMDLINE_OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) $(CMDLINE_OBJ) -o $@

verify: test/verify.cpp $(DEPS) $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@

STATISTICS_OBJ=$(OBJ) $(CMDLINE_OBJ)
statistics: test/statistics.cpp $(DEPS) $(STATISTICS_OBJ)
	$(CXX) $(FLAGS) -DUSE_STATISTICS $< $(STATISTICS_OBJ) -o $@

BENCHMARK_OBJ=$(OBJ) $(CMDLINE_OBJ)
benchmark: test/benchmark.cpp $(DEPS) include/time_utils.h include/hybrid-parser.inl $(BENCHMARK_OBJ)
	$(CXX) $(FLAGS) $< $(BENCHMARK_OBJ) -o $@

obj/input_generator.o: test/input_generator.cpp include/input_generator.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/command_line.o: test/command_line.cpp include/command_line.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/application.o: test/application.cpp include/application.h include/command_line.h include/input_generator.h include/time_utils.h include/discrete_distribution.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/block_info.o: src/block_info.cpp src/block_info.inl include/block_info.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/sse-parser-statistics.o: src/sse-parser-statistics.cpp include/sse/sse-parser-statistics.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/discrete_distribution.o: test/discrete_distribution.cpp include/discrete_distribution.h
	$(CXX) $(FLAGS) $< -c -o $@

src/block_info.inl: scripts/generator.py scripts/writer.py
	python $< $@

include/hybrid-parser.inl: scripts/hybrid-generator.py
	python $< > $@

clean:
	$(RM) $(ALL) obj/*.o
