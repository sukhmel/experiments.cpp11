#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>


#define DEBUG_LOG(value)

template<class T> class ThreadSafeQueue
{
public:
    ThreadSafeQueue(long maxSize = 64) : free(maxSize), used(0) {  }

    //! wait until there is free space in queue, then push value
    void push(const T &value) {
        acquire_free(1);
        std::lock_guard<std::mutex> lock(access_mutex);
        queue.push(value);
        DEBUG_LOG(value);
        change_used(+1);
    }

    //! wait until queue is not empty, then remove first element and return it
    T pop() {
        acquire_used(1);
        std::lock_guard<std::mutex> lock(access_mutex);
        auto val = queue.front();
        queue.pop();
        DEBUG_LOG(val);
        change_free(+1);
        return val;
    }

    //! wait until queue is not empty, then return first element
    T& front() {
        acquire_used(1);
        std::lock_guard<std::mutex> lock(access_mutex);
        auto value = queue.front();
        DEBUG_LOG(value);
        change_used(+1);
    }

    //! check if queue is empty
    bool is_empty() const {
       std::lock_guard<std::mutex> lock(condition_mutex);
        return 0 == used;
    }

    //! check if queue is full
    bool is_full() const {
       std::lock_guard<std::mutex> lock(condition_mutex);
        return 0 == free;
    }

    //! get current amount of elements in queue
    int count() const {
       std::lock_guard<std::mutex> lock(condition_mutex);
        return used;
    }

    //! remove all elements from queue
    bool clear()
    {
        bool result = false;
        std::lock_guard<std::mutex> lock(access_mutex);
        int amount = count();
        if (amount)
        {
            clear_queue();
            change_both(amount);
            DEBUG_LOG("cleared");
            result = true;
        }

        return result;
    }

    //! get first element of queue or empty value if none available
    T& try_front() {
        T value{};
        std::lock_guard<std::mutex> lock(access_mutex);
        if(used > 0)
        {
            value = queue.front();
            DEBUG_LOG(value);
        }
        return value;
    }

    //! get and remove from queue first element or empty value if none available
    T try_pop() {
        T value{};
        std::lock_guard<std::mutex> lock(access_mutex);
        if(used > 0)
        {
            change_used(-1);
            value = queue.front();
            queue.pop();
            DEBUG_LOG(value);
            change_free(+1);
        }
        return value;
    }

    //! try to add an element to queue if space is available, return true on success
    bool try_push(T value) {
        std::lock_guard<std::mutex> lock(access_mutex);
        bool success = free > 0;
        if(success)
        {
            change_free(-1);
            queue.push(value);
            DEBUG_LOG(value);
            change_used(+1);
        }
        return success;
    }

    //! try to change size of the queue
    bool try_resize(int size)
    {
        bool result = false;

        if (size > 0)
        {
            std::lock_guard<std::mutex> lock(access_mutex);
            int difference = size - (used + free);
            if (used < size) //! we don't want destructive resize
            {
                //! add or remove available elements
                //! depending on sign of difference
                change_free(difference);
                result = true;

                DEBUG_LOG("resized");
            }
        }

        return result;
    }

protected:
   void clear_queue()
   {
       while (!queue.empty())
       {
           queue.pop();
       }
   }

   void change_both(long free_delta)
   {
       std::lock_guard<std::mutex> lock(condition_mutex);
       free += free_delta;
       used -= free_delta;

       if (free > 0)
       {
           space_available.notify_all();
       }
       if (used > 0)
       {
           values_available.notify_all();
       }
   }

   void change_free(long delta)
   {
       std::lock_guard<std::mutex> lock(condition_mutex);
       free += delta;
       if (free > 0)
       {
           space_available.notify_all();
       }
   }

   void change_used(long delta)
   {
       std::lock_guard<std::mutex> lock(condition_mutex);
       used += delta;
       if (used > 0)
       {
           values_available.notify_all();
       }
   }

   void acquire_free(long amount)
   {
       std::unique_lock<std::mutex> condition_lock(condition_mutex);
       while(true)
       {
           if (free >= amount)
           {
               free -= amount;
               break;
           }
           else
           {
               space_available.wait(condition_lock);
           }
       }
   }

   void acquire_used(long amount)
   {
       std::unique_lock<std::mutex> condition_lock(condition_mutex);
       while(true)
       {
           if (used >= amount)
           {
               used -= amount;
               break;
           }
           else
           {
               values_available.wait(condition_lock);
           }
       }
   }

private:
    std::queue<T> queue;
    long free{-1};
    long used{-1};
    std::mutex access_mutex;
    std::mutex condition_mutex;
    std::condition_variable values_available; //! notify on new elements
    std::condition_variable space_available; //! notify on free space
};

#endif // THREADSAFE_QUEUE_HPP
