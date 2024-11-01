#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    // Submit a task to the thread pool
    template <class F, class... Args>
    auto submit(int priority, F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    // Stop the pool from accepting new tasks and wait for current tasks to finish
    void shutdown();

    static ThreadPool* getInstance()
    {
        std::call_once(creat_flag, []() { pool = new ThreadPool(); });
        return pool;
    }

    static void destroyInstance()
    {
        delete pool;
        pool = nullptr;
    }

private:
    // Worker thread function
    void worker();

    // Task queue with priority
    struct Task
    {
        std::function<void()> func;
        int priority;

        Task(std::function<void()> f, int p) : func(f), priority(p)
        {}

        bool operator<(const Task& other) const
        {
            return priority < other.priority;
        }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop_flag;
    std::atomic<int> idle_threads;

    static ThreadPool* pool;
    static std::once_flag creat_flag;
};

template <class F, class... Args>
auto ThreadPool::submit(int priority, F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{

    using return_type = typename std::invoke_result<F, Args...>::type;

    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // Don't allow submitting after stopping the pool
        if (stop_flag)
            throw std::runtime_error("Submit on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); }, priority); // Default priority 0
    }
    condition.notify_one();
    return result;
}
