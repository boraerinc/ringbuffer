#include "ring_buffer.h"
#include <iostream>
#include <assert.h>

template<typename T>
void printRingBuffer(const RingBuffer<T>& rb) {
  for(const auto& val : rb) {
    std::cout << val << ' ';
  }
  std::cout << std::endl;
}

template<typename T>
void printRingBufferNonConst(RingBuffer<T>& rb) {
  for(const auto& val : rb) {
    std::cout << val << ' ';
  }
  std::cout << std::endl;
}

template<typename T>
void setValuesToZero(RingBuffer<T>& rb) {
  for(auto& val : rb) {
    val = 0;
  }
}

int main() {
  // Test 1 default capacity should be DEFAULT_CAP, size 0
  RingBuffer<int> values{};
  assert(values.capacity() == RingBuffer<int>::DEFAULT_CAP);
  assert(values.size() == 0);

  // Test 2 insert values, check size, head, back
  values.push_back(1);
  values.push_back(2);
  values.push_back(3);
  values.push_front(4);
  values.push_back(5);
  values.push_front(6);
  assert(values.size() == 6);
  std::cout << values.front() << std::endl;
  assert(values.front() == 6);

  assert(values.back() == 5);


  // Test 3 operator[]
  assert(values[0] == 6);
  assert(values[1] == 4);
  assert(values[2] == 1);
  assert(values[3] == 2);
  assert(values[4] == 3);
  assert(values[5] == 5);

  // Test 4 Pop, size and value check
  values.pop_back();
  values.pop_front();
  values.pop_front();
  assert(values.size() == 3);
  assert(values[0] == 1);
  assert(values[1] == 2);
  assert(values[2] == 3);
  
  // Test 5 add more than capacity
  for(int i{20}; i < 500; ++i) {
    values.push_front(i);
  }
  assert(values.size() == RingBuffer<int>::DEFAULT_CAP);

  // Test 6 Clearing
  values.clear();
  assert(values.size() == 0);
  std::cout << "Printing before clearing (should be empty): \n";
  printRingBuffer(values);
  std::cout<< "Done!\n";

  // Test 7 print the thing
  for(int i{0}; i < 10; ++i) {
    values.push_back(i);
  }
  std::cout << "Printing before adding values:\n";
  printRingBuffer(values);
  std::cout<< "Done!\n";

  // Test 8 Move construction
  RingBuffer<int> newValues{std::move(values)};
  assert(newValues.size() == 10);
  assert(newValues.back() == 9);
  assert(newValues.front() == 0);
  assert(values.size() == 0);

  // Test 9 Copy construction
  RingBuffer<int> newValuesTwo{newValues};
  assert(newValues.size() == 10);
  assert(newValues.back() == 9);
  assert(newValues.front() == 0);
  assert(newValuesTwo.size() == 10);
  assert(newValuesTwo.back() == 9);
  assert(newValuesTwo.front() == 0);

  // Test 10 Assignment
  values = newValues;
  assert(values.size() == 10);
  assert(values.back() == 9);
  assert(values.front() == 0);

  // Test 10 B move assignment
  newValuesTwo.push_back(10);
  newValuesTwo.push_front(11);
  newValues = std::move(newValuesTwo);
  assert(newValuesTwo.size() == 0);
  assert(newValues.size() == 12);
  assert(newValues.back() == 10);
  assert(newValues.front() == 11);
  

  // Test 11 Bit_ceil
  RingBuffer<int> ceil1(7uz);
  assert(ceil1.capacity() == 8);

  RingBuffer<int> ceil2(55uz);
  assert(ceil2.capacity() == 64);

  RingBuffer<int> ceil3(129uz);
  assert(ceil3.capacity() == 256);


  // Test 12 Wraparound
  RingBuffer<int> forwardwrap{};
  for(int i{}; i < 129; ++i) {
    forwardwrap.push_back(i);
  }
  assert(forwardwrap[0] == 1);
  assert(forwardwrap.back() == 128);
  assert(forwardwrap.size() == 128);
  RingBuffer<int> backwrap{};

  for(int i{}; i < 129; ++i) {
    backwrap.push_front(i);
  }
  assert(backwrap.back() == 1);
  assert(backwrap.size() == 128);

  //Test 13 NonConst Iterator:
  printRingBufferNonConst(newValues);

  //Test 14 Modifying iterator
  setValuesToZero(newValues);
  printRingBufferNonConst(newValues);

}
