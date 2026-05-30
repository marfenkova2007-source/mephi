#pragma once

#include <mutex>
#include <condition_variable>
#include <optional>

class TimeOut : public std::exception {
    const char* what() const noexcept override {
        return "Timeout";
    }
};

template<typename T>
class UnbufferedChannel {
private:
    std::optional<T> value;
    bool chanal_busy = false;
    int kol_writerow =0;
    //bool writer_in_stupor = false;

    std::mutex mtx_;
    std::condition_variable cv_get_; 
    std::condition_variable cv_put_; 
    std::condition_variable ne_write_wait_read_;    


public:
    void Put(const T& data) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        while (chanal_busy || value.has_value()) {
            cv_put_.wait(lock);
        }
        
        chanal_busy = true;
        value = data;
        cv_get_.notify_one();
        
        while (value.has_value()) {
            ne_write_wait_read_.wait(lock);
        }
        
        chanal_busy = false;
        cv_put_.notify_one();
    }

    T Get(std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        if (!value.has_value()) {
            if (timeout == std::chrono::milliseconds(0)) {
                while (!value.has_value()) {
                    cv_get_.wait(lock);
                }
            } else {
                auto wait_time = std::chrono::steady_clock::now() + timeout;
                while (!value.has_value()) {
                    auto pok = cv_get_.wait_until(lock, wait_time);
                    if (pok == std::cv_status::timeout) {
                        throw TimeOut();
                    }
                }
            }
        }
        
        T read = std::move(*value);
        value.reset();
        ne_write_wait_read_.notify_one();
        
        return read;
    }
};
