.PHONY: all clean

FLAGS:=-std=c++11 -Wall -Wextra -pedantic -O3 $(CXXFLAGS)
FLAGS:=$(FLAGS) -Iinclude

ALL=validate

OBJ=obj/input_generator.o

all: $(ALL)

validate: src/validate.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@

obj/input_generator.o: src/input_generator.cpp include/input_generator.h
	$(CXX) $(FLAGS) $< -c -o $@

clean:
	$(RM) $(ALL)
