#ifndef PARALLEL_H
#define PARALLEL_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

struct LogMessage
{
    int id;
    std::string msg;
    std::string logMsg;

    bool operator>(const LogMessage& other) const 
    {
        return id > other.id;
    }
};

class ThreadSafeQueue
{
    public:
        void push(const LogMessage& log)
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                queue.push(log);
            }
            cv.notify_one();
        }

        LogMessage wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [this]{
                return !queue.empty();
            });

            LogMessage log = queue.top();
            queue.pop();
            return log;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(mtx);
            return queue.empty();
        }

    private:
        std::priority_queue<
            LogMessage,
            std::vector<LogMessage>,
            std::greater<>
        > queue;
        std::mutex mtx;
        std::condition_variable cv;

};

#endif // PARALLEL_H