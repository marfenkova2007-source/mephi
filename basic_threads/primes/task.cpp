#include "task.h"
#include <chrono>
#include <vector>
#include <stdexcept>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;


PrimeNumbersSet::PrimeNumbersSet()
    :nanoseconds_under_mutex_(0)
    ,nanoseconds_waiting_mutex_(0)
    {}


bool PrimeNumbersSet::IsPrime(uint64_t number) const{
    std::lock_guard<std::mutex> lock(set_mutex_);
    for (auto it = primes_.begin(); it!=primes_.end(); ++it){
        if (number== *it ){
            return true;
        }
    }
    return false;

};

uint64_t PrimeNumbersSet::GetNextPrime(uint64_t number) const{
    std::lock_guard<std::mutex> lock(set_mutex_);
    auto it = primes_.upper_bound(number);
    if (it!= primes_.end()){
        return *it;
    }
    throw std::invalid_argument("No next");
};

bool Prime(uint64_t n) {
    if (n <= 1){
        return false;
    }
    
    for (uint64_t i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    
    return true;
}

void PrimeNumbersSet::AddPrimesInRange(uint64_t from, uint64_t to){
    auto time_ozhidania_begin = std::chrono::steady_clock::now();
    std::unique_lock<std::mutex> lock(set_mutex_);
    auto time_ozhidania_end = std::chrono::steady_clock::now();
    nanoseconds_waiting_mutex_ += std::chrono::duration_cast<std::chrono::nanoseconds>(time_ozhidania_end-time_ozhidania_begin).count();
    lock.unlock();
    std::vector<uint64_t> mas;
    for (uint64_t i=from; i<to; ++i){
        if (Prime(i)){
            mas.push_back(i);
        }
    }
    auto time_work_begin = std::chrono::steady_clock::now();
    lock.lock();
    for (uint64_t p : mas){
        primes_.insert(p); 
    }

    auto time_work_end = std::chrono::steady_clock::now();
    nanoseconds_under_mutex_ += std::chrono::duration_cast<std::chrono::nanoseconds>(time_work_end-time_work_begin).count();
    return;
};


size_t PrimeNumbersSet::GetPrimesCountInRange(uint64_t from, uint64_t to) const{
    std::lock_guard<std::mutex> lock(set_mutex_);
    size_t k = 0;
    auto it = primes_.lower_bound(from);
    while (it != primes_.end() && *it < to) {
        ++k;
        ++it;
    }
    return k;

};


uint64_t PrimeNumbersSet::GetMaxPrimeNumber() const{
    std::lock_guard<std::mutex> lock(set_mutex_);
    if (primes_.empty()){
        return 0;
    }
    //а сразу дойти до этого было явно не судьба 
    return *primes_.rbegin();
}

std::chrono::nanoseconds PrimeNumbersSet::GetTotalTimeWaitingForMutex() const{
    return std::chrono::nanoseconds(nanoseconds_waiting_mutex_.load());
};

std::chrono::nanoseconds PrimeNumbersSet::GetTotalTimeUnderMutex() const{
    return std::chrono::nanoseconds(nanoseconds_under_mutex_.load());

};