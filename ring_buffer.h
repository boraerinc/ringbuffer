#pragma once

#include <cstddef>
#include <bit>
#include <utility>

/*
 *
 * Ring Buffer invariant: Head always points to the frontmost element inserted, 
 * and tail points to one-past the backmost element inserted. Here are a few
 * examples to illustrate.
 *
 * Example 1: at size 0, head = 0, tail = 0. Pushing front will increment tail
 * After: data[0] = elem, tail = 1, head = 0. Invariant holds.
 *
 * Example 2: at size = 1, head = 0, tail = 1. Pushing front will first 
 * decrement head, and then place an element.
 * After: size = 2, tail = 1, head = capacity - 1. Invariant holds.
 *
 * Example 3: at size = full, tail = n, head = n. Pushing front will first
 * decrement head, place new element, overwriting backmost element, then
 * tail will also be decremented.
 * After: size is the same, tail = n-1, head = n-1. In a way, tail is still
 * pointing to "one-past the backmost element", and head is pointing at the
 * frontmost element (the one just inserted). Invariant holds.
 *
 */
template<typename T>
class RingBuffer {

  class Const_Iterator;

  class Iterator {
    T* m_Base;
    size_t m_Offset;
    size_t m_Capacity;

  public:
    Iterator(T* base, size_t capacity, size_t offset) 
      : m_Base{base}
      , m_Offset(offset) 
      , m_Capacity{capacity}
      
    {}

    Iterator(const Iterator& rhs) 
      : m_Base{rhs.m_Base}
      , m_Offset(rhs.m_Offset) 
      , m_Capacity{rhs.m_Capacity}
    {}

    Iterator& operator++() {
      m_Offset++;
      return *this;
    }

    Iterator operator++(int) {
      Iterator ret{*this};
      m_Offset++;
      return ret;
    }

    T& operator*() const {
      return *(m_Base + (m_Offset & (m_Capacity - 1)));
    }

    bool operator<(const Iterator& rhs) const noexcept {
      return static_cast<ptrdiff_t>(m_Offset - rhs.m_Offset) < 0; 
    }

    bool operator!=(const Iterator& rhs) const noexcept {
      return (*this < rhs) || (rhs < *this); 
    }

    bool operator==(const Iterator& rhs) const noexcept {
      return !(*this != rhs); 
    }

    friend class Const_Iterator;

  };

  class Const_Iterator {
    T* m_Base;
    size_t m_Capacity;
    size_t m_Offset;

  public:
    Const_Iterator(T* base, size_t capacity, size_t offset) 
      : m_Base{base}
      , m_Capacity{capacity}
      , m_Offset(offset) 
      
    {}

    Const_Iterator(const Const_Iterator& rhs) 
      : m_Base{rhs.m_Base}
      , m_Capacity{rhs.m_Capacity}
      , m_Offset(rhs.m_Offset) 
    {}

    Const_Iterator(const Iterator& rhs)
      : m_Base{rhs.m_Base}
      , m_Capacity{rhs.m_Capacity}
      , m_Offset(rhs.m_Offset) 
    {}

    Const_Iterator& operator++() {
      m_Offset++;
      return *this;
    }

    Const_Iterator operator++(int) {
      Const_Iterator ret{*this};
      m_Offset++;
      return ret;
    }

    const T& operator*() const {
      return *(m_Base + (m_Offset & (m_Capacity - 1)));
    }

    bool operator<(const Const_Iterator& rhs) const noexcept {
      return static_cast<ptrdiff_t>(m_Offset - rhs.m_Offset) < 0;
    }

    bool operator!=(const Const_Iterator& rhs) const noexcept {
      return (*this < rhs) || (rhs < *this);
    }

    bool operator==(const Const_Iterator& rhs) const noexcept {
      return !(*this != rhs); 
    }
  };

  size_t m_Capacity;
  size_t m_Head;
  size_t m_Tail;
  size_t m_Size;
  T* m_Data;

  size_t head() const noexcept {
    return (m_Head) & (m_Capacity - 1);
  }

  size_t tail() const noexcept {
    return m_Tail & (m_Capacity - 1);
  }


  void clearForMove() noexcept {
    m_Capacity = 0uz;
    m_Head = 0uz;
    m_Tail = 0uz;
    m_Size= 0uz;
    m_Data = nullptr;
  }

