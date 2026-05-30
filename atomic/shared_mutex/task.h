#pragma once

#include <atomic>


class SharedMutex {
private:
    std::atomic<int> kol_read{0};
    std::atomic<bool> writing{false};

public:

    SharedMutex() : kol_read(0), writing(false) {}

    void lock() {
        while (true) {
            bool pok = false;
            if (writing.compare_exchange_weak(pok, true, std::memory_order_acquire)) {
                while (kol_read.load(std::memory_order_acquire) > 0) {
                    __builtin_ia32_pause(); // просто с пустым вайлом ему не нравится, значит попробуем паузой
                }
                break;
            }
            __builtin_ia32_pause();
        }
    }

    void unlock() {
        writing.store(false, std::memory_order_release);
    }

    void lock_shared() {
        while (true) {
            if (!writing.load(std::memory_order_acquire)) {
                kol_read.fetch_add(1, std::memory_order_acquire);
                if (writing.load(std::memory_order_acquire)) {
                    kol_read.fetch_sub(1, std::memory_order_release);
                }
                else {
                    break;
                }
            }
            __builtin_ia32_pause();
        }
    }

    void unlock_shared() {
        kol_read.fetch_sub(1, std::memory_order_release);
    }
};