#include <iostream>
#include <algorithm>

class Array {
public:
    Array(std::ostream& ostream)
        : Ostream_(ostream)
        , data_(new int[2]())
        , capacity_(2)
        , size_(0)
        {
        Ostream_<<"Constructed. "<<*this<<"\n";
    };
    Array(const Array& array)
        : Ostream_(array.Ostream_)
        , data_(new int[array.capacity_])
        , capacity_(array.capacity_)
        , size_(array.size_)
    {
        for (size_t i = 0; i<array.size_; ++i){
            data_[i] = array.data_[i];
        }
        Ostream_ <<"Constructed from another Array. "<<*this<<"\n";
    };
    Array(size_t size, std::ostream& ostream = std::cout, int defaultValue = 0)
        : Ostream_(ostream)
        , data_(new int[2*size]())
        , capacity_(size*2)
        , size_(size)
    
    {
        for (size_t i=0; i<size_;++i){
            data_[i] = defaultValue;
        }
        Ostream_ << "Constructed with default. "<<*this<<"\n";
    };
    ~Array()
    {
        Ostream_ <<"Destructed "<<size_<<"\n";
        delete[] data_;
    };

    size_t Size() const{
        return size_;
    };
    size_t Capacity() const{
        return capacity_;
    };
    void Reserve(size_t newCapacity){
        if (newCapacity > capacity_){
        int* new_data = new int[newCapacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = newCapacity;
    }
    };
    void Resize(size_t newSize){
        if (capacity_<newSize){
            Reserve(newSize);
        }
        if (size_<newSize){
            for (auto i=size_; i<newSize; ++i){
                data_[i] = 0;
            }
        }
        size_ = newSize;
    };
    void PushBack(int value = 0){
        if (capacity_==size_){
            capacity_  = capacity_*2;
            int* new_data = new int[capacity_];
            for (size_t i=0; i<size_; ++i){
                new_data[i] = data_[i];
            }
            delete[] data_;
            data_ = new_data;
        }
        data_[size_] = value;
        ++size_;
    };
    void PopBack(){
        if (size_>0){
            --size_;
        }
    };

    const int& operator[](const size_t i) const{
        return data_[i];
    };
    int& operator[](const size_t i){
        return data_[i];
    };
    explicit operator bool() const{
        if (size_==0){
            return false;
        }
        return true;
    };

    bool operator<(const Array& it) const{
        for (size_t i=0; i<std::min(size_, it.size_); ++i){
            if (data_[i]<it.data_[i]){
                return true;
            }
        }
            if (size_<it.size_){
                return true;
            }
            return false;
    };
    bool operator>(const Array& it) const{
        return it < *this;
    };
    bool operator!=(const Array& it) const{
        return ((it<*this) or (*this<it));
    };
    bool operator==(const Array& it) const{
        return !((it<*this) and (*this<it));
    };
    bool operator<=(const Array& it) const{
        return (*this<it or *this==it);
    };
    bool operator>=(const Array& it) const{
        return (*this>it or *this==it);
    };

    Array& operator<<(const int value){
        PushBack(value);
        return *this;
    };
    Array& operator<<(const Array& it){
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
    };

    friend std::ostream& operator<<(std::ostream& ostream, const Array& array){
        ostream<<"Result Array's capacity is "<<array.capacity_<<" and size is "<<array.size_;
        if (array.size_!=0){
           ostream<<", elements are: ";
            for (size_t i=0; i<array.size_-1;++i){
                ostream<<array.data_[i]<<", ";
            }
            ostream<<array.data_[array.size_-1];
        }
        return ostream;
    };

private:
    std::ostream& Ostream_;
    int *data_;
    size_t capacity_;
    size_t size_;

};

