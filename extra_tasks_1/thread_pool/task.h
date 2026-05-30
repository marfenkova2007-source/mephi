#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <chrono>

/*
 * Требуется написать класс ThreadPool, реализующий пул потоков, которые выполняют задачи из общей очереди.
 * С помощью метода PushTask можно положить новую задачу в очередь
 * С помощью метода Terminate можно завершить работу пула потоков.
 * Если в метод Terminate передать флаг wait = true,
 *  то пул подождет, пока потоки разберут все оставшиеся задачи в очереди, и только после этого завершит работу потоков.
 * Если передать wait = false, то все невыполненные на момент вызова Terminate задачи, которые остались в очереди,
 *  никогда не будут выполнены.
 * После вызова Terminate в поток нельзя добавить новые задачи.
 * Метод IsActive позволяет узнать, работает ли пул потоков. Т.е. можно ли подать ему на выполнение новые задачи.
 * Метод GetQueueSize позволяет узнать, сколько задач на данный момент ожидают своей очереди на выполнение.
 * При создании нового объекта ThreadPool в аргументах конструктора указывается количество потоков в пуле. Эти потоки
 *  сразу создаются конструктором.
 * Задачей может являться любой callable-объект, обернутый в std::function<void()>.
 */



class ThreadPool {
private:
    std::queue<std::function<void()>> tasks;
    mutable std::mutex mutex_;
    std::atomic<size_t> tasksCount_{0}; //пусть будет кол-во 
    std::atomic<bool> terminated_{false}; 

    std::condition_variable cv_; 
    
    std::vector<std::thread> potoki;
    
    
    void Potok(){
        while (true){
            std::function<void()> taska;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                //пока нет задач, но в терм мы не заходили
                while (tasks.empty() and !terminated_.load()){
                    cv_.wait(lock);
                }

                if (tasks.empty() and terminated_.load()){
                    return; //просто пиздуем так как заходили в терминэйт
                }
                if (!tasks.empty()){
                    taska = std::move(tasks.front());
                    tasks.pop();
                    tasksCount_.fetch_sub(1); //-1 
                }
            }
            
            if (taska){
                try{
                    taska();
                }
                catch(const std::exception& e){
                    std::cerr<<"Taska с исключением внутри"<<e.what()<<std::endl;
                }
                catch(...){
                    std::cerr<<"Ошибки присутствуют в самой таске"<<std::endl;
                }
            }
        }
    }

public:
    ThreadPool(size_t threadCount): tasksCount_(0), terminated_(false) {
        for (size_t i = 0; i<threadCount; ++i){
            potoki.emplace_back(&ThreadPool::Potok, this);
        }
    }
    ~ThreadPool(){
        //term false то надо принудительно его
        if (!terminated_){
            Terminate(true);
        }
    }

    void PushTask(const std::function<void()>& task) {
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (terminated_.load()){ //не имеем права работать
                throw std::logic_error("Мы уже заходили в терминейт, а значит больше не работаем");
            }
            tasks.push(task);
            tasksCount_.fetch_add(1);
        }
        cv_.notify_one();

    }

    

    void Terminate(bool wait) {
        if (terminated_.exchange(true)) {
            return; 
        }
        terminated_.store(true); //мы идём на завершение всего
        if (wait){
            while (true){
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (tasks.empty()){
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        else{
            {
                std::lock_guard<std::mutex> lock(mutex_);
                while (!tasks.empty()) {
                    tasks.pop();
                    tasksCount_.fetch_sub(1);
                }
            }
        }
        cv_.notify_all();
        for (auto& deistv:potoki){
            if (deistv.joinable()){
                deistv.join();
            }
        }
    }

    bool IsActive() const {
        //когда терм плохой, то всё хорошо
        return !terminated_.load();
    }

    size_t QueueSize() const {
        return tasksCount_.load();
    }
};

