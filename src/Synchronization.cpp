#include "Synchronization.h"
#include <iostream>

namespace PTManager {

// ===== SafeMutex Implementation =====

/**
 * @brief Constructs a SafeMutex with deadlock detection capabilities
 *
 * @param mutexName Identifier for debugging and error messages (defaults to "unnamed")
 *
 * Initializes the mutex with no owner and zero lock count.
 * The name helps identify which mutex is involved in deadlock situations.
 */
SafeMutex::SafeMutex(const std::string& mutexName)
    : name(mutexName.empty() ? "unnamed" : mutexName),
      owner(std::thread::id()), lockCount(0) {}

/**
 * @brief Attempts to acquire the mutex with timeout and recursive lock detection
 *
 * @param timeout Maximum time to wait for the lock
 * @return true if lock was acquired, false on timeout or recursive lock attempt
 *
 * Detects and prevents recursive locking by the same thread, which would cause deadlock.
 * Uses timed locking to detect potential deadlocks when timeout expires.
 * Updates owner thread ID and increments lock count on successful acquisition.
 */
bool SafeMutex::lock(std::chrono::milliseconds timeout) {
    auto thisThreadId = std::this_thread::get_id();

    // Check for recursive locking (potential deadlock)
    if (owner.load() == thisThreadId) {
        std::cerr << "Warning: Thread " << thisThreadId
                  << " attempting to recursively lock mutex '" << name << "'" << std::endl;
        return false;
    }

    if (mtx.try_lock_for(timeout)) {
        owner.store(thisThreadId);
        lockCount++;
        return true;
    }

    std::cerr << "Deadlock warning: Thread " << thisThreadId
              << " timeout waiting for mutex '" << name << "'" << std::endl;
    return false;
}

/**
 * @brief Attempts to acquire the mutex without blocking
 *
 * @return true if lock was acquired, false if already locked
 *
 * Non-blocking lock attempt. Returns immediately regardless of lock availability.
 * Updates owner and lock count on success.
 */
bool SafeMutex::tryLock() {
    auto thisThreadId = std::this_thread::get_id();

    if (mtx.try_lock()) {
        owner.store(thisThreadId);
        lockCount++;
        return true;
    }
    return false;
}

/**
 * @brief Releases the mutex
 *
 * Clears the owner thread ID, decrements lock count, and unlocks the underlying mutex.
 * Caller must ensure they own the lock before calling.
 */
void SafeMutex::unlock() {
    owner.store(std::thread::id());
    lockCount--;
    mtx.unlock();
}

/**
 * @brief Checks if the mutex is currently locked
 *
 * @return true if any thread owns the lock, false otherwise
 *
 * Thread-safe query that checks if owner is set to a valid thread ID.
 */
bool SafeMutex::isLocked() const {
    return owner.load() != std::thread::id();
}

// ===== SafeLockGuard Implementation =====

/**
 * @brief RAII wrapper that acquires a SafeMutex and automatically releases it
 *
 * @param m The SafeMutex to lock
 * @throws std::runtime_error if lock acquisition fails
 *
 * Acquires the mutex in the constructor using the default timeout.
 * Throws exception if lock cannot be acquired, preventing silent failures.
 * Ensures the lock is always released when the guard goes out of scope.
 */
SafeLockGuard::SafeLockGuard(SafeMutex& m) : mutex(m), locked(false) {
    locked = mutex.lock();
    if (!locked) {
        throw std::runtime_error("Failed to acquire lock on mutex: " + mutex.getName());
    }
}

/**
 * @brief Destructor that releases the mutex if it was successfully locked
 *
 * Automatically unlocks the mutex when the guard goes out of scope.
 * Only unlocks if the lock was successfully acquired in the constructor.
 */
SafeLockGuard::~SafeLockGuard() {
    if (locked) {
        mutex.unlock();
    }
}

// ===== Semaphore Implementation =====

/**
 * @brief Constructs a POSIX semaphore with initial value
 *
 * @param value Initial count of available resources
 * @param semName Identifier for debugging (defaults to "unnamed")
 *
 * Creates an unnamed semaphore (pshared=0) for thread synchronization within a process.
 * The initial value represents the number of resources immediately available.
 * Logs error if initialization fails.
 */
Semaphore::Semaphore(unsigned int value, const std::string& semName)
    : name(semName.empty() ? "unnamed" : semName), initialized(false) {

    if (sem_init(&sem, 0, value) == 0) {
        initialized = true;
    } else {
        std::cerr << "Failed to initialize semaphore '" << name << "': "
                  << strerror(errno) << std::endl;
    }
}

/**
 * @brief Destructor that cleans up semaphore resources
 *
 * Destroys the semaphore if it was successfully initialized.
 * Behavior is undefined if threads are still waiting on the semaphore.
 */
Semaphore::~Semaphore() {
    if (initialized) {
        sem_destroy(&sem);
    }
}

/**
 * @brief Decrements the semaphore, blocking if value is zero
 *
 * @return true on success, false if semaphore not initialized
 *
 * Blocks the calling thread until the semaphore value becomes greater than zero,
 * then atomically decrements it and returns.
 */
bool Semaphore::wait() {
    if (!initialized) return false;
    return sem_wait(&sem) == 0;
}

/**
 * @brief Attempts to decrement the semaphore without blocking
 *
 * @return true if semaphore was decremented, false if would block or not initialized
 *
 * Returns immediately. If semaphore value is zero, returns false without waiting.
 */
bool Semaphore::tryWait() {
    if (!initialized) return false;
    return sem_trywait(&sem) == 0;
}

/**
 * @brief Attempts to decrement the semaphore with timeout
 *
 * @param timeout Maximum time to wait
 * @return true if semaphore was decremented, false on timeout or not initialized
 *
 * Converts relative timeout to absolute time required by sem_timedwait().
 * Handles nanosecond overflow when adding timeout to current time.
 */
bool Semaphore::timedWait(std::chrono::milliseconds timeout) {
    if (!initialized) return false;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    long nsec = ts.tv_nsec + (timeout.count() % 1000) * 1000000;
    ts.tv_sec += timeout.count() / 1000 + nsec / 1000000000;
    ts.tv_nsec = nsec % 1000000000;

    return sem_timedwait(&sem, &ts) == 0;
}

/**
 * @brief Increments the semaphore and wakes one waiting thread
 *
 * @return true on success, false if not initialized
 *
 * Atomically increments the semaphore value. If threads are waiting,
 * one is unblocked to proceed.
 */
bool Semaphore::post() {
    if (!initialized) return false;
    return sem_post(&sem) == 0;
}

/**
 * @brief Queries the current value of the semaphore
 *
 * @return Current semaphore count, or -1 if not initialized or query failed
 *
 * Returns the number of resources currently available.
 * Value may change immediately after this call in multi-threaded contexts.
 */
int Semaphore::getValue() {
    if (!initialized) return -1;

    int value;
    if (sem_getvalue(&sem, &value) == 0) {
        return value;
    }
    return -1;
}

// ===== RWLock Implementation =====

/**
 * @brief Constructs a reader-writer lock with writer preference
 *
 * Initializes counters for tracking active readers, writers, and waiting writers.
 * Writer preference prevents reader starvation of writers.
 */
RWLock::RWLock() : readers(0), writers(0), waitingWriters(0) {}

/**
 * @brief Acquires a read lock (shared access)
 *
 * Blocks if any writer is active or waiting. Multiple readers can hold
 * the lock simultaneously. Writer preference policy: waits for all
 * waiting writers to get their turn before allowing new readers.
 */
void RWLock::readLock() {
    std::unique_lock<std::mutex> lock(mutex);

    // Wait if there's a writer or waiting writers (writers have priority)
    while (writers > 0 || waitingWriters > 0) {
        readCV.wait(lock);
    }

    readers++;
}

/**
 * @brief Releases a read lock
 *
 * Decrements reader count. If this was the last reader, signals one
 * waiting writer to proceed.
 */
void RWLock::readUnlock() {
    std::unique_lock<std::mutex> lock(mutex);
    readers--;

    if (readers == 0) {
        writeCV.notify_one();
    }
}

/**
 * @brief Acquires a write lock (exclusive access)
 *
 * Blocks until all readers and writers have released their locks.
 * Only one writer can hold the lock at a time, and no readers are allowed.
 * Increments waitingWriters to prevent new readers from acquiring the lock.
 */
void RWLock::writeLock() {
    std::unique_lock<std::mutex> lock(mutex);
    waitingWriters++;

    // Wait until no readers and no other writers
    while (readers > 0 || writers > 0) {
        writeCV.wait(lock);
    }

    waitingWriters--;
    writers++;
}

/**
 * @brief Releases a write lock
 *
 * Decrements writer count. Preferentially wakes one waiting writer if any exist,
 * otherwise wakes all waiting readers to allow concurrent read access.
 */
void RWLock::writeUnlock() {
    std::unique_lock<std::mutex> lock(mutex);
    writers--;

    // Wake up all waiting readers or one writer
    if (waitingWriters > 0) {
        writeCV.notify_one();
    } else {
        readCV.notify_all();
    }
}

// ===== Barrier Implementation =====

/**
 * @brief Constructs a synchronization barrier for thread coordination
 *
 * @param numThreads Number of threads that must call wait() before any proceed
 *
 * Creates a reusable barrier. Each generation completes when threshold threads arrive.
 */
Barrier::Barrier(size_t numThreads)
    : threshold(numThreads), count(0), generation(0) {}

/**
 * @brief Blocks until all threads reach the barrier
 *
 * When the last thread arrives (count reaches threshold), all waiting threads
 * are released simultaneously. The barrier automatically resets for reuse.
 * Uses generation number to handle spurious wakeups and barrier reuse.
 */
void Barrier::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    size_t gen = generation;

