#include <iostream>
#include <algorithm>

// vector of ints
class Vector {
public:
    Vector(); // default constructor конструктор по-умолчанию
    Vector(size_t size, int defaultValue = 0);
    Vector(const Vector& other); // конструктор копирования
    ~Vector();
    void PushBack(int v);
    void PushFront(int v);
    bool Insert(size_t pos, int v);

    size_t Size() const {
        return size_;
    }

    int Back() const {
        return data_[size_ - 1];
    }

    int PopBack();
    void Erase(size_t pos);
    int At(int pos) const;
    int operator [](int pos) const;
    int& At(int pos);
    int& operator[](int pos);
    void Reserve(size_t newCapacity);
    void Resize(size_t size);

    Vector& operator =(const Vector& other);
private:
    size_t size_, capacity_;
    int* data_;

    void ReserveInternal(size_t newCapacity);
};

Vector::Vector() : size_(0), capacity_(2), data_(new int[capacity_]) {
    std::cout << "Vector initialized with default capacity " << capacity_ << std::endl;
}

Vector::Vector(size_t size, int defaultValue)
: size_(size)
, capacity_(size < 1 ? 2 : size * 2)
, data_(new int[capacity_]) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = defaultValue;
    }
    std::cout << "Vector initialized with default value " << defaultValue << " and size " << size_ << std::endl;
}

Vector::Vector(const Vector& other)
: size_(other.size_)
, capacity_(other.capacity_)
, data_(new int[capacity_]) {
    // memcpy(data_, other.data_, sizeof(*data_) * size_);
    std::copy_n(other.data_, size_, data_);
    std::cout << "Vector initialized as copy of other vector\n";
}

Vector::~Vector() {
    delete[] data_;
}

void Vector::PushBack(int v) {
    Insert(size_, v);
}

void Vector::PushFront(int v) {
    Insert(0, v);
}

bool Vector::Insert(size_t pos, int v) {
    if (pos > size_) {
        return false;
    }
    if (size_ == capacity_) {
        Reserve(capacity_ * 2);
    }
    for (size_t i = size_; i > pos; --i) {
        data_[i] = data_[i-1];
    }
    data_[pos] = v;
    ++size_;
    return true;
}

void Vector::Reserve(size_t newCapacity) {
    if (newCapacity <= capacity_) {
        return;
    }
    ReserveInternal(newCapacity);
}

void Vector::Erase(size_t pos) {
    if (pos >= size_) {
        return;
    }
    for (size_t i = pos; i + 1 < size_; ++i) {
        data_[i] = data_[i + 1];
    }
    --size_;
}

int Vector::PopBack() {
    int result = Back();
    Erase(size_ - 1);
    return result;
}

int Vector::At(int pos) const {
    return const_cast<Vector*>(this)->At(pos);
}

int Vector::operator [](int pos) const {
    return const_cast<Vector*>(this)->operator[](pos);
}

int& Vector::At(int pos) {
    // like in Python
    if (pos >= size_ || pos < 0 && static_cast<size_t>(std::abs(pos)) > size_) {
        std::cout << "At(" << pos << ") is out of range\n";
        exit(1);
    }
    return (*this)[pos];
    // return this->operator[](pos);
}

int& Vector::operator [](int pos) {
    return data_[(pos + size_) % size_];
}

void Vector::Resize(size_t size) {
    if (size > capacity_) {
        ReserveInternal(size);
    }
    if (size < size_) {
        ReserveInternal(size);
    }
    size_ = size;
}

void Vector::ReserveInternal(size_t newCapacity) {
    capacity_ = newCapacity;
    auto* newP = new int[capacity_];
    std::copy_n(data_, size_, newP);
    delete[] data_;
    data_ = newP;
}

Vector& Vector::operator =(const Vector& other) {
    Vector v = other;  // то же самое, что Vector v(other)
    std::swap(v, *this);
    return *this;
}

void PrintVector(const Vector& v) {
    for (size_t i = 0; i < v.Size(); ++i) {
        std::cout << v[i] << std::endl;
    }
}


