/*
 * File:   heap_profiler.cpp
 * Author: couozu
 *
 * Created on March 12, 2015, 9:58 PM
 */

#include <thread>

#include "heap_profiler.h"
#include "connection_manager.h"

const int HeapProfiler::default_id = 0;

HeapProfiler& heap_profiler() {
  static HeapProfiler worker{};
  return worker;
};

class bool_guard {
public:
  bool_guard(bool& _b) : b(_b) {
    b = true;
  }
  ~bool_guard() {
    b = false;
  }
private:
  bool& b;
};

HeapProfiler::HeapProfiler() {
  allocations.clear();
  allocated_space.clear();
}


int HeapProfiler::get_allocations_id() {
  auto it = allocation_ids.find(std::this_thread::get_id());
  return allocation_ids.end() == it ? default_id : it->second;
}

uint64_t& HeapProfiler::get_metric(int id) {
  auto it = allocated_space.emplace(id, 0);
  return it.first->second;
}

void* HeapProfiler::allocate(std::size_t count) {
  std::lock_guard<std::recursive_mutex> lock(allocation_mutex);

  void* ptr = std::malloc(count);

  if (!is_in_counter) {
    ++allocs;
    const int alloc_id = get_allocations_id();
    bool_guard b(is_in_counter);
    get_metric(alloc_id) += count;
    auto it = allocations.find(ptr);
    if (it == allocations.end()) {
      allocations.emplace(ptr, AllocData{count, alloc_id});
    }
  }

  return ptr;
}

void HeapProfiler::dealloc(void* ptr) {
  if (ptr != nullptr) {
    std::lock_guard<std::recursive_mutex> lock(allocation_mutex);

    std::free(ptr);

    if (!is_in_counter) {
      auto it = allocations.find(ptr);
      if (it != allocations.end()) {
        ++deallocs;
        bool_guard b(is_in_counter);
        get_metric(it->second.id) -= it->second.size;
        allocations.erase(ptr);
      }
    }
  }
}

void HeapProfiler::set_allocations_id(int id) {
  std::lock_guard<std::recursive_mutex> lock(allocation_mutex);
  bool_guard b(is_in_counter);
  allocation_ids[std::this_thread::get_id()] = id;
  allocated_space.emplace(id, 0);
}

void* operator new(std::size_t count){
  return heap_profiler().allocate(count);
}

void operator delete(void* ptr) {
  return heap_profiler().dealloc(ptr);
}
