#ifndef PROCESS_THREAD_MANAGER_THREADPOOL_H
#define PROCESS_THREAD_MANAGER_THREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <memory>

namespace PTManager {

enum class ThreadState {
    IDLE,
    RUNNING,
    BLOCKED,
    TERMINATED
};

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<size_t> activeTasks;

    std::vector<ThreadState> workerStates;
    std::mutex stateMutex;

    void workerThread(size_t id);

public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Disable copy and move
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Task submission
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<std::result_of_t<F(Args...)>>;

    // Pool management
    size_t getPoolSize() const { return workers.size(); }
    size_t getActiveTasks() const { return activeTasks.load(); }
    size_t getQueuedTasks();

    void waitForCompletion();
    void shutdown();

    // Thread state monitoring
    ThreadState getThreadState(size_t id);
    void printThreadStates();
};

// Template implementation must be in header
template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<std::result_of_t<F(Args...)>>
{
    using return_type = std::result_of_t<F(Args...)>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex);

        if (stop) {
            throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
        }

        tasks.emplace([task](){ (*task)(); });
    }

    condition.notify_one();
    return res;
}

} // namespace PTManager

#endif //PROCESS_THREAD_MANAGER_THREADPOOL_H