#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>


class threadpool
{
    public:
    threadpool(size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            workers.emplace_back([this]()
            {
                while(true)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this](){
                            return stop | !tasks.empty();
                        });

                        if(stop && tasks.empty()) return;

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }   
    }

    void enqueue(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(task);
        }
        cv.notify_one();
    }

    ~threadpool()
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
        }

        cv.notify_all();

        for(auto& w : workers)
        {
            w.join();
        }
    }

    private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
};

#endif