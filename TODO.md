# TODO

Roughly ordered by payoff within each section.

## Correctness and design
- [ ] **Raw storage instead of `new T[]`.** Allocate bytes, `std::construct_at` on push, `std::destroy_at` on pop/clear, destroy the live range in the destructor. Removes the "T must be default-constructible" limitation, stops popped elements living on, and makes the copy constructor exception-safe (it currently leaks if `T`'s assignment throws).
- [ ] `emplace_back` / `emplace_front` (falls out of the above).
- [ ] `push_front` / `push_back` overloads for `const T&` and `T&&`, and move the value into the slot (today it is taken by value and then copied).
- [ ] Debug `assert`s for the documented preconditions (`pop_*`, `front`, `back` on empty; `operator[]` bounds). The power-of-two mask makes misuse silent, since every index lands in bounds.
- [ ] Moved-from state: today it is "assign or destroy only", and `push_*` on a moved-from buffer crashes. Option: make capacity 0 a real state with lazy allocation, and make the default-constructed state the same as moved-from.
- [ ] Iterators:
  - [ ] Iterator typedefs (`value_type`, `difference_type`, `iterator_category`, ...) so `std::` algorithms work
  - [ ] `operator->`, default constructor
  - [ ] Merge `Iterator` / `Const_Iterator` into one `template<bool IsConst>`
  - [ ] Consider random-access (`--`, `+`, `-`, `[]`)
  - [ ] Qualify `std::size_t` / `std::ptrdiff_t`
- [ ] Optional API: `try_push_*` (returns bool), `at()`, initializer-list constructor, free `swap`, comparison operators, a two-span contiguous view of the contents.
- [ ] Optional: growable variant (copy in logical order into a doubled buffer, reset head/tail).
- [ ] Optional: replace `0uz` with `size_t{0}` to support C++20 compilers.

## Tests
- [ ] Replace raw `assert` with a test library (doctest / Catch2) or a `CHECK` macro, since `assert` disappears under `-DNDEBUG`.
- [ ] Add cases: copy of a wrapped buffer, `it++`, full-buffer iteration, empty iteration, capacity 1, `RingBuffer<std::string>`, moved-from behaviour, self-assignment, const-iterator `<` across a counter wrap.
- [ ] Put the randomized differential test against a `std::deque` model into the repo (random ops incl. both-end push/pop, clear, copy/move/assign; compare size, `[]`, `front`/`back`, and both iterators after every op).
- [ ] Makefile: `asan` target (`-g -fsanitize=address,undefined`), `test` target that runs the binary, `.PHONY`, `-Wextra`.
- [ ] CI (GitHub Actions): gcc + clang, plus the sanitizer build.

## Docs and repo
- [ ] README: build/run commands, compiler requirement (C++23 for `uz`), short usage example, API table.
- [ ] README "Guarantees and limitations": preconditions unchecked, overflow overwrites the opposite end, capacity rounds up to a power of two, moved-from state, not thread-safe, iterator invalidation, exception safety, complexity.
- [ ] README design notes: why power-of-two masking, why free-running counters, the head/tail invariant.
- [ ] `LICENSE` (MIT).
- [ ] Layout: `include/`, `tests/`; CMake once a dependency (Google Benchmark / test lib) is added.

## Next projects (by payoff)
1. [ ] Benchmark against `std::deque` and `std::vector`-as-queue (Google Benchmark), with a chart in the README.
2. [ ] SPSC lock-free variant (atomics, acquire/release, cache-line padding to avoid false sharing) as the feed-to-engine handoff in slipstream; benchmark against mutex + `std::queue`.
3. [ ] Thread-safety story for the main class (or document that it is not).
4. [ ] Another data structure: order book (after the SPSC queue) or a memory pool.
