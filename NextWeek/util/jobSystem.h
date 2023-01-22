#include <functional>
#include <atomic>
#include <deque>
#include <condition_variable>

class JobSystem {
    struct JobArgs
    {
        uint32_t jobIndex;
    };
    
    struct Job {
        std::function<void(JobArgs)> func;
    }

    struct Jobqueue
    {
        std::deque<Job> queue;
    };
    
    struct WorkerState
    {
        std::condition_variable cv;
    };
    

    struct InternalState
    {
        
    } static jobState;
    
    struct context
    {
        std::atomic<uint32_t> counter {0};
    };

    void Excute(std::function<void(JobArgs)>& task);
};

