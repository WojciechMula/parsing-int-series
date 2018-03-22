.PHONY: all clean

FLAGS:=-std=c++11 -Wall -Wextra -pedantic -march=native -O3 $(CXXFLAGS)
FLAGS:=$(FLAGS) -Iinclude

ALL=validate verify benchmark

OBJ=obj/input_generator.o \
    obj/block_info.o

DEPS=include/scalar-parser.h \
     include/sse-convert.h \
     include/sse-matcher.h \
     include/sse-parser.h

all: $(ALL)

validate: src/validate.cpp $(DEPS) $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@

verify: src/verify.cpp $(DEPS) $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@

benchmark: src/benchmark.cpp $(DEPS) include/time_utils.h $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@

obj/input_generator.o: src/input_generator.cpp include/input_generator.h
	$(CXX) $(FLAGS) $< -c -o $@

obj/block_info.o: src/block_info.cpp src/block_info.inl include/block_info.h
	$(CXX) $(FLAGS) $< -c -o $@

src/block_info.inl: scripts/generator.py scripts/writer.py
	python $< $@

clean:
	$(RM) $(ALL)
