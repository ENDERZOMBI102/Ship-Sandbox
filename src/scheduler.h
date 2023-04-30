#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <mutex>
#include <queue>
#include <thread>

class scheduler
{
public:
    struct task
    {
        virtual void process() = 0;
        virtual ~task() {}
    };
private:
    class thread: public std::thread
    {
        scheduler *parent;
        task *currentTask;
    public:
        int name;
        thread(scheduler *_parent);
        static void enter(void *_this);
    };
    class semaphore
    {
        std::mutex m;
        std::condition_variable condition;
        unsigned long count_;
    public:
        semaphore(): count_(0) {}
        void signal();
        void wait();
    };
    std::uint32_t nthreads;
    std::vector <thread*> threadPool;
    semaphore available;
    semaphore completed;
    std::queue<task*> tasks;
    std::mutex critical;
public:
    scheduler();
    ~scheduler();
    void schedule(task *t);
    void wait();
    int getNThreads();
};


#endif // _SCHEDULER_H_
