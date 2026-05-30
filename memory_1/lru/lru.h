
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <list>
#include <unordered_map>

using namespace std;
// я честно пыталась реаизовать через деку, но не хочет оно быстро работать
template<typename T>
class Deque {
private:
    std::vector<T> buffer;
    size_t front_index;
    size_t back_index;
    size_t size_;
    size_t capacity_;
public:
    Deque()
    :buffer(0)
    , front_index(0)
    , back_index(0)
    , size_(0)
    , capacity_(0){}
    
    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        std::vector<T> new_buffer(new_capacity);
        for (size_t i = 0; i < size_; ++i) {
            new_buffer[i] = (*this)[i];
        }
        
        buffer = std::move(new_buffer);
        front_index = 0;
        back_index = size_;
        capacity_ = new_capacity;
    }

    void Resize(size_t new_size, const T& default_value = T()) {
        if (new_size == size_) {
            return;
        }
    
        if (new_size < size_) {
            back_index = (front_index + new_size) % capacity_;
            size_ = new_size;
            if (size_ == 0) {
                front_index = back_index = 0;
            }
        } 
        else {
            if (new_size > capacity_) {
                size_t new_capacity = std::max(capacity_ * 2, new_size);
                std::vector<T> new_buffer(new_capacity);
                for (size_t i = 0; i < size_; ++i) {
                    new_buffer[i] = (*this)[i];
                }
                
                buffer = std::move(new_buffer);
                front_index = 0;
                capacity_ = new_capacity;
            }
            for (size_t i = size_; i < new_size; ++i) {
                buffer[(front_index + i) % capacity_] = default_value;
            }
            
            back_index = (front_index + new_size) % capacity_;
            size_ = new_size;
        }
    };

    size_t Size() const{
        return size_;
    };
    bool Empty() const {
        return size_ == 0;
    };
    void PushBack(const T& value) {
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        
        if (size_ == 0) {
            if (capacity_ == 0) {  
              capacity_ = 1;
            buffer.resize(1);}
            buffer[0] = value;
            front_index = 0;
            back_index = 1;
        } else {
            buffer[back_index] = value;
            back_index = (back_index + 1) % capacity_;
        }
        size_++;
    };


    void PushFront(const T& value) {
        if (size_ == capacity_) {
            Reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        
        if (size_ == 0) {
            if (capacity_ == 0) {  
             capacity_ = 1;
             buffer.resize(1);}
            buffer[0] = value;
            front_index = 0;
            back_index = 1;
        } else {
            front_index = (front_index - 1 + capacity_) % capacity_;
            buffer[front_index] = value;
        }
        size_++;
    };
    void PopBack() {
        if (Empty()) {
            return;
        }
        
        back_index = (back_index - 1 + capacity_) % capacity_;
        --size_;
        
        if (size_ == 0) {
            front_index = 0;
            back_index = 0;
        }
    };
    void PopFront() {
        if (Empty()) {
            return;
        }
        front_index = (front_index + 1) % capacity_;
        --size_;
        if (size_ == 0) {
            front_index = 0;
            back_index = 0;
        }
    };
    void Clear() {
        capacity_ = 1;
        front_index = 0;
        back_index = 0;
        size_ = 0;
        buffer.clear();
    };
    T& Front() {
        return buffer[front_index];
    };
    const T& Front() const {
        return buffer[front_index];
    };
    T& Back() {
        return buffer[(back_index - 1 + capacity_) % capacity_];
    };
    const T& Back() const {
        return buffer[(back_index - 1 + capacity_) % capacity_];
    };
    bool operator<(const Deque<T>& other) const {
        size_t m = std::min(size_, other.size_);
        for (size_t i = 0; i < m; ++i) {
            if ((*this)[i] < other[i]) {
                return true;
            }
        }
        return false;
    };
    bool operator>(const Deque<T>& other) const {
        return other < *this;
    };
    bool operator<=(const Deque<T>& other) const {
        return !(other < *this);
    }
    bool operator>=(const Deque<T>& other) const {
        return !(*this < other);
    }
    bool operator==(const Deque<T>& other) const {
        if (size_ != other.size_) {
            return false;
        }
        
        for (size_t i = 0; i < size_; ++i) {
            if ((*this)[i] != other[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Deque<T>& other) const {
        return !(*this == other);
    }
    
    
    T& operator[](size_t index) {
        return buffer[(front_index + index) % capacity_];
    }

    const T& operator[](size_t index) const {
        return buffer[(front_index + index) % capacity_];
    }
};

template <typename K, typename V>
class LruCache {
private:
    size_t max_size_;
    size_t curr_size_ = 0;
    list<pair<K, V>>kash;
    unordered_map<K, typename list<pair<K, V>>::iterator> mas;
    void MoveToFront(size_t ind) {
        /*if (ind == 0){
            return;
        }
        auto element = kash[ind];
        for (size_t i = ind; i>0; --i) {
            kash[i] = kash[i - 1];
            marshryt_from_key_to_index[kash[i].first] = i;
        }
        kash[0] = element;
        marshryt_from_key_to_index[element.first] = 0;*/
    }

public:
    LruCache(size_t max_size): max_size_(max_size){
        //marshryt_from_key_to_index.reserve(max_size * 2);
    };

    void Put(const K& key, const V& value){
        auto element = mas.find(key);
        if (element!= mas.end()){
            /*size_t ind = element ->second;
            kash[ind].second = value;
            MoveToFront(ind);*/
            auto i = element->second;
            i->second = value;
            kash.splice(kash.begin(), kash, i);
            return;
        }
        if (kash.size()>=max_size_){
            K new_key = kash.back().first;
            mas.erase(new_key);
            kash.pop_back();
        }

        kash.push_front({key, value});
        mas[key] = kash.begin();
        ++curr_size_;

        
        /*for (size_t i = 1; i <= old_size; ++i) {
            marshryt_from_key_to_index[kash[i].first] = i;
        }*/

    };

    bool Get(const K& key, V* value){
        auto element = mas.find(key);
        if (element==mas.end()){
            return false;
        }
        auto ind = element->second;
        *value = ind->second;
        //MoveToFront(ind);
        kash.splice(kash.begin(), kash, ind);
        return true;
    };
};