#include <cstddef>

class CowString {
private:
    struct StringData {
        int ref_count = 1;
        size_t size = 0;
        size_t capacity = 0;
        char* data;
        
        StringData(size_t cap) : capacity(cap) {
            data = new char[capacity];
            data[0] = '\0';
        }
        
        StringData(const char* str) {
            size = 0;
            while (str[size] != '\0') ++size;
            capacity = size + 1;
            if (capacity < 2) capacity = 2;
            data = new char[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = str[i];
            }
            data[size] = '\0';
        }
        
        ~StringData() {
            delete[] data;
        }
    };
    
    StringData* data_;
    
    void EnsureUnique() {
        if (data_->ref_count > 1) {
            data_->ref_count--;
            data_ = new StringData(data_->data);
        }
    }

public:
    CowString() : data_(new StringData(2)) {}
    
    CowString(const char* str) : data_(new StringData(str)) {}
    
    CowString(const CowString& other) : data_(other.data_) {
        data_->ref_count++;
    }
    
    CowString& operator=(const CowString& other) {
        if (this != &other) {
            if (--data_->ref_count == 0) {
                delete data_;
            }
            data_ = other.data_;
            data_->ref_count++;
        }
        return *this;
    }
    
    ~CowString() {
        if (--data_->ref_count == 0) {
            delete data_;
        }
    }

    const char& At(size_t index) const {
        return data_->data[index];
    }
    
    char& operator[](size_t index) {
        EnsureUnique();
        return data_->data[index];
    }

    const char& Back() const {
        return data_->data[data_->size - 1];
    }

    void PushBack(char c) {
        // Делаем копию ТОЛЬКО если есть другие владельцы
        if (data_->ref_count > 1) {
            data_->ref_count--;
            data_ = new StringData(data_->data);
        }
        
        // Проверяем capacity
        if (data_->size + 2 > data_->capacity) {
            size_t new_capacity = data_->capacity * 2;
            char* new_data = new char[new_capacity];
            
            for (size_t i = 0; i < data_->size; ++i) {
                new_data[i] = data_->data[i];
            }
            
            delete[] data_->data;
            data_->data = new_data;
            data_->capacity = new_capacity;
        }
        
        data_->data[data_->size] = c;
        data_->size++;
        data_->data[data_->size] = '\0';
    }
    
    size_t Size() const {
        return data_->size;
    }
    
    size_t Capacity() const {
        return data_->capacity;
    }

    void Reserve(size_t capacity_) {
        std::cout << "Reserve called - current capacity: " << data_->capacity 
                << ", requested: " << capacity_ 
                << ", ref_count: " << data_->ref_count << std::endl;
        
        if (capacity_ > data_->capacity) {
            // Делаем копию ТОЛЬКО если есть другие владельцы
            if (data_->ref_count > 1) {
                std::cout << "Making copy in Reserve because ref_count > 1" << std::endl;
                data_->ref_count--;
                data_ = new StringData(data_->data);
            }
            
            char* new_data = new char[capacity_];
            for (size_t i = 0; i < data_->size; ++i) {
                new_data[i] = data_->data[i];
            }
            new_data[data_->size] = '\0';
            
            std::cout << "Reallocating in Reserve - old data: " << (void*)data_->data 
                    << ", new data: " << (void*)new_data << std::endl;
            
            delete[] data_->data;
            data_->data = new_data;
            data_->capacity = capacity_;
            
            std::cout << "After Reserve - data address: " << (void*)data_->data << std::endl;
        }
    }
    
    void Resize(size_t size_) {
        EnsureUnique();
        
        if (size_ > data_->size) {
            if (size_ + 1 > data_->capacity) {
                Reserve(size_ + 1);
            }
            for (size_t i = data_->size; i < size_; ++i) {
                data_->data[i] = '\0';
            }
        }
        
        data_->size = size_;
        data_->data[data_->size] = '\0';
    }
};