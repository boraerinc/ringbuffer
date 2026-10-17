ring_buffer_test:
	g++-16 -std=c++23 -Wall -Wpedantic -o ring_buffer_test ring_buffer_test.cpp

clean:
	rm ring_buffer_test 