    if (++count == threshold) {
        generation++;
        count = 0;
        cv.notify_all();
    } else {
        cv.wait(lock, [this, gen] { return gen != generation; });
    }
}

/**
 * @brief Resets the barrier to initial state
 *
 * Clears the count and generation. Should only be called when no threads
 * are waiting at the barrier to avoid undefined behavior.
 */
void Barrier::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    count = 0;
    generation = 0;
}

// ===== ConditionVariable Implementation =====

/**
 * @brief Constructs a condition variable with ready flag
 *
 * Initializes the ready flag to false. Threads will block on wait()
 * until the flag is set true and notify() is called.
 */
ConditionVariable::ConditionVariable() : ready(false) {}

/**
 * @brief Blocks until the ready flag becomes true
 *
 * Atomically releases the lock and waits. When signaled, reacquires the lock
 * and checks the ready flag. Handles spurious wakeups by rechecking the predicate.
 */
void ConditionVariable::wait() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return ready; });
}

/**
 * @brief Blocks until a custom predicate becomes true
 *
 * @param pred Callable that returns true when wait should end
 *
 * More flexible version allowing custom wake conditions beyond the ready flag.
 * The predicate is checked after each wakeup to handle spurious wakeups.
 */
template<typename Predicate>
void ConditionVariable::wait(Predicate pred) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, pred);
}

