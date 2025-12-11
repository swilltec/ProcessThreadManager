#include "ThreadPool.h"
#include <iostream>

namespace PTManager {

/**
 * @brief Constructs a thread pool with specified number of worker threads
 *
 * @param numThreads Number of worker threads to create in the pool
 *
 * Initializes all worker threads in IDLE state and starts them immediately.
 * Each worker runs the workerThread() function, waiting for tasks to be enqueued.
 * Workers persist until shutdown() is called.
 */
ThreadPool::ThreadPool(size_t numThreads)
    : stop(false), activeTasks(0) {

    workerStates.resize(numThreads, ThreadState::IDLE);

    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this, i] { workerThread(i); });
    }

    std::cout << "ThreadPool created with " << numThreads << " threads" << std::endl;
}

/**
 * @brief Destructor that ensures graceful shutdown of the thread pool
 *
 * Calls shutdown() to stop all workers and wait for them to finish.
 * Ensures no thread handles are left dangling and all resources are cleaned up.
 */
ThreadPool::~ThreadPool() {
    shutdown();
}

/**
 * @brief Main execution loop for worker threads
 *
 * @param id Unique identifier for this worker thread (0-based index)
 *
 * Continuously waits for tasks in the queue, executes them, and updates thread state.
 * Transitions: IDLE (waiting) -> RUNNING (executing) -> IDLE (waiting) -> TERMINATED (shutdown).
 * Handles exceptions within tasks to prevent worker thread termination.
 * Exits when stop flag is set and task queue is empty.
 */
void ThreadPool::workerThread(size_t id) {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            {
                std::lock_guard<std::mutex> stateLock(stateMutex);
                workerStates[id] = ThreadState::IDLE;
            }

            condition.wait(lock, [this] {
                return stop.load() || !tasks.empty();
            });

            if (stop.load() && tasks.empty()) {
                std::lock_guard<std::mutex> stateLock(stateMutex);
                workerStates[id] = ThreadState::TERMINATED;
                return;
            }

            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        if (task) {
            {
                std::lock_guard<std::mutex> stateLock(stateMutex);
                workerStates[id] = ThreadState::RUNNING;
            }

            activeTasks++;

            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "Thread " << id << " caught exception: "
                          << e.what() << std::endl;
            }

            activeTasks--;
        }
    }
}

/**
 * @brief Returns the number of tasks waiting in the queue
 *
 * @return Number of pending tasks not yet assigned to workers
 *
 * Thread-safe query of the task queue size. Does not include tasks
 * currently being executed by workers.
 */
size_t ThreadPool::getQueuedTasks() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return tasks.size();
}

/**
 * @brief Blocks until all queued and active tasks complete
 *
 * Polls the queue and active task count every 10ms until both are zero.
 * Does not prevent new tasks from being enqueued during the wait.
 * Useful for synchronization points where all work must finish before proceeding.
 */
void ThreadPool::waitForCompletion() {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (tasks.empty() && activeTasks.load() == 0) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/**
 * @brief Gracefully shuts down the thread pool
 *
 * Sets the stop flag, wakes all waiting workers, and joins all threads.
 * Workers will complete their current tasks but won't pick up new ones.
 * Remaining queued tasks are discarded. Safe to call multiple times.
 * After shutdown, the thread pool cannot be restarted.
 */
void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop.load()) {
            return;  // Already stopped
        }
        stop = true;
    }

    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    std::cout << "ThreadPool shut down" << std::endl;
}

/**
 * @brief Queries the current state of a specific worker thread
 *
 * @param id Thread identifier (0-based index)
 * @return Current ThreadState, or TERMINATED if id is invalid
 *
 * Thread-safe query of worker state. States are updated by the worker itself:
 * IDLE while waiting, RUNNING while executing a task, TERMINATED after shutdown.
 */
ThreadState ThreadPool::getThreadState(size_t id) {
    if (id >= workerStates.size()) {
        return ThreadState::TERMINATED;
    }

    std::lock_guard<std::mutex> lock(stateMutex);
    return workerStates[id];
}

/**
 * @brief Prints a comprehensive status report of the thread pool
 *
 * Displays pool size, active task count, queued task count, and the state
 * of each individual worker thread. Thread-safe snapshot of current state.
 * Useful for debugging and monitoring thread pool health and utilization.
 */
void ThreadPool::printThreadStates() {
    std::lock_guard<std::mutex> lock(stateMutex);

    std::cout << "\n=== Thread Pool Status ===" << std::endl;
    std::cout << "Pool size: " << workers.size() << std::endl;
    std::cout << "Active tasks: " << activeTasks.load() << std::endl;
    std::cout << "Queued tasks: " << getQueuedTasks() << std::endl;

    for (size_t i = 0; i < workerStates.size(); ++i) {
        std::cout << "Thread " << i << ": ";
        switch (workerStates[i]) {
            case ThreadState::IDLE: std::cout << "IDLE"; break;
            case ThreadState::RUNNING: std::cout << "RUNNING"; break;
            case ThreadState::BLOCKED: std::cout << "BLOCKED"; break;
            case ThreadState::TERMINATED: std::cout << "TERMINATED"; break;
        }
        std::cout << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

} // namespace PTManager