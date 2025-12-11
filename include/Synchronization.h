

#ifndef PROCESS_THREAD_MANAGER_SYNCHRONIZATION_H
#define PROCESS_THREAD_MANAGER_SYNCHRONIZATION_H

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <string>
#include <cstring>
#include <semaphore.h>

namespace PTManager {

// Enhanced Mutex with deadlock detection
class SafeMutex {
private:
    std::timed_mutex mtx;
    std::string name;
    std::atomic<std::thread::id> owner;
    std::atomic<int> lockCount;

public:
    explicit SafeMutex(const std::string& mutexName = "");

    bool lock(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));
    bool tryLock();
    void unlock();

    std::string getName() const { return name; }
    bool isLocked() const;
    std::thread::id getOwner() const { return owner.load(); }
};

// RAII lock guard for SafeMutex
class SafeLockGuard {
private:
    SafeMutex& mutex;
    bool locked;

public:
    explicit SafeLockGuard(SafeMutex& m);
    ~SafeLockGuard();

    SafeLockGuard(const SafeLockGuard&) = delete;
    SafeLockGuard& operator=(const SafeLockGuard&) = delete;
};

// Semaphore wrapper
class Semaphore {
private:
    sem_t sem;
    std::string name;
    bool initialized;

public:
    explicit Semaphore(unsigned int value = 0, const std::string& semName = "");
    ~Semaphore();

    bool wait();
    bool tryWait();
    bool timedWait(std::chrono::milliseconds timeout);
    bool post();

    int getValue();
};

// Reader-Writer Lock
class RWLock {
private:
    std::mutex mutex;
    std::condition_variable readCV;
    std::condition_variable writeCV;

    int readers;
    int writers;
    int waitingWriters;

public:
    RWLock();

    void readLock();
    void readUnlock();

    void writeLock();
    void writeUnlock();
};

// Barrier for synchronizing multiple threads
class Barrier {
private:
    std::mutex mutex;
    std::condition_variable cv;
    size_t threshold;
    size_t count;
    size_t generation;

public:
    explicit Barrier(size_t numThreads);

    void wait();
    void reset();
};

// Condition Variable wrapper with better interface
class ConditionVariable {
private:
    std::condition_variable cv;
    std::mutex mtx;
    bool ready;

public:
    ConditionVariable();

    void wait();
    template<typename Predicate>
    void wait(Predicate pred);

    bool waitFor(std::chrono::milliseconds timeout);

    void notify();
    void notifyAll();
    void setReady(bool r);
};

// Spinlock implementation
class SpinLock {
private:
    std::atomic_flag flag;

public:
    SpinLock();

    void lock();
    bool tryLock();
    void unlock();
};

} // namespace PTManager


#endif //PROCESS_THREAD_MANAGER_SYNCHRONIZATION_H