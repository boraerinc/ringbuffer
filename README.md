# Hello!
This is my Ring Buffer. As I work on this data structure, I will update the repo. The class currently works, but I have a few planned improvements:
- Improve memory allocation logic to use raw storage + `construct_at/destroy_at`
- Use a testing library or macro engine intead of `assert` statements.
- Make the class thread safe 
- Benchmark against some popular Deque/ArrayDeque/RingBuffer implementations.

## Know Limitations
T must be default constructible! Will change this soon. 

All code written by hand, it is a learning experience, not about pumping out code!