/**
 * @brief Waits with timeout for the ready flag to become true
 *
 * @param timeout Maximum time to wait
 * @return true if ready flag became true, false on timeout
 *
 * Returns false if timeout expires before the ready flag is set,
 * even if notify() was called.
 */
bool ConditionVariable::waitFor(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mtx);
    return cv.wait_for(lock, timeout, [this] { return ready; });
}

/**
 * @brief Wakes one waiting thread
 *
 * If multiple threads are waiting, only one is awakened.
 * The awakened thread will recheck the predicate before proceeding.
 */
void ConditionVariable::notify() {
    std::lock_guard<std::mutex> lock(mtx);
    cv.notify_one();
}

/**
 * @brief Wakes all waiting threads
 *
 * All waiting threads are awakened and will compete for the lock.
 * Each will recheck the predicate - only those for which it's true will proceed.
 */
void ConditionVariable::notifyAll() {
    std::lock_guard<std::mutex> lock(mtx);
    cv.notify_all();
}

/**
 * @brief Updates the ready flag state
 *
 * @param r New value for the ready flag
 *
 * Thread-safe setter for the ready flag. Does not notify waiting threads -
 * caller must explicitly call notify() or notifyAll() after setting ready.
 */
void ConditionVariable::setReady(bool r) {
    std::lock_guard<std::mutex> lock(mtx);
    ready = r;
}

// ===== SpinLock Implementation =====

/**
 * @brief Constructs a spinlock in unlocked state
 *
 * Initializes the atomic flag to clear (unlocked).
 * Spinlocks are efficient for very short critical sections where
 * the overhead of thread sleep/wake would exceed busy-waiting cost.
 */
SpinLock::SpinLock() {
    flag.clear();
}

/**
 * @brief Acquires the spinlock using busy-waiting
 *
 * Continuously attempts to set the flag until successful.
 * Does not yield CPU - keeps spinning. Use only for very short critical sections.
 * Uses acquire memory ordering to ensure proper synchronization.
 */
void SpinLock::lock() {
    while (flag.test_and_set(std::memory_order_acquire)) {
        // Busy wait
    }
}

/**
 * @brief Attempts to acquire the spinlock without waiting
 *
 * @return true if lock was acquired, false if already locked
 *
 * Single attempt to acquire - returns immediately.
 * Uses acquire memory ordering on successful lock.
 */
bool SpinLock::tryLock() {
    return !flag.test_and_set(std::memory_order_acquire);
}

/**
 * @brief Releases the spinlock
 *
 * Clears the flag to allow other threads to acquire the lock.
 * Uses release memory ordering to ensure all prior writes are visible
 * to the thread that acquires the lock next.
 */
void SpinLock::unlock() {
    flag.clear(std::memory_order_release);
}

} // namespace PTManager