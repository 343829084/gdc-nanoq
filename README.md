**gdc-nanoq** is a wait-free multi-threaded single-producer/single-consumer
circular queue implementation in C++11. It's been tested on Linux with
GCC 5.3.1 and on Mac OS X with clang 7.3.0.


```c++
// The implementation is in a single header file.
#include <gdc/nanoq.hpp>

// A factory will allocate the queue for us. This factory allocates
// it dynamically from the heap.
gdc::nanoq_heap_factory<int> factory(32);

// Get the queue from the factory.
gdc::nanoq<int>& q = factory.get();

// Produce in one thread.
q.push_back(42);

// Consume in another thread.
std::cout << q.front() << std::endl;
```
