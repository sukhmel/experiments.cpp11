/* 
 * File:   heap_profiler.h
 * Author: couozu
 *
 * Created on March 12, 2015, 9:58 PM
 */

#ifndef HEAP_PROFILER_H
#define	HEAP_PROFILER_H

#include <stdint.h>
#include <cstddef>
#include <mutex>
#include <vector>
#include <thread>
#include <map>

class HeapProfiler {
public:
  HeapProfiler();
  
  HeapProfiler(const HeapProfiler& orig) = delete;
  virtual ~HeapProfiler() {}
  
  const std::map<int, uint64_t>& get_allocated_space() const {
    return allocated_space;
  }

  uint64_t get_allocations() const {
    return allocs;
  }
  uint64_t get_deallocations() const {
    return deallocs;
  }
  
  void* allocate(std::size_t count);
  void dealloc(void* ptr);
  
  void set_allocations_id(int id);
  int get_allocations_id();
  
private:
  static const int default_id;
  std::map<std::thread::id, int> allocation_ids;
  
  
  uint64_t& get_metric(int id);
  
  struct AllocData {
    std::size_t size;
    int id;
  };
  
  std::map<void*, AllocData> allocations{};
 
  std::recursive_mutex allocation_mutex;
  
  std::map<int, uint64_t> allocated_space;

  uint64_t allocs{};
  uint64_t deallocs{};

  bool is_in_counter{false};
};

HeapProfiler& heap_profiler();

class alloc_id_guard {
public:
  alloc_id_guard(int id) : old_allocations_id(heap_profiler().get_allocations_id()) {
    heap_profiler().set_allocations_id(id);
  }
  ~alloc_id_guard() {
    heap_profiler().set_allocations_id(old_allocations_id);
  }
public:
  const int old_allocations_id;
};

#endif	/* HEAP_PROFILER_H */