  public:
  Iterator begin() {
    return Iterator(m_Data,  m_Capacity, m_Head); 
  }
  Const_Iterator begin() const {
    return Const_Iterator(m_Data,  m_Capacity, m_Head);
  }
  Const_Iterator cbegin() const {
    return Const_Iterator(m_Data,  m_Capacity, m_Head);
  }
  Iterator end() {
    return Iterator(m_Data,  m_Capacity, m_Tail);
  }
  Const_Iterator end() const {
    return Const_Iterator(m_Data,  m_Capacity, m_Tail);
  }
  Const_Iterator cend() const {
    return Const_Iterator(m_Data,  m_Capacity, m_Tail);
  }

  static constexpr size_t DEFAULT_CAP = 128;

  // *************************************************************************
  // RULE OF FIVE 
  // *************************************************************************

  // Constructor
  explicit RingBuffer(size_t capacity = DEFAULT_CAP) 
    : m_Capacity { std::bit_ceil(capacity) }
    , m_Head { 0uz }
    , m_Tail { 0uz }
    , m_Size { 0uz }
    , m_Data { new T[m_Capacity] }
  {}

  
  // Copy Constructor
  RingBuffer(const RingBuffer& other) 
    : m_Capacity { other.m_Capacity }
    , m_Head { 0uz }
    , m_Tail { other.m_Size }
    , m_Size { other.m_Size }
    , m_Data { new T[other.m_Capacity] }
  {
    for(auto it{0uz}; it < other.m_Size; ++it) {
      m_Data[it] = other[it];
    }
  }

  // Move Constructor
  // Postcondition: other should only be assigned to or destroyed
  RingBuffer(RingBuffer&& other) noexcept
    : m_Capacity { other.m_Capacity }
    , m_Head { other.m_Head }
    , m_Tail { other.m_Tail }
    , m_Size { other.m_Size }
    , m_Data { other.m_Data }
  {
    other.clearForMove();
  }

  // assignment (Copy and swap idiom for exception safety)
  RingBuffer& operator=(RingBuffer other) {
    swap(other);
    return *this;
  }

  void swap(RingBuffer& other) noexcept {
    std::swap(m_Tail, other.m_Tail);
    std::swap(m_Head, other.m_Head);
    std::swap(m_Size, other.m_Size);
    std::swap(m_Data, other.m_Data);
    std::swap(m_Capacity, other.m_Capacity);
  }

  //Destructor
  ~RingBuffer() {
    delete[] m_Data;
  }

  // Precondition: !empty()
  void pop_front() {
    m_Head++;
    m_Size--;
  }

  // Precondition: !empty()
  void pop_back() {
    m_Tail--;
    m_Size--;
  }
  
  // Pushing when the buffer is full wraps and overwrites tail element
  // Tail is still valid after
  void push_front(T val) {
    if(m_Size == 0) {
      ++m_Tail;
      ++m_Size;
    } else if(m_Size == m_Capacity) {
      --m_Tail;
      --m_Head;
    } else {
      ++m_Size;
      --m_Head;
    }

    m_Data[head()] = val;
  }

  // Pushing when the buffer is full wraps and overwrites head element
  // Head is still valid after
  void push_back(T val) {
    m_Data[tail()] = val;
    m_Tail++;
    if(m_Size == m_Capacity) {
      m_Head++;
    } else {
      m_Size++;
    }
  }

  // Precondition: !empty()
  T& front() noexcept {
    return m_Data[head()];
  }
  
  // Precondition: !empty()
  const T& front() const noexcept {
    return m_Data[head()];
  }

  // Precondition: !empty()
  T& back() noexcept {
    return m_Data[(m_Tail - 1) & (m_Capacity - 1)];
  }
  
  // Precondition: !empty()
  const T& back() const noexcept {
    return m_Data[(m_Tail - 1) & (m_Capacity - 1)];
  }

  // Precondition: where < m_Size 
  const T& operator[](size_t where) const noexcept {
    return m_Data[(head() + where) & (m_Capacity-1)];
  }

  // Precondition: !empty()
  T& operator[](size_t where) noexcept {
    return m_Data[(head() + where) & (m_Capacity-1)];
  }

  [[nodiscard]] size_t size() const noexcept {
    return m_Size;
  }
  
  [[nodiscard]] size_t capacity() const noexcept {
    return m_Capacity;
  }

  [[nodiscard]] bool empty() const noexcept {
    return m_Size == 0;
  }

  [[nodiscard]] bool full() const noexcept {
    return m_Size == m_Capacity;
  }

  void clear() noexcept {
    m_Size = 0uz;
    m_Head = 0uz;
    m_Tail = 0uz;
  }

};
