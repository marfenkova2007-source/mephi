#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <chrono>

class ThreadPool {
private:
    std::vector<std::function<void()>> tasks;  // vector вместо deque
    mutable std::mutex mutex_;
    std::atomic<size_t> tasksCount_{0};
    std::atomic<bool> terminated_{false};
    std::condition_variable cv_;
    std::vector<std::thread> potoki;
    size_t next_task_index = 0;  // индекс следующей задачи
    
    void Potok(){
        while (true){
            std::function<void()> taska;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while (next_task_index >= tasks.size() && !terminated_.load()){
                    cv_.wait(lock);
                }
                
                if (next_task_index >= tasks.size() && terminated_.load()){
                    return;
                }
                
                if (next_task_index < tasks.size()){
                    taska = std::move(tasks[next_task_index]);
                    next_task_index++;
                    tasksCount_.fetch_sub(1);
                }
            }
            
            if (taska){
                try{
                    taska();
                }
                catch(const std::exception& e){
                    std::cerr << "Taska с исключением внутри" << e.what() << std::endl;
                }
                catch(...){
                    std::cerr << "Ошибки присутствуют в самой таске" << std::endl;
                }
            }
        }
    }
    
public:
    ThreadPool(size_t threadCount) : tasksCount_(0), terminated_(false), next_task_index(0) {
        for (size_t i = 0; i < threadCount; ++i){
            potoki.emplace_back(&ThreadPool::Potok, this);
        }
    }
    
    ~ThreadPool(){
        if (!terminated_){
            Terminate(true);
        }
    }
    
    void PushTask(const std::function<void()>& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (terminated_.load()){
                throw std::logic_error("Мы уже заходили в терминейт, а значит больше не работаем");
            }
            tasks.push_back(task);
            tasksCount_.fetch_add(1);
        }
        cv_.notify_one();
    }
    
    void Terminate(bool wait) {
        if (terminated_.exchange(true)) {
            return;
        }
        terminated_.store(true);
        if (wait){
            while (true){
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (next_task_index >= tasks.size()){
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        else{
            {
                std::lock_guard<std::mutex> lock(mutex_);
                tasks.clear();
                tasksCount_.store(0);
                next_task_index = 0;
            }
        }
        cv_.notify_all();
        for (auto& deistv : potoki){
            if (deistv.joinable()){
                deistv.join();
            }
        }
    }
    
    bool IsActive() const {
        return !terminated_.load();
    }
    
    size_t QueueSize() const {
        return tasksCount_.load();
    }
};