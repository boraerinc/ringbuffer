CXX ?= g++
CXXFLAGS ?= -std=c++23 -Wall -Wpedantic

ring_buffer_test: ring_buffer_test.cpp ring_buffer.h
	$(CXX) $(CXXFLAGS) -o $@ ring_buffer_test.cpp

clean:
	rm ring_buffer_test 
