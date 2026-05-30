#pragma once
#include <iostream>
#include <algorithm>

template<typename T>
class Array {
public:
    class Iterator{
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using value_type = T;
        using pointer = T*;

        bool operator==(const Iterator & other) const {
            return ptr_ == other.ptr_;
        }
        bool operator!=(const Iterator& other) const{
            return !(*this==other);
        }
        bool operator<(const Iterator& other) const{
            return ptr_<other.ptr_;
        }
        bool operator<=(const Iterator& other) const{
            return *this==other or *this<other;
        }
        bool operator>(const Iterator& other) const{
            return !(*this <= other);
        }
        bool operator>=(const Iterator& other) const{
            return *this == other or *this > other;
        }
        Iterator& operator++(){
            ++ptr_;
            return *this;
        }
        Iterator operator++(int){
            Iterator prevValue = *this;
            ++ptr_;
            return prevValue;
        }
        Iterator& operator--(){
            --ptr_;
            return *this;
        }
        Iterator operator--(int){
            Iterator prevValue = *this;
            --ptr_;
            return prevValue;
        }
        Iterator& operator+=(difference_type n){
            ptr_+= n;
            return *this;
        }
        Iterator operator +(difference_type n){
            return Iterator(*this) +=n;
        }
        Iterator& operator-=(difference_type n){
            return *this += -n;
        }
        Iterator operator -(difference_type n){
            return Iterator(*this) -= n;
        }
        Iterator operator +(difference_type n) const{
            return Iterator(ptr_ +n);
        }
        Iterator operator -(difference_type n) const{
            return Iterator(ptr_  - n);
        }
        difference_type operator-(const Iterator& other) const{
            return ptr_ - other.ptr_;
        }
        reference operator *(){
            return *ptr_;
        }
        reference operator*() const{
            return *ptr_;
        }
        pointer operator ->(){
            return ptr_;
        }
        pointer operator ->() const{
            return ptr_;
        }
        T& operator [](difference_type n) {
            return ptr_[n];
        }
        reference operator[](difference_type n) const{
            return ptr_[n];
        }
        friend Iterator operator+(difference_type n, const Iterator& other) {
            return Iterator(other.ptr_ + n);
        }
        

    private:
        T *ptr_;

        explicit Iterator(T *ptr) : ptr_(ptr){}

        friend Array;
    };

    Array(std::ostream& ostream) 
        : Ostream_(ostream)
        , data_(new T[2]())
        , capacity_(2)
        , size_(0)
        {
        Ostream_<<"Constructed. "<<*this<<"\n";
    }

    Array(const Array<T>& array) 
        : Ostream_(array.Ostream_)
        , data_(new T[array.capacity_])
        , capacity_(array.capacity_)
        , size_(array.size_)
    {
        for (size_t i = 0; i<array.size_; ++i){
            data_[i] = array.data_[i];
        }
        Ostream_ <<"Constructed from another Array. "<<*this<<"\n";
    };

    Array(size_t size, std::ostream& ostream = std::cout, T defaultValue = T()) : Ostream_(ostream)
        , data_(new T[2*size]())
        , capacity_(size*2)
        , size_(size)
    
    {
        for (size_t i=0; i<size_;++i){
            data_[i] = defaultValue;
        }
        Ostream_ << "Constructed with default. "<<*this<<"\n";
    };

    ~Array() {
        Ostream_ <<"Destructed "<<size_<<"\n";
        delete[] data_;
    }
    Iterator begin(){
        return Iterator(data_);
    }
    Iterator begin() const{
        return Iterator(data_);

    }
    Iterator end(){
        return Iterator(data_+size_);
    }
    Iterator end() const{
        return Iterator(data_+size_);
    }
    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    void Reserve(size_t newCapacity) {
        if (newCapacity > capacity_){
            T* new_data = new T[newCapacity];
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
            capacity_ = newCapacity;
        }
    }

    void Resize(size_t newSize) {
        if (capacity_<newSize){
            Reserve(newSize);
        }
        if (size_<newSize){
            for (size_t i=size_; i<newSize; ++i){
                data_[i] = 0;
            }
        }
        size_ = newSize;
    }

    void PushBack(T value = 0) {
        if (capacity_==size_){
            capacity_  = capacity_*2;
            T* new_data = new T[capacity_];
            for (size_t i=0; i<size_; ++i){
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
        }
        data_[size_] = value;
        ++size_;
    }

    void PopBack() {
        if (size_>0){
            --size_;
        }
    }

    const T& operator [](const size_t i) const {
         return data_[i];
    }

    T& operator [](const size_t i) {
         return data_[i];
    }

    explicit operator bool() const {
        if (size_==0){
            return false;
        }
        return true;
    }

    bool operator <(const Array<T>& it) const {
        for (size_t i=0; i<std::min(size_, it.size_); ++i){
            if (data_[i]<it.data_[i]){
                return true;
            }
            if (data_[i]>it.data_[i]){
                return false;
            }
        }
            if (size_<it.size_){
                return true;
            }
            return false;
    }

    bool operator >(const Array<T>& it) const {
        return it < *this;
    }

    bool operator !=(const Array<T>& it) const {
        return ((it<*this) or (*this<it));
    }

    bool operator ==(const Array<T>& it) const {
        return !((it<*this) and (*this<it));
    }

    bool operator <=(const Array<T>& it) const {
        return (*this<it or *this==it);
    }

    bool operator >=(const Array<T>& it) const {
        return (*this>it or *this==it);
    }

    Array<T>& operator <<(const T& value) {
        PushBack(value);
        return *this;
    }

    Array<T>& operator <<(const Array<T>& it) {
        if (size_ + it.size_ >capacity_){
            Reserve(size_+it.size_);
        }
        int ind = 0;
        for (auto i = size_; i<(size_+it.size_); ++i){
            data_[i] = it.data_[ind];
            ++ind;
        }
        size_ = size_ + it.size_;
        return *this;
    }

    bool Insert(size_t pos, const T& value) {
        if (pos>size_){
            return false;
        }
        if (size_>=capacity_){
            Reserve(capacity_*2);
        }
        else if (pos == 0){
            T* new_data = new T[capacity_];
            new_data[0] = value;
            for (size_t i= 1; i<=size_; ++i){
                new_data[i] = data_[i-1];
            }
            delete[] data_;
            data_ = new_data;
            ++size_;
        }
        else if(pos==size_){
            data_[size_] = value;
            ++size_;
        }
        else{
            T* new_data = new T[capacity_];
            for (size_t i=0; i<pos; ++i){
                new_data[i] = data_[i];
            }
            new_data[pos] = value;
            for (size_t i = pos; i<size_; ++i){
                new_data[i+1] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
            ++size_;
        }
        return true;
    }

    bool Erase(size_t pos) {
        if (size_-1<pos){
            return false;
        }
        for (size_t i = pos; i < size_ - 1; ++i) {
        data_[i] = data_[i + 1];
        }
        --size_;

        return true;
    }

    void Clear(){
        size_ = 0;
    }

private:
    // ToDo
    std::ostream& Ostream_;
    T *data_;
    size_t capacity_;
    size_t size_;
    
};

template<typename T>
std::ostream& operator <<(std::ostream& ostream, const Array<T>& array) {
    ostream<<"Result Array's capacity is "<<array.Capacity()<<" and size is "<<array.Size();
        if (array.Size()!=0){
           ostream<<", elements are: ";
            for (size_t i=0; i<array.Size()-1;++i){
                ostream<<array[i]<<", ";
            }
            ostream<<array[array.Size()-1];
        }
        return ostream;
}


