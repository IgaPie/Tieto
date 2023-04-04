#include "threadpool.h"

ThreadPool::ThreadPool(int threadsCount)
{
    threads.resize(threadsCount);
    for(int i = 0; i < threadsCount; i++){
        threads[i] = std::thread(&ThreadPool::Wait, this);
    }
}

void ThreadPool::AddJob(std::function<void()> function)
{
    {
        std::lock_guard lk(jobsMutex);
        jobs.push(function);
    }
    taskCondition.notify_one();
}
bool ThreadPool::IsBusy() {
    bool poolbusy;
    std::lock_guard lock(jobsMutex);
    return !jobs.empty();
}

std::thread::id ThreadPool::GetThreadID(int ThreadIndex){
    return threads[ThreadIndex].get_id();
}

void ThreadPool::Terminate() {
    {
        std::lock_guard lock(jobsMutex);
        shouldTerminate = true;
    }
    taskCondition.notify_all();
    for (std::thread& thread : threads) {
        thread.join();
    }
    threads.clear();
}

void ThreadPool::Wait()
{
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(jobsMutex);
            taskCondition.wait(lock, [this] {
                return !jobs.empty() || shouldTerminate;
            });
            if (shouldTerminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}
