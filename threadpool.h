#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool(int threadsCount = 4);
    void AddJob(std::function<void()> function);
    bool IsBusy();
    void Terminate();
    std::thread::id GetThreadID(int ThreadIndex);
private:
    void Wait();
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    std::mutex jobsMutex;
    bool shouldTerminate = false;
    std::condition_variable taskCondition;
};

#endif // THREADPOOL_H
