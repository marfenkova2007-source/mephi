#include <cstddef>

//воруем код с семинара и адаптируем на чары
class Vector {
public:
    Vector(); 
    Vector(size_t size, char defaultValue = '\0');
    Vector(const char* str); 
    Vector(const Vector& other);
    ~Vector();
    void PushBack(char v);
    void PushFront(char v);
    bool Insert(size_t pos, char v);

    size_t Size() const {
        return size_;
    }

    char Back() const {
        return data_[size_ - 1];
    }

    char PopBack();
    void Erase(size_t pos);
    char At(size_t pos) const;
    char operator [](size_t pos) const;
    char& At(size_t pos);
    char& operator[](size_t pos);
    void Reserve(size_t newCapacity);
    void Resize(size_t size);

    Vector& operator =(const Vector& other);
    
    const char* Data() const { return data_; }
    size_t Capacity() const { return capacity_; }
    
private:
    size_t size_ = 0, capacity_ = 2;
    char* data_ = nullptr;

    void ReserveInternal(size_t newCapacity);
};

Vector::Vector() : data_(new char[capacity_]) {
    data_[0] = '\0';
}

Vector::Vector(size_t size, char defaultValue)
: size_(size)
, capacity_(size < 1 ? 2 : size + 1)
, data_(new char[capacity_]) {
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = defaultValue;
    }
    data_[size_] = '\0';
}

Vector::Vector(const char* str) {
    size_ = 0;
    while (str[size_] != '\0') ++size_;
    capacity_ = size_ + 1;
    if (capacity_ < 2) capacity_ = 2;
    data_ = new char[capacity_];
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = str[i];
    }
    data_[size_] = '\0';
}

Vector::Vector(const Vector& other)
: size_(other.size_)
, capacity_(other.capacity_)
, data_(new char[capacity_]) {
    for (size_t i = 0; i <= size_; ++i) { 
        data_[i] = other.data_[i];
    }
}

Vector::~Vector() {
    delete[] data_;
}

void Vector::PushBack(char v) {
    Insert(size_, v);
}

void Vector::PushFront(char v) {
    Insert(0, v);
}

bool Vector::Insert(size_t pos, char v) {
    if (pos > size_) {
        return false;
    }
    if (size_ + 1 >= capacity_) {
        Reserve(capacity_ * 2);
    }
    for (size_t i = size_; i > pos; --i) {
        data_[i] = data_[i-1];
    }
    data_[pos] = v;
    ++size_;
    data_[size_] = '\0';
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
    data_[size_] = '\0';
}

char Vector::PopBack() {
    if (size_ == 0) return '\0';
    char result = Back();
    Erase(size_ - 1);
    return result;
}

char Vector::At(size_t pos) const {
    return data_[pos];
}

char Vector::operator [](size_t pos) const {
    return data_[pos];
}

char& Vector::At(size_t pos) {
    return data_[pos];
}

char& Vector::operator[](size_t pos) {
    return data_[pos];
}

void Vector::Resize(size_t size) {
    if (size > capacity_) {
        ReserveInternal(size + 1);
    }
    if (size < size_) {
        size_ = size;
        data_[size_] = '\0';
    } else {
        for (size_t i = size_; i < size; ++i) {
            data_[i] = '\0';
        }
        size_ = size;
        data_[size_] = '\0';
    }
}

void Vector::ReserveInternal(size_t newCapacity) {
    capacity_ = newCapacity;
    char* newP = new char[capacity_];
    for (size_t i = 0; i <= size_; ++i) {
        newP[i] = data_[i];
    }
    delete[] data_;
    data_ = newP;
}

Vector& Vector::operator =(const Vector& other) {
    if (this != &other) {
        Vector temp(other);
        size_ = temp.size_;
        capacity_ = temp.capacity_;
        delete[] data_;
        data_ = temp.data_;
        temp.data_ = nullptr;
    }
    return *this;
}

class CowString {
private:
    struct State {
        int ref_count = 1;
        Vector data;
        
        State() : data() {}
        State(const char* str) : data(str) {}
    };
    State* data_;
    
    void Make() {
        if (data_->ref_count > 1) {
            data_->ref_count--;
            data_ = new State(data_->data.Data());
        }
    }

public:
    CowString() : data_(new State()) {}
    CowString(const char* str) : data_(new State(str)) {}
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
        return data_->data.At(index);
    }
    char& operator[](size_t index) {
        Make();
        return data_->data[index];
    }
    const char& Back() const {
        return data_->data[data_->data.Size() - 1];
    }
    void PushBack(char c) {
        Make();
        data_->data.PushBack(c);
    }
    size_t Size() const {
        return data_->data.Size();
    }
    size_t Capacity() const {
        return data_->data.Capacity();
    }
    void Reserve(size_t capacity_) {
        Make();
        data_->data.Reserve(capacity_ + 1);
    }
    void Resize(size_t size_) {
        Make();
        data_->data.Resize(size_);
    }
};