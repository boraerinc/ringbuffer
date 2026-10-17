#include <cstddef>
#include <bit>
#include <cstring>
#include <utility>

template<typename T>
class RingBuffer {
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

    bool operator<(const Iterator& rhs) {
      return (m_Base + m_Offset) < (rhs.m_Base + rhs.m_Offset); 
    }

    bool operator!=(const Iterator& rhs) {
      return (m_Base + m_Offset) != (rhs.m_Base + rhs.m_Offset); 
    }

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

    bool operator<(const Const_Iterator& rhs) const {
      return (m_Base + m_Offset ) < 
        (rhs.m_Base + rhs.m_Offset );
    }

    bool operator!=(const Const_Iterator& rhs) const {
      return (*this < rhs) || (rhs < *this);
    }

    bool operator==(const Const_Iterator& rhs) const {
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
  RingBuffer(size_t capacity = DEFAULT_CAP) 
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

  void pop_front() {
    m_Head++;
    m_Size--;
  }

  void pop_back() {
    m_Tail--;
    m_Size--;
  }
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

  void push_back(T val) {
    m_Data[tail()] = val;
    m_Tail++;
    if(m_Size == m_Capacity) {
      m_Head++;
    } else {
      m_Size++;
    }
  }

  T& front() noexcept {
    return m_Data[head()];
  }
  
  const T& front() const noexcept {
    return m_Data[head()];
  }

  T& back() noexcept {
    return m_Data[(m_Tail - 1) & (m_Capacity - 1)];
  }
  
  const T& back() const noexcept {
    return m_Data[(m_Tail - 1) & (m_Capacity - 1)];
  }

  const T& operator[](size_t where) const noexcept {
    return m_Data[(head() + where) & (m_Capacity-1)];
  }

  T& operator[](size_t where) noexcept {
    return m_Data[(head() + where) & (m_Capacity-1)];
  }

  size_t size() const noexcept {
    return m_Size;
  }
  
  size_t capacity() const noexcept {
    return m_Capacity;
  }

  bool isEmpty() const noexcept {
    return m_Size == 0;
  }

  bool isFull() const noexcept {
    return m_Size == m_Capacity;
  }

  void clear() noexcept {
    m_Size = 0uz;
    m_Head = 0uz;
    m_Tail = 0uz;
  }

};
