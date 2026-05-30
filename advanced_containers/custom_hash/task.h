#include <unordered_map>
#include <vector>
#include <string>

struct SuperKey {
    std::string StrPart;
    int IntPart;
    float FloatPart;

    bool operator==(const SuperKey& rhs) const {
        return this->StrPart == rhs.StrPart && this->IntPart == rhs.IntPart && this->FloatPart == rhs.FloatPart;
    }
};

namespace std{
    template<>
    struct hash<SuperKey>{
        size_t operator()(const SuperKey& key) const{
            return hash<string>{}(key.StrPart) + hash<int>{}(key.IntPart) + hash<float>{}(key.FloatPart);
        }
    };
}
/*
 * Напишите хэш-функцию, реализовав специализацию шаблона std::hash для типа SuperKey
 * Напишите функцию PopulateHashMap, которая добавляет в hashMap пары ключ-значение из данного вектора toAdd
 */
void PopulateHashMap(
    std::unordered_map<SuperKey, std::string>& hashMap,
    const std::vector<std::pair<SuperKey, std::string>>& toAdd
);


#include "task.h"

using namespace std;

vector<int> HeapSort(const vector<int>& v) {
    priority_queue<int> mas;  
    
    for (int i : v) {
        mas.push(i);
    }
    vector<int> res(v.size());
    int i=v.size()-1;
    while (i>=0) {
        res[i] = mas.top();
        mas.pop();
        --i;
    }
    return res;
}


#include "task.h"

void ReverseList(std::list<int>& l) {
    std::list<int> mas;
    auto end = l.end();
    auto begin = l.begin();
    if (begin==end){
        return ;
    }
    while (begin!=end and begin!=--end){
        std::swap(*begin, *end);
        ++begin;
    }
    
}

#include <cstdlib>
#include <iterator>
#include <cstdint>

/*
 * Нужно написать функцию, которая принимает на вход диапазон, применяет к каждому элементу данную операцию и затем складывает результат применения операции в новый диапазон
 * Входной диапазон задан итераторами [firstIn; lastIn)
 * Выходной диапазон начинается с firstOut и имеет такую же длину как входной диапазон
 * Операция является функцией с одним аргументом (унарная функция), возвращающая результат такого типа, который можно положить в OutputIt
 */

template<class InputIt, class OutputIt, class UnaryOperation>
void Transform(InputIt firstIn, InputIt lastIn, OutputIt firstOut, UnaryOperation op) {
    for (auto x = firstIn; x!=lastIn; ++x){
        *firstOut = op(*x);
        ++firstOut;
        }
    //return firstOut;
}

/*
 * Нужно написать функцию, которая принимает на вход диапазон и переставляет элементы в нем таким образом, чтобы элементы,
 * которые удовлетворяют условию p, находились ближе к началу диапазона, чем остальные элементы.
 * Входной диапазон задан итераторами [first; last)
 * p является функцией с одним аргументом (унарная функция), возвращающая результат типа bool
 */

template<class BidirIt, class UnaryPredicate>
void Partition(BidirIt first, BidirIt last, UnaryPredicate p) {
    if (first==last){
        return;
    }
    auto l = first;
    auto r = last;
    --r;
    while (l!=r){
        if (p(*l)){
            ++l;
        }
        else{
            while (!p(*r) and l!=r){
                --r;
            }
            if (p(*r)){
                std::swap(*l, *r);
                ++l;
            }
            else{
                break;
            }            
        }
    }
}

/*
 * Нужно написать функцию, которая принимает на вход два отсортированных диапазона и объединяет их в новый отсортированный диапазон, содержащий все элементы обоих входных диапазонов.
 */
template<class InputIt1, class InputIt2, class OutputIt>
void Merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt firstOut) {
    auto l = first1;
    auto r = first2;
    while (l!= last1 and r!=last2){
        if (*l<*r){
            *firstOut = *l;
            ++l;
            ++first1;
        }
        else{
            *firstOut = *r;
            ++r;
            ++first2;
        }
        ++firstOut;
    }
    while (first1!=last1){
        *firstOut = *first1;
        ++first1;
        ++firstOut;
    }
    while (first2!=last2){
        *firstOut = *first2;
        ++first2;
        ++firstOut;
    }
    //return firstOut;
}


/*
 * Напишите класс "диапазон чисел Фибоначчи"
 * Экземпляр класса представляет из себя диапазон от первого до N-го числа Фибоначчи (1, 2, 3, 5, 8, 13 и т.д.)
 * С помощью функций begin и end можно получить итераторы и пробежать по диапазону или передать их в STL-алгоритмы
 */
class FibonacciRange {
private:
    size_t amount_;
public:

    class Iterator {
        friend class FibonacciRange;
    private:
        uint64_t now;
        uint64_t last;
        size_t kol;
        
    public:
        using value_type = uint64_t;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::input_iterator_tag;

        Iterator(size_t count): now(1), last(1), kol(count){}
        Iterator(): now(0), last(0), kol(0){}


        value_type operator *() const {
            return now;
            // разыменование итератора -- доступ к значению
        }

        Iterator& operator ++() {
            uint64_t dop = now+last;
            last = now;
            now = dop;
            --kol;
            return *this;
            // prefix increment
        }

        Iterator operator ++(int) {
            uint64_t dop = now+last;
            last = now;
            now = dop;
            --kol;
            return *this;
            // postfix increment
        }

        bool operator ==(const Iterator& rhs) const {
            //if (now==rhs.now and last==rhs.last and kol==rhs.kol)
            if (kol==rhs.kol){
                return true;
            }
            else{
                return false;
            }
        }

        bool operator <(const Iterator& rhs) const {
            if (now<rhs.now){
                return true;
            }
            if (now==rhs.now){
                if (last<rhs.last){
                    return true;
                }
                if (last==rhs.last){
                    if (kol<rhs.kol){
                        return true;
                    }
                }
            }
            return false;
        }
    };

    FibonacciRange(size_t amount): amount_(amount) {}

    Iterator begin() const {
        return Iterator(amount_);
    }

    Iterator end() const {
        return Iterator();
    }

    size_t size() const {
        return amount_;
    }
};


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


#include "tasks.h"

#include <cmath>

using namespace std;

int NOuter::NInner::DoSomething(int lhs, int rhs) {
    int res = lhs + rhs;
    return res;
}

int NOuter::DoSomething(int lhs, int rhs) {
    return lhs-rhs;
}

int NOverload::ProcessTwoArgs(int lhs, int rhs) {
    return lhs + rhs;
}

char NOverload::ProcessTwoArgs(char lhs, char rhs) {
    return max(lhs, rhs);
}

int NOverload::ProcessTwoArgs(int lhs, char rhs) {
    return lhs - rhs;
}

unsigned int NOverflow::WithOverflow(int lhs, int rhs) {
    return lhs  + rhs;
}

uint64_t NOverflow::WithOverflow(int64_t lhs, int64_t rhs) {
    return lhs-rhs;
}

int NLoop::SumInRange(const int lhs, const int rhs) {
    int s = 0;
    for (int i=lhs; i<rhs; ++i){
        s += i;
    }
    return s;
}

int NLoop::CountFixedBitsInRange(const int from, const int to, const int bitsCnt) {
    int k = 0;
    for (int i=from; i<=to; ++i){
        int a = i;
        int count = 0;
        while (a/2!=0) {
            count += a%2;
            a/= 2;
        }
        count+= a%2;
        if (count==bitsCnt){
            ++k;
        }
    }
    return k;
}
//`|(sin(a) / 2 + cos(a)) * (sin(a) / 2 + cos(a)) + tan(a) * atan(a)|`
double NMath::ComputeMathFormula(const double arg) {
    return abs((sin(arg)/2 +cos(arg))*(sin(arg)/2 + cos(arg)) + tan(arg) * atan(arg));
}

bool NMath::IsPositive(int arg) {
    return arg>0;
}

int NRecursion::CalculateFibonacci(const int arg) {
    long long a = 0, b = 1, c;
    if (arg==1){
        return 0;
    }
    else if(arg==1){
        return 1;
    }
    else{
        for(int i=2; i<=arg; ++i){
            c = a+b;
            a = b;
            b = c;
        }
    }
    return c;
}

#include "tasks.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

void NPointers::Increment(int *n){
    if (n!=nullptr){
        *n+=1;
    }
    return ;
}

int NPointers::Multiply(int a, int b, bool *flag){
    *flag = false;
    if ((a>0 and b>0) or (a<0 and b<0)){
        if (a*b<0 and flag!=nullptr){
            *flag = true;
        }
    }
    if ((a>0 and b<0) or (a<0 and b>0)){
        if (a*b>0 and flag!=nullptr){
            *flag = true;
        }
    }
    return a*b;
}

int NPointers::ScalarProduct(int const mas1[], int const mas2[], int n){
    int res = 0;
    for (int i=0; i<n; ++i){
        res+= mas1[i] * mas2[i];
    }
    return res;
}

int NPointers::SizeOfMaximumSquareOfCrosses(char const* mas, int n, int m){
    int mm = 0;
    vector<vector<int>> dp(n, vector<int> (m, 0));
    for (int i=0; i<n; ++i){
        for (int j=0; j<m; ++j){
            if (mas[i * m + j]=='+'){
                if (i==0 or j==0){
                    dp[i][j]=1;
                    mm = max(1, mm);
                }
                else{
                    dp[i][j] = min(dp[i-1][j], min(dp[i][j-1], dp[i-1][j-1])) + 1;
                    mm = max(mm, dp[i][j]);
                }
            }
        }
    }
    return mm;
}

long long *NPointers::MultiplyToLongLong(int a, int b){
    long long *c = new long long;
    *c =(long long)a*b;
    return c;
}

int NReferences::MultiplyInplace(int &n, int a){
    n = n*a;
    return n;
}

int NReferences::CompareArraysByAverage(const int mas[], int n, int const m[]){
    float s1 = 0, s2 = 0;
    float k2 = 5.0;
    for (int i=0; i<n; ++i){
        s1+= mas[i];
    }
    for (int i=0; i<5;++i){
        s2+= m[i];
    }
    if (s1/n<s2/k2){
        return -1;
    }
    else{
        if (s1/n == s2/k2){
            return 0;
        }
        else{
            return 1;
        }
    }
}

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


#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

template<typename T>
class FlattenedVector {
public:
    class Iterator{
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using value_type = T;
        using pointer = T*;
        Iterator(FlattenedVector* old, size_t index)
            :index_(index)
            ,old_vector_(old){}

        Iterator()
            :index_(0)
            ,old_vector_(nullptr){}

        bool operator==(const Iterator & other) const {
            return index_==other.index_ and old_vector_==other.old_vector_;
        }
        bool operator!=(const Iterator& other) const{
            return !(*this==other);
        }
        bool operator<(const Iterator& other) const{
            return index_<other.index_;
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
            ++index_;
            return *this;
        }
        Iterator operator ++(int){
            Iterator prevIndex = *this;
            ++index_;
            return prevIndex;
        }
        Iterator& operator --(){
            --index_;
            return *this;
        }
        Iterator operator --(int){
            Iterator prevIndex = *this;
            --index_;
            return prevIndex;
        }
        Iterator& operator+=(difference_type n){
            index_+= n;
            return *this;
        }
        Iterator& operator-=(difference_type n){
            index_-=n;
            return *this;
        }
        Iterator operator -(difference_type n) const{
            return Iterator(old_vector_, index_-n);
        }
        Iterator operator +(difference_type n) const{
            return Iterator(old_vector_, index_+n);
        }
        difference_type operator-(const Iterator& other) const{
            return index_ - other.index_;
        }
        reference operator *(){
            size_t l = 0;
            size_t r = old_vector_->prefix_kol_.size();
            
            while (l < r) {
                size_t m=(l+r)/2;
                if (old_vector_->prefix_kol_[m] <= index_) {
                    l = m+1;
                } else {
                    r = m;
                }
            }
            size_t prev_sum;
            if (l == 0) {
                prev_sum = 0;
            } else {
                prev_sum = old_vector_->prefix_kol_[l - 1];
            }
            return (*old_vector_->data_)[l][index_ - prev_sum];
        }
        reference operator*() const{
            size_t l = 0;
            size_t r = old_vector_->prefix_kol_.size();
            
            while (l < r) {
                size_t m=(l+r)/2;
                if (old_vector_->prefix_kol_[m] <= index_) {
                    l = m+1;
                } else {
                    r = m;
                }
            }
            size_t prev_sum;
            if (l == 0) {
                prev_sum = 0;
            } else {
                prev_sum = old_vector_->prefix_kol_[l - 1];
            }
            return (*old_vector_->data_)[l][index_ - prev_sum];
        }
        pointer operator ->(){
            return &operator*();
        }
        pointer operator ->() const{
            return &operator*();
        }
        T& operator [](difference_type n) {
            return *(*this +n);
        }
        reference operator[](difference_type n) const{
            return *(*this +n);
        }
        friend Iterator operator+(difference_type n, const Iterator& other) {
            return Iterator(other.old_vector_, other.index_ + n);
        }
    private:
        size_t index_;
        FlattenedVector* old_vector_;

    };
    FlattenedVector(std::vector<std::vector<T>> &data)
        :data_(&data){
        size_t kol = 0;

        for (size_t i = 0; i<data.size(); ++i){
            kol+= data[i].size();
            prefix_kol_.push_back(kol);
        }
        size_ = kol;
    }

    Iterator begin(){
        return Iterator(this, 0);
    }
    Iterator end(){
        return Iterator(this, size_);
    }
    size_t size(){
        return size_;
    }
    bool empty(){
        return size_==0;
    }

private:
    std::vector<std::vector<T>>* data_;
    size_t size_;
    std::vector<size_t> prefix_kol_;
};

#pragma once
#include <algorithm>
#include <iostream>
#include <cmath>

class Range {
public:
    class Iterator{
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using reference = int64_t;
        using value_type = int64_t;
        using pointer = void;

        Iterator(int64_t value, int64_t shag)
            :value_(value)
            ,shag_(shag){
            }

        reference operator *(){
            return value_;
        }
        reference operator*() const{
            return value_;
        }

        Iterator& operator++(){
            value_+= shag_;
            return *this;
        }
        Iterator operator++(int){
            Iterator prevValue = *this;
            value_+= shag_;
            return prevValue;
        }

         bool operator==(const Iterator & other) const {
            return value_== other.value_;
        }
        bool operator!=(const Iterator& other) const{
            return !(*this==other);
        }
        Iterator& operator--(){
            value_ -= shag_;
            return *this;
        }
        Iterator operator--(int){
            Iterator prevValue = *this;
            value_ -= shag_;
            return prevValue;
        }

    private:
        int64_t value_;
        int64_t shag_;

    };
    
    Range(int64_t n)
        :start_(0)
        ,end_(n)
        ,step_(1){
        }
    Range(int64_t nach, int64_t con)
        :start_(nach)
        ,end_(con)
        ,step_(1){
        }
    Range(int64_t nach, int64_t con, int64_t shag)
        :start_(nach)
        ,end_(con)
        ,step_(shag){}

    Iterator begin(){
        return Iterator(start_, step_);
    }
    Iterator begin() const{
        return Iterator(start_, step_);

    }
    Iterator end(){
        if (step_==0){
            return Iterator(start_, step_);
        }
        if ((step_>0 and start_>=end_) or (step_<0 and start_<=end_)){
            return Iterator(start_, step_);
        }
        int64_t kol = std::abs(end_ - start_) / std::abs(step_);
        if (std::abs(end_ - start_) % std::abs(step_) != 0) {
            ++kol;
        }
        return Iterator(start_ + kol*step_, step_);
    }
    Iterator end() const{
        if (step_==0){
            return Iterator(start_, step_);
        }
        if ((step_>0 and start_>=end_) or (step_<0 and start_<=end_)){
            return Iterator(start_, step_);
        }
        int64_t kol = std::abs(end_ - start_) / std::abs(step_);
        if (std::abs(end_ - start_) % std::abs(step_) != 0) {
            ++kol;
        }
        return Iterator(start_ + kol*step_, step_);
    }

    int64_t Size() const{
        if (step_==0){
            return 0;
        }
        if ((step_>0 and start_>=end_) or (step_<0 and start_<=end_)){
            return 0;
        }
        int64_t kol = std::abs(end_ - start_) / std::abs(step_);
        if (std::abs(end_ - start_) % std::abs(step_) != 0) {
            ++kol;
        }
        return kol;
    }
    


private:
        int64_t start_;
        int64_t end_;
        int64_t step_;
};

bool isalpha(unsigned char c) {
    const char* bukvi = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    for (const char* simvol = bukvi; *simvol != '\0'; ++simvol) {
        if (c==*simvol){
            return true;
        }
    }
    return false;
}

unsigned char tolower(unsigned char c) {
    const char* bukvi = "QWERTYUIOPLKJHGFDSAZXCVBNM";
    for (const char* s = bukvi; *s!='\0'; ++s){
        if (c == *s){
            c = c+('a'-'A');
            return c;
        }
    }
    return c;
}

#include "task.h"

char* concat(const char* lhs, const char* rhs) {
    int len1 = 0;
    while(*lhs != '\0'){
        ++lhs;
        ++len1;
    }
    int len2=0;
    while (*rhs!= '\0'){
        ++rhs;
        ++len2;
    }
    char* itog = new char[len1 + len2 +1];
    char* shs = itog;
    lhs-= len1;
    rhs-= len2;
    while (*lhs!= '\0'){
        *shs = *lhs;
        ++lhs;
        ++shs;
    }
    while(*rhs != '\0'){
        *shs = *rhs;
        ++rhs;
        ++shs;
    }
    *shs = '\0';
    return itog;
}

#include <string>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

string normalize(string_view path) {
    string s;
    if (path.size()==0){
        return ".";
    }
    if (path.size() == 1 || (path.find('.') == string_view::npos && path.find('/') == string_view::npos)){
        return string(path);
    }
    vector<string> slova;
    size_t l = 0;
    size_t r = path.size();
    while (l<r){
        if (path[l]=='/'){
            ++l;
            continue;
        }
        size_t new_right = l;
        while( new_right<r and path[new_right]!='/'){
            ++new_right;
        }
        string slovo = string(path.substr(l, new_right-l));
        //slova.push_back(slovo);
        if (!slovo.empty()) {  
            slova.push_back(slovo);
        }
        l = new_right;
    }
    vector<string> itog;
    int k=0;
    for(int i= slova.size()-1; i>=0; --i){
        if (slova[i]==".."){
            ++k;
        }
        else if(slova[i]=="."){
            continue;
        }
        else{
            if (k==0){
                itog.push_back(slova[i]);
            }
            else{
                --k;
            }
        }
    }
    reverse(itog.begin(), itog.end());
    if (path[0] != '/') {
        while (k > 0) {
            itog.insert(itog.begin(),"..");
            --k;
        }
    }
    //reverse(itog.begin(), itog.end());
    if (path[0]=='/'){
        s+= "/";
    }
    for (size_t i=0; i<itog.size(); ++i){
        if (i!=itog.size()-1){
            s+=itog[i];
            s+= "/";
        }
        else{
            s+= itog[i];
        }
    }
    if (s.size()==0){
        return ".";
    }
    return s;
}

#include "task.h"

bool isalpha(unsigned char c) {
    const char* bukvi = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    for (const char* simvol = bukvi; *simvol != '\0'; ++simvol) {
        if (c==*simvol){
            return true;
        }
    }
    return false;
}

unsigned char tolower(unsigned char c) {
    const char* bukvi = "QWERTYUIOPLKJHGFDSAZXCVBNM";
    for (const char* s = bukvi; *s!='\0'; ++s){
        if (c == *s){
            c = c+('a'-'A');
            return c;
        }
    }
    return c;
}

bool is_palindrome(const std::string& s) {
    size_t r = s.size()-1;
    size_t l = 0;
    bool pok = true;
    while (l<r and r>0 and l<s.size()){
        if (isalpha(s[r])){
            if (isalpha(s[l])){
                if (tolower(s[r])!=tolower(s[l])){
                    pok = false;
                    break;
                }
                else{
                    ++l;
                    --r;
                }
            }
            else{
                ++l;
            }
        }
        else{
            --r;
        }
    }
    return pok;
}

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

#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

class Timer {
private:
    steady_clock::time_point timer_time;
public:
    Timer(milliseconds time){
        auto now = steady_clock::now();
        timer_time = now + time;

    }
    bool Expired() const{
        auto now = steady_clock::now();
        if (now<timer_time){
            return false;
        }
        else{
            return true;
        }


    };
};

class TimeMeasurer {
private:
    steady_clock::time_point start;
    ostream* output;
public:
    TimeMeasurer(ostream& write = cout){
        output = &write;
        start = steady_clock::now();
    }
    ~TimeMeasurer(){
        auto end = steady_clock::now();
        auto otvet = end - start;
        milliseconds itog = duration_cast<milliseconds>(otvet);
        *output << "Elapsed time: " << itog.count()<<endl;
    }

    
};

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

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <functional>
#include <numeric>
#include <sstream>
#include <optional>
#include <memory>
#include <unordered_map>

using namespace std;

vector<shared_ptr<string>> DeDuplicate(const vector<unique_ptr<string>>& in){
    vector<shared_ptr<string>> out;
    unordered_map<string, shared_ptr<string>> slov;
    for (auto& x: in){
        if (slov.find(*x)==slov.end()){
            slov[*x] = make_shared<string>(*x);
        }
        out.push_back(slov[*x]);
    }
    return out;

}

vector<unique_ptr<string>> Duplicate(const vector<shared_ptr<string>>& in) {
    vector<unique_ptr<string>> out;  
    for (auto& shared_str : in) {
        out.push_back(make_unique<string>(*shared_str));
    }
    return out;
}

#include "logged_command_wrapper.h"
#include <iostream>
#include <list>
#include <vector>

/* Курсор влево */
class MoveCursorLeftCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        if (cursorPosition>0){
            --cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorLeftCommand(*this);
    }
};

/* Курсор вправо */
class MoveCursorRightCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        if (buffer.size()>cursorPosition){
            ++cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorRightCommand(*this);
    }
};

class MoveCursorUpCommand : public ICommand {
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        int k = 0;
        size_t nach_curs = cursorPosition;
        while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
            ++k;
            --nach_curs;
        }
        if (nach_curs>0){
            int len_of_new_string = 0;
            --nach_curs;
            while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
                ++len_of_new_string;
                --nach_curs;
            }
            if (len_of_new_string>=k){
                cursorPosition = nach_curs + k;
            }
            else{
                cursorPosition= nach_curs+ len_of_new_string;
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorUpCommand(*this);
    }
};
 
/* Курсор вниз */
class MoveCursorDownCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        int kol_bukv_do_nach = 0;
        if (cursorPosition<buffer.size() and buffer[cursorPosition]=='\n'){
            if (cursorPosition+1<buffer.size()){
                size_t new_cursor = cursorPosition;
                while (new_cursor>0 and buffer[new_cursor-1]!='\n'){
                    --new_cursor;
                }
                auto rast = cursorPosition-new_cursor;
                size_t next_cursor = cursorPosition +1;
                while (next_cursor< buffer.size() and buffer[next_cursor] != '\n'){
                    ++next_cursor;
                }
                ++cursorPosition ;
                cursorPosition += std::min( rast, next_cursor - cursorPosition);
            }
            return;
        }
        size_t nach_curs = cursorPosition;
        while (nach_curs>0 and !(buffer[nach_curs-1]=='\n')){
            ++kol_bukv_do_nach;
            --nach_curs;
        }
        while (cursorPosition<buffer.size() and !(buffer[cursorPosition]=='\n')){
            ++cursorPosition;
        }
        if (cursorPosition<buffer.size()){
            ++cursorPosition;
            while (kol_bukv_do_nach!=0 and !(buffer[cursorPosition]=='\n') and cursorPosition<buffer.size()){
                --kol_bukv_do_nach;
                ++cursorPosition;
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveCursorDownCommand(*this);
    }
};

/* Выделить текст */
class SelectTextCommand : public ICommand {
public:
    SelectTextCommand(size_t selectionSize): selectionSize_ (selectionSize){}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.SelectText(cursorPosition, std::min(cursorPosition + selectionSize_, buffer.size()));
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitSelectCommand(*this);
    }
private:
    size_t selectionSize_;
};

/* Ввести текст */
class InsertTextCommand : public ICommand {
public:
    InsertTextCommand(std::string text): new_text(text){}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
        }
        editor.UnselectText();
        buffer.insert(cursorPosition, new_text);
        cursorPosition += new_text.length();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitInsertTextCommand(*this);
    }
private:
    std::string new_text;
};

/* Удалить текст */
class DeleteTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
            editor.UnselectText();
        }
        else{
            if (cursorPosition<buffer.size()){
                buffer.erase(cursorPosition, 1);
            }
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitDeleteTextCommand(*this);
    }

};

/* Скопировать текст */
class CopyTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            clipboard = "";
            auto [start, end] = editor.GetSelection();
            for (; start<end; ++start){
                clipboard += buffer[start];
            }
        }
        else{
            clipboard = buffer[cursorPosition];
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitCopyTextCommand(*this);
    }
};

/* Вставить скопированный текст */
class PasteTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            buffer.erase(start, end-start);
            cursorPosition = start;
        }
        buffer.insert(cursorPosition, clipboard);
        cursorPosition += clipboard.length();
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitPasteTextCommand(*this);
    }
};

/* Привести выделенный текст в ВЕРХНИЙ регистр */
class UppercaseTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            for (; start<end; ++start){
                buffer[start] = std::toupper(buffer[start]);
            }
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitUppercaseTextCommand(*this);
    }
};

/* Привести выделенный текст в нижний регистр */
class LowercaseTextCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        if (editor.HasSelection()){
            auto [start, end]  = editor.GetSelection();
            for (; start<end; ++start){
                buffer[start] = std::tolower(buffer[start]);
            }
        }
        editor.UnselectText();
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitLowercaseTextCommand(*this);
    }
};

/* Перенести курсор в конец строки */
class MoveToEndCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition<buffer.size() and buffer[cursorPosition]!='\n'){
            ++cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveToEndCommand(*this);
    }
};

/* Перенести курсор в начало строки */
class MoveToStartCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition>0 and buffer[cursorPosition-1]!='\n'){
            --cursorPosition;
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitMoveToStartCommand(*this);
    } 
};

/* Удалить часть строки, начиная от позиции курсора до первого пробела или конца строки */
class DeleteWordCommand : public ICommand {
public:
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        editor.UnselectText();
        while (cursorPosition<buffer.size() and buffer[cursorPosition]!=' ' and buffer[cursorPosition]!= '\n'){
            buffer.erase(cursorPosition, 1);
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        visitor.VisitDeleteWordCommand(*this);
    }
};

/* Макрос */
class MacroCommand : public ICommand {
public:
    MacroCommand(std::list<CommandPtr> commands) : macrocommands_(commands.begin(), commands.end()) {}
    void Apply(std::string&buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
        for (auto& c: macrocommands_){
            c->Apply(buffer, cursorPosition, clipboard, editor);
        }
    }
    void AcceptVisitor(CommandVisitor& visitor)override{
        //visitor.VisitMacroCommand(*this);
    }
private:
    std::vector<CommandPtr> macrocommands_;
};

CommandLoggerVisitor::CommandLoggerVisitor(std::ostream& logStream): logStream_(logStream){}
void CommandLoggerVisitor::VisitMoveCursorLeftCommand(MoveCursorLeftCommand& command){
    logStream_<<"h";
}
void CommandLoggerVisitor::VisitMoveCursorRightCommand(MoveCursorRightCommand& command){
    logStream_<<"l";
}
void CommandLoggerVisitor::VisitMoveCursorUpCommand(MoveCursorUpCommand& command){
    logStream_<<"k";
}
void CommandLoggerVisitor::VisitMoveCursorDownCommand(MoveCursorDownCommand& command){
    logStream_<<"j";
}
void CommandLoggerVisitor::VisitSelectCommand(SelectTextCommand& command){
    logStream_<<"v";
}
void CommandLoggerVisitor::VisitInsertTextCommand(InsertTextCommand& command){
    logStream_<<"i";
}
void CommandLoggerVisitor::VisitDeleteTextCommand(DeleteTextCommand& command){
    logStream_<<"d";
}
void CommandLoggerVisitor::VisitCopyTextCommand(CopyTextCommand& command){
    logStream_<<"y";
}
void CommandLoggerVisitor::VisitPasteTextCommand(PasteTextCommand& command){
    logStream_<<"p";
}
void CommandLoggerVisitor::VisitUppercaseTextCommand(UppercaseTextCommand& command){
    logStream_<<"U";
}
void CommandLoggerVisitor::VisitLowercaseTextCommand(LowercaseTextCommand& command){
    logStream_<<"u";
}
void CommandLoggerVisitor::VisitMoveToEndCommand(MoveToEndCommand& command){
    logStream_<<"$";
}
void CommandLoggerVisitor::VisitMoveToStartCommand(MoveToStartCommand& command){
    logStream_<<"0";
}
void CommandLoggerVisitor::VisitDeleteWordCommand(DeleteWordCommand& command){
    logStream_<<"dE";
}


CommandBuilder::CommandBuilder(): 
    type_ (Type::MoveCursorLeft),
    selectionSize_(0),
    text_(""),
    logStreamPtr_(nullptr),
    subcommands_(){}

    CommandPtr CommandBuilder::build(){
        if (type_==Type::MoveCursorLeft){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorLeftCommand>());
            }
            return std::make_shared<MoveCursorLeftCommand>();
        }
        if (type_==Type::MoveCursorRight){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorRightCommand>());
            }
            return std::make_shared<MoveCursorRightCommand>();
        }
        if (type_==Type::MoveCursorUp){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorUpCommand>());
            }
            return std::make_shared<MoveCursorUpCommand>();
        }
        if (type_==Type::MoveCursorDown){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveCursorDownCommand>());
            }
            return std::make_shared<MoveCursorDownCommand>();
        }
        if (type_==Type::SelectText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<SelectTextCommand>(selectionSize_));
            }
            return std::make_shared<SelectTextCommand>(selectionSize_);
        }
        if (type_==Type::InsertText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<InsertTextCommand>(text_));
            }
            return std::make_shared<InsertTextCommand>(text_);
        }
        if (type_==Type::DeleteText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<DeleteTextCommand>());
            }
            return std::make_shared<DeleteTextCommand>();
        }
        if (type_==Type::CopyText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<CopyTextCommand>());
            }
            return std::make_shared<CopyTextCommand>();
        }
        if (type_==Type::PasteText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<PasteTextCommand>());
            }
            return std::make_shared<PasteTextCommand>();
        }
        if (type_==Type::UppercaseText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<UppercaseTextCommand>());
            }
            return std::make_shared<UppercaseTextCommand>();
        }
        if (type_==Type::LowercaseText){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<LowercaseTextCommand>());
            }
            return std::make_shared<LowercaseTextCommand>();
        }
        if (type_==Type::MoveToEnd){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveToEndCommand>());
            }
            return std::make_shared<MoveToEndCommand>();
        }
        if (type_==Type::MoveToStart){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<MoveToStartCommand>());
            }
            return std::make_shared<MoveToStartCommand>();
        }
        if (type_==Type::DeleteWord){
            if (logStreamPtr_){
                return std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, std::make_shared<DeleteWordCommand>());
            }
            return std::make_shared<DeleteWordCommand>();
        }
        if (type_==Type::Macro){
            if (logStreamPtr_){
                std::list<CommandPtr> comands;
                for (auto& comand: subcommands_){
                    comands.push_back(std::make_shared<LoggedCommandWrapper>(*logStreamPtr_, comand));
                }
                return std::make_shared<MacroCommand>(comands);
            }
            return std::make_shared<MacroCommand>(subcommands_);
        }
        return {};
    }
    CommandBuilder& CommandBuilder::WithType(Type type){
        type_ = type;
        return *this;
    };
    CommandBuilder& CommandBuilder::SelectionSize(size_t selectionSize){
        selectionSize_ = selectionSize;
        return *this;
    };
    CommandBuilder& CommandBuilder::Text(std::string text){
        text_ = text;
        return *this;
    };
    CommandBuilder& CommandBuilder::LogTo(std::ostream& logStream){
        logStreamPtr_ = &logStream;
        return *this;
    };
    CommandBuilder& CommandBuilder::AddSubcommand(CommandPtr subcommand){
        subcommands_.push_back(subcommand);
        return *this;
    };


#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <exception>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <functional>
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;

class IObject {
    /*
     * Базовый класс для всех объектов, с которыми работает калькулятор
     */
public:
    virtual ~IObject() = default;

    /*
     * Преобразование объекта в строку. Удобно, когда нужно вывести объект в описание исключения или в логи.
     * В проверяющем коде задания ожидается, что объекты, представляющие числовые литералы, будут приводиться к строке
     * так, как это делает функция std::to_string (не стоит изобретать свой велосипед в этом месте)
     */
    virtual string Stringify() const = 0;
};

using ObjPtr = shared_ptr<const IObject>;
using Context = unordered_map<std::string, ObjPtr>;

class IExpression : public IObject {
    /*
     * Базовый класс для вычислимых выражений, таких как
     * Числовые константы, функции (в т.ч. лямбда-функции), имена, операции применения функции к аргументу
     */
public:
    /*
     * Вычисляет значение выражения. Результатом вычисления может быть как константа, так и какой-то более сложный объект
     * Например, функция после применения её в какому-то аргументу может вернуть другую функцию.
     *
     * Подразумевается, что функции в данной задаче каррированные, т.е. могут быть применены только к одному аргументу
     * за один раз. В таком случае функция с несколькими аргументами превращается в функцию от одного аргумента, которая возвращает
     * другую функцию, имеющую на один аргумент меньше. См. https://ru.wikipedia.org/wiki/Каррирование
     */
    virtual ObjPtr Evaluate(const Context& ctx) const = 0;
};

using ExprPtr = shared_ptr<const IExpression>;

class Exception : public std::exception {
private:
    string error;
public:
    Exception(const string& words): error(words) {}
    const char* what() const noexcept override {
        return error.c_str();
    }
};
//А зачем определённые названия то...

class ParsingException : public Exception {
public:
    ParsingException(const string& words) : Exception("Cannot parse string: " + words) {}
};

class ParenthesisMismatchException : public ParsingException {
public:
    ParenthesisMismatchException(const string& words = "Parenthesis mismatch") : ParsingException(words) {}
};

class EmptyParsingResultException : public ParsingException {
public:
    EmptyParsingResultException(const string& words = "Empty parsing result") : ParsingException(words) {}
};

class BadLambdaArgumentException : public ParsingException {
public:
    BadLambdaArgumentException(const string& words) : ParsingException("Expected argument names for the lambda, got " + words) {}
};

class LambdaWithoutArgumentsException : public ParsingException {
public:
    LambdaWithoutArgumentsException(const string& words = "A lambda requires at least one argument") : ParsingException(words) {}
};

class EvaluationException : public Exception {
public:
    EvaluationException(const string& words) : Exception(words) {}
};

class UnresolvedReferenceException : public EvaluationException {
public:
    UnresolvedReferenceException(const string& words) : EvaluationException("Unknown name " + words) {}
};

class ApplyException : public EvaluationException {
public:
    ApplyException(const string& words) : EvaluationException(words) {}
};


class Work_with_n : public IExpression {
private:
    double n;
public:
    Work_with_n(double nn) : n(nn) {}
    string Stringify() const override {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.6f", n);
        string itog = buffer;
        return itog;
    }
    ObjPtr Evaluate(const Context& ctx)const override {
        return make_shared<Work_with_n>(n);
    }
    double GetValue()const{
        return n;
    }
};

class Name : public IExpression {
private:
    string name;
public:
    Name(const string& ima): name(ima) {}
    string Stringify()const override{
        return name;
    }
    ObjPtr Evaluate(const Context& ctx)const override{
        auto it = ctx.find(name);
        if (it==ctx.end()) {
            throw UnresolvedReferenceException(name);
        }
        return it->second;
    }
};

class BuiltInFunction : public IExpression {
private:
    string name_;
    function<double(double, double)> binary_op_;
    function<double(double)> unary_op_;
    bool is_binary_;
    
public:
    BuiltInFunction(const string& name, function<double(double, double)> op)
        : name_(name), binary_op_(op), is_binary_(true) {}
    
    BuiltInFunction(const string& name, function<double(double)> op)
        : name_(name), unary_op_(op), is_binary_(false) {}
    
    string Stringify() const override {
        return name_;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        return make_shared<BuiltInFunction>(*this);
    }
    
    bool IsBinary() const { return is_binary_; }
    
    double ApplyBinary(double a, double b) const {
        if (!is_binary_) throw ApplyException(name_ + " is not a binary operator");
        return binary_op_(a, b);
    }
    
    double ApplyUnary(double a) const {
        if (is_binary_) throw ApplyException(name_ + " is not a unary operator");
        return unary_op_(a);
    }
};

class LambdaFunction : public IExpression {
private:
    vector<string> arg_names;
    shared_ptr<const IExpression> body;
    Context capture_ctx;
    
public:
    LambdaFunction(const vector<string>& args, shared_ptr<const IExpression> b, const Context& ctx) : arg_names(args), body(b) {
        for (const auto& pair : ctx) capture_ctx[pair.first] = pair.second;
    }
    string Stringify() const override {
        string result = "λ";
        for (size_t i = 0; i < arg_names.size(); ++i) {
            result += arg_names[i];
            if (i != arg_names.size() - 1) {
                result += ".λ";
            }
        }
        
        string body_str = body->Stringify();
        result += "." + body_str;
        return result;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        return make_shared<LambdaFunction>(arg_names, body, capture_ctx);
    }
    
    ObjPtr Apply(ObjPtr arg) const {
        if (arg_names.empty()){
            throw ApplyException("земля в элюминаторе");
        }
        if (arg_names.size() == 1) {
            Context new_ctx = capture_ctx;
            new_ctx[arg_names[0]] = arg;
            return body->Evaluate(new_ctx);
        }
        else {
            vector<string> remaining_args(arg_names.begin() + 1, arg_names.end());
            Context new_ctx = capture_ctx;
            new_ctx[arg_names[0]] = arg;
            return make_shared<LambdaFunction>(remaining_args, body, new_ctx);
        }
    }
};

class Operations : public IExpression {
private:
    shared_ptr<const IExpression> x;
    shared_ptr<const IExpression> y;  
    string op;
public:
    Operations(shared_ptr<const IExpression> a, shared_ptr<const IExpression> b, const string& doo): 
        x(a),
        y(b),
        op(doo) {}
    
    string Stringify() const override {
        string left_str = x->Stringify();
        string right_str = y->Stringify();
        if (left_str[0] != '(' || left_str.back() != ')') {
            left_str = "(" + left_str + ")";
        }
        if (right_str[0] != '(' || right_str.back() != ')') {
            right_str = "(" + right_str + ")";
        }
        return "(" + op + left_str + right_str + ")";
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        ObjPtr xx = x->Evaluate(ctx);
        ObjPtr yy = y->Evaluate(ctx);
        auto xn = dynamic_pointer_cast<const Work_with_n>(xx);
        auto yn = dynamic_pointer_cast<const Work_with_n>(yy);
        if (xn==nullptr or yn==nullptr) {
            throw ApplyException("Поломочка из-за того что переменная хранит фигню");
        }
        double eins = xn->GetValue();
        double zwei = yn->GetValue();
        double itog;
        if (op=="+") {
            itog = eins + zwei;
        } 
        else if (op == "-") {
            itog = eins - zwei;
        }
        else if (op == "*") {
            itog = eins * zwei;
        }
        else if (op == "/") {
            if (zwei==0){
                throw ApplyException("пытаемся делить на 0");
            }
            else{
                itog = eins / zwei;
            }
        }
        else{
            throw ApplyException("Чёрт знает как мы тут оказались");
        }
        return make_shared<Work_with_n>(itog);
    }
};

class Functions : public IExpression {
private:
    shared_ptr<const IExpression> function;
    shared_ptr<const IExpression> argument;
public:
    Functions(shared_ptr<const IExpression> f, shared_ptr<const IExpression> x) : 
        function(f), argument(x) {}
    
    string Stringify() const override {
        string func_str = function->Stringify();
        string arg_str = argument->Stringify();
        
        auto builtin = dynamic_pointer_cast<const BuiltInFunction>(function);
        if (builtin && !builtin->IsBinary()) {
            return "(" + func_str + " " + arg_str + ")";
        }
        return func_str + " " + arg_str;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        auto func_app = dynamic_pointer_cast<const Functions>(function);
        if (func_app) {
            ObjPtr inner_result = func_app->Evaluate(ctx);
            auto inner_lambda = dynamic_pointer_cast<const LambdaFunction>(inner_result);
            if (!inner_lambda) {
                ObjPtr arg_obj = argument->Evaluate(ctx);
                throw ApplyException("Cannot apply function '" + func_app->Stringify() + "' to argument " + arg_obj->Stringify());
            }
            ObjPtr arg_obj = argument->Evaluate(ctx);
            return inner_lambda->Apply(arg_obj);
        }
        
        ObjPtr func_obj = function->Evaluate(ctx);
        ObjPtr arg_obj = argument->Evaluate(ctx);
        
        auto lambda = dynamic_pointer_cast<const LambdaFunction>(func_obj);
        if (lambda) return lambda->Apply(arg_obj);
        
        auto builtin = dynamic_pointer_cast<const BuiltInFunction>(func_obj);
        if (builtin && !builtin->IsBinary()) {
            auto num = dynamic_pointer_cast<const Work_with_n>(arg_obj);
            if (!num){
                throw ApplyException("sin/cos");
            }
            return make_shared<Work_with_n>(builtin->ApplyUnary(num->GetValue()));
        }
        
        throw ApplyException("Cannot apply function '" + func_obj->Stringify() + "' to argument " + arg_obj->Stringify());
    }
};

class IStatement : public IObject {
    /*
     * Базовый класс для инструкций, подаваемых на вход калькулятору
     * Одна входная строка в итоге должна быть преобразована в одну инструкцию
     * Инструкции бывают двух типов -- вычисление значения выражения и присваивание
     * Инструкция присваивания выглядит как `[name] = [expression]`, где
     *  `[name]` -- какое-то имя из букв и цифр, начинающееся с буквы;
     *  `[expression]` -- выражение, значение которого нужно запомнить под именем `[name]`.
     * Если в исходной строке не было знака равенства (который означает операцию присваивания),
     * то вся строка воспринимается как инструкция вычисления выражения
     */
public:
    virtual ObjPtr Execute(Context& ctx) const = 0;
};
using StatementPtr = shared_ptr<const IStatement>;

class Expression : public IStatement {
private:
    shared_ptr<const IExpression> write;
public:
    Expression(shared_ptr<const IExpression> words) : write(words) {}
    string Stringify() const override {
        return write->Stringify();
    }
    ObjPtr Execute(Context& ctx) const override {
        return write->Evaluate(ctx);
    }
};

class Prisvaivanie: public IStatement {
private:
    string name;
    shared_ptr<const IExpression> write;
public:
    Prisvaivanie(const string& ima, shared_ptr<const IExpression> words) : 
        name(ima), write(words) {}
    string Stringify() const override {
        return name + " = " + write->Stringify();
    }
    ObjPtr Execute(Context& ctx) const override {
        ObjPtr itog = write->Evaluate(ctx);
        ctx[name] = itog;
        return itog;
    }
};

struct Token {
    /*
     * Токен -- минимальная неделимая единица разбора исходной строки
     */
    enum class Type {
        Literal = 0,  // литерал -- буквальное значение (числовая константа в данной задаче типа `1`, `25.48` и т.п.)
        Name,  // Какое-то имя из букв и цифр, начинающееся с буквы, либо знак встроенной бинарной операции (+-*/). Имя ссылается на какое-то значение (константа, функция)
        LeftParenthesis,  // Открывающая круглая скобка `(`
        RightParenthesis,  // Закрывающая круглая скобка скобка `)`
        LambdaSign,  // Условный символ "лямбда" `\` (в данном случае это левый слэш как в языке Haskell)
        Arrow,  // Стрелка `->`
        EqualsSign,  // Знак равенства `=`
    };

    Type type_;
    string value_;
};

class StatementParser {
public:
    StatementPtr ParseString(const string& s, const Context& ctx) {
        vector<Token> mas = Tokenize(s, ctx);
        if (mas.empty()) {
            throw EmptyParsingResultException();
        }

        for (size_t i = 0; i < mas.size(); ++i) {
            if (mas[i].type_ == Token::Type::EqualsSign) {
                if (i == 0 || mas[i-1].type_ != Token::Type::Name) {
                    throw ParsingException("Cannot assign in expression");
                }
                string imya = mas[i-1].value_;
                size_t pos = i + 1;
                shared_ptr<const IExpression> expr = ParseExpression(mas, pos, ctx);
                if (pos != mas.size()) {
                    throw ParsingException("-----");
                }
                return make_shared<Prisvaivanie>(imya, expr);
            }
        }
        
        size_t pos = 0;
        shared_ptr<const IExpression> expr = ParseExpression(mas, pos, ctx);
        if (pos != mas.size()) {
            if (pos < mas.size() && mas[pos].type_ == Token::Type::RightParenthesis) {
                throw ParenthesisMismatchException();
            }
            throw ParsingException("-----");
        }
        return make_shared<Expression>(expr);
    }

    /*
     * Разобрать исходную строку на токены, которые можно в дальнейшем преобразовать в инструкцию для калькулятора
     * Эта функция будет полезна в `ParseString`
     */
    std::vector<Token> Tokenize(const string& s, const Context& ctx);

    shared_ptr<const IExpression> ParseExpression(const vector<Token>& mas, size_t& pos, const Context& ctx);
    
private:
    shared_ptr<const IExpression> ParseAddSub(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseMulDiv(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParsePrimary(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseApplications(shared_ptr<const IExpression> func, const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseLambda(const vector<Token>& tokens, size_t& pos, const Context& ctx);
};
shared_ptr<const IExpression> StatementParser::ParseExpression(const vector<Token>& mas, size_t& pos, const Context& ctx) {
    return ParseAddSub(mas, pos, ctx);
}

shared_ptr<const IExpression> StatementParser::ParseAddSub(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    shared_ptr<const IExpression> left = ParseMulDiv(tokens, pos, ctx);
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        string op = tokens[pos].value_;
        if (op == "+" or op == "-") {
            ++pos;
            shared_ptr<const IExpression> right = ParseMulDiv(tokens, pos, ctx);
            left = make_shared<Operations>(left, right, op);
        }
        else{
            break;
        }
    }
    return left;
}

shared_ptr<const IExpression> StatementParser::ParseMulDiv(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    shared_ptr<const IExpression> left = ParsePrimary(tokens, pos, ctx);
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        string op = tokens[pos].value_;
        if (op == "*" or op == "/") {
            ++pos;
            shared_ptr<const IExpression> right = ParsePrimary(tokens, pos, ctx);
            left = make_shared<Operations>(left, right, op);
        }else{
            break;
        }
    }
    return left;
}

shared_ptr<const IExpression> StatementParser::ParsePrimary(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    if (pos >= tokens.size()){
        throw EmptyParsingResultException();
    }
    
    if (tokens[pos].type_ == Token::Type::LambdaSign) {
        return ParseLambda(tokens, pos, ctx);
    }
    
    if (tokens[pos].type_ == Token::Type::LeftParenthesis) {
        ++pos;
        if (pos < tokens.size() && tokens[pos].type_ == Token::Type::RightParenthesis) {
            throw EmptyParsingResultException();
        }
        shared_ptr<const IExpression> expr = ParseAddSub(tokens, pos, ctx);
        if (pos >= tokens.size() || tokens[pos].type_ != Token::Type::RightParenthesis) {
            throw ParenthesisMismatchException();
        }
        ++pos;
        return ParseApplications(expr, tokens, pos, ctx);
    }
    
    shared_ptr<const IExpression> expr;
    if (tokens[pos].type_ == Token::Type::Literal) {
        expr = make_shared<Work_with_n>(stod(tokens[pos].value_));
    }
    else if (tokens[pos].type_ == Token::Type::Name) {
        expr = make_shared<Name>(tokens[pos].value_);
    }
    else {
        throw ParsingException("Unexpected token: " + tokens[pos].value_);
    }
    ++pos;
    
    return ParseApplications(expr, tokens, pos, ctx);
}

shared_ptr<const IExpression> StatementParser::ParseApplications(shared_ptr<const IExpression> func, const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    while (pos < tokens.size()) {
        if (tokens[pos].type_ == Token::Type::LambdaSign or
            tokens[pos].type_ == Token::Type::LeftParenthesis or
            tokens[pos].type_ == Token::Type::Literal or
            tokens[pos].type_ == Token::Type::Name) {
            
            if (tokens[pos].type_ == Token::Type::Name) {
                string name = tokens[pos].value_;
                if (name=="+" or name=="-" or name=="*" or name=="/"){
                    break;
                }
            }
            
            shared_ptr<const IExpression> arg;
            if (tokens[pos].type_ == Token::Type::LambdaSign) {
                arg = ParseLambda(tokens, pos, ctx);
            }
            else if (tokens[pos].type_ == Token::Type::LeftParenthesis) {
                ++pos;
                arg = ParseAddSub(tokens, pos, ctx);
                if (pos >= tokens.size() or tokens[pos].type_ != Token::Type::RightParenthesis) {
                    throw ParenthesisMismatchException();
                }
                ++pos;
            }
            else if (tokens[pos].type_ == Token::Type::Literal) {
                arg = make_shared<Work_with_n>(stod(tokens[pos].value_));
                ++pos;
            }
            else if (tokens[pos].type_ == Token::Type::Name) {
                arg = make_shared<Name>(tokens[pos].value_);
                ++pos;
            }
            
            func = make_shared<Functions>(func, arg);
        }
        else {
            break;
        }
    }
    return func;
}

shared_ptr<const IExpression> StatementParser::ParseLambda(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    if (tokens[pos].type_ != Token::Type::LambdaSign) {
        throw ParsingException("Expected lambda");
    }
    ++pos;
    
    vector<string> args;
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        args.push_back(tokens[pos].value_);
        ++pos;
    }
    
    if (args.empty()) {
        throw LambdaWithoutArgumentsException();
    }
    
    if (pos >= tokens.size() || tokens[pos].type_ != Token::Type::Arrow) {
        if (pos < tokens.size()) {
            throw BadLambdaArgumentException(tokens[pos].value_);
        } else {
            throw BadLambdaArgumentException("ничего");
        }
    }
    ++pos;
    
    shared_ptr<const IExpression> body = ParseAddSub(tokens, pos, ctx);
    return make_shared<LambdaFunction>(args, body, ctx);
}

vector<Token> StatementParser::Tokenize(const string& s, const Context& ctx) {
    vector<Token> mas;
    size_t i = 0;
    string chisla = "0123456789.";
    string alfavit = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_";
    
    while (i < s.size()) {
        if (s[i] == ' ') {
            ++i;
            continue;
        }
        
        // Числа
        if (isdigit(s[i]) || (s[i] == '.' && i + 1 < s.size() && isdigit(s[i+1]))) {
            string n;
            while (i < s.size() && (isdigit(s[i]) || s[i] == '.')) {
                n += s[i];
                ++i;
            }
            mas.push_back({Token::Type::Literal, n});
            continue;
        }
        
        // Имена
        if (isalpha(s[i]) || s[i] == '_') {
            string imya;
            while (i < s.size() && (isalnum(s[i]) || s[i] == '_')) {
                imya += s[i];
                ++i;
            }
            mas.push_back({Token::Type::Name, imya});
            continue;
        }
        
        // Специальные символы
        if (s[i] == '(') {
            mas.push_back({Token::Type::LeftParenthesis, "("});
        }
        else if (s[i] == ')') {
            mas.push_back({Token::Type::RightParenthesis, ")"});
        }
        else if (s[i] == '\\') {
            mas.push_back({Token::Type::LambdaSign, "\\"});
        }
        else if (s[i] == '=') {
            mas.push_back({Token::Type::EqualsSign, "="});
        }
        else if (s[i] == '-') {
            if (i + 1 < s.size() and s[i + 1] == '>') {
                mas.push_back({Token::Type::Arrow, "->"});
                ++i;
            }
            else {
                mas.push_back({Token::Type::Name, "-"});
            }
        }
        else if (s[i] == '+') {
            mas.push_back({Token::Type::Name, "+"});
        }
        else if (s[i] == '*') {
            mas.push_back({Token::Type::Name, "*"});
        }
        else if (s[i] == '/') {
            mas.push_back({Token::Type::Name, "/"});
        }
        else {
            throw ParsingException(string(1, s[i]));
        }
        ++i;
    }
    
    return mas;
}

class Calculator {
public:
    Calculator() {
        // Добавляем базовые предопределенные функции в калькулятор.
        // Реализовать классы нужно самостоятельно, тригонометрические функции брать из стандартной библиотеки
        ctx_["+"] = make_shared<BuiltInFunction>("+", [](double a, double b) { return a + b; });
        ctx_["-"] = make_shared<BuiltInFunction>("-", [](double a, double b) { return a - b; });
        ctx_["*"] = make_shared<BuiltInFunction>("*", [](double a, double b) { return a * b; });
        ctx_["/"] = make_shared<BuiltInFunction>("/", [](double a, double b) { return a / b; });
        ctx_["sin"] = make_shared<BuiltInFunction>("sin", [](double a) { return sin(a); });
        ctx_["cos"] = make_shared<BuiltInFunction>("cos", [](double a) { return cos(a); });
    }

    string ProcessString(const string& stringExpression) {
        StatementPtr statement = ParseLine(stringExpression);
        return ExecuteStatement(statement)->Stringify();
    }

    ObjPtr ExecuteStatement(StatementPtr statement) {
        /*
         * Выполнить данную инструкцию.
         * В результате выполнения инструкции присваивания будут происходить изменения в контексте
         * В результате выполнения инструкции вычисления выражения контекст не должен меняться
         */
        return statement->Execute(ctx_);
    }

    ObjPtr EvaluateExpression(shared_ptr<const IExpression> expression) const {
        return expression->Evaluate(ctx_);
    }

    StatementPtr ParseLine(const string& line) {
        return parser_.ParseString(line, ctx_);
    }

    shared_ptr<const IExpression> ParseExpressionLine(const string line) {
        /*
         * Разобрать строку в выражение.
         * Если строка задает инструкцию присваивания, а не инструкцию вычисления выражения, то бросить исключение
         */
        vector<Token> mas = parser_.Tokenize(line, ctx_);
        if (mas.empty()) {
            throw EmptyParsingResultException();
        }
        for (const Token& el : mas) {
            if (el.type_ == Token::Type::EqualsSign) {
                throw ParsingException("Want expression but it is assignment statement");
            }
        }
        size_t pos = 0;
        return parser_.ParseExpression(mas, pos, ctx_);
    }
    
private:
    Context ctx_;
    StatementParser parser_;
};


#pragma once
#include <typeinfo> 
#include <memory> 
#include <utility>


using namespace std;

struct inner_base {
    virtual ~inner_base() = default;
    virtual inner_base* clone() const = 0;  
    virtual const type_info& type() const = 0;  
};
template<typename T>
struct inner : inner_base {
    inner(const T& value_) : value(value_) {}
    inner(T&& value_) : value(std::move(value_)) {}
    
    virtual inner_base* clone() const override {
        inner<T>* new_obect = new inner<T>(value);
        return new_obect;
    }
    
    virtual const type_info& type() const override {
        return typeid(T);
    }
    T value;
};

class Any {
private:
    unique_ptr<inner_base> ykas_na_val;
public:
    Any() = default;
    Any(Any&& other) : ykas_na_val(std::move(other.ykas_na_val)) {}
    template<typename T>
    Any(T value) : ykas_na_val(make_unique<inner<T>>(std::move(value))) {}
    Any(const Any& other) {
        if (other.ykas_na_val) {
            ykas_na_val.reset(other.ykas_na_val->clone());
        }
    }
    ~Any() = default;
    bool Empty() const{
        return ykas_na_val == nullptr;
    }
    void Reset(){
        ykas_na_val.reset();
    }
    void Swap(Any& other){
        ykas_na_val.swap(other.ykas_na_val);
    }
    const type_info& Type() const {
        if (ykas_na_val != nullptr) {
            return ykas_na_val->type();
        }
        else {
            return typeid(void);
        }
    }

    template<typename T>
    T& Value() {
        try {
            if (ykas_na_val == nullptr) {
                throw bad_cast();
            }
            if (ykas_na_val->type() != typeid(T)) {
                throw bad_cast();
            }
            return static_cast<inner<T>*>(ykas_na_val.get())->value;
        }
        catch (const bad_cast& e) {
            throw;
        }
        catch (...) {
            throw bad_cast();
        }
    }
    
    template<typename T>
    const T& Value() const {
        try {
            if (ykas_na_val == nullptr) {
                throw bad_cast();
            }
            
            if (ykas_na_val->type() != typeid(T)) {
                throw bad_cast();
            }
            
            return static_cast<const inner<T>*>(ykas_na_val.get())->value;
        }
        catch (const bad_cast& e) {
            throw; 
        }
        catch (...) {
            throw bad_cast();
        }
    }
    
    Any& operator=(const Any& other) {
        if (this != &other) {
            Any copia(other);
            Swap(copia);
        }
        return *this;
    }
    
    template<typename T>
    Any& operator=(T value) {
        Any copia(std::move(value));
        Swap(copia);
        return *this;
    }
    
    Any& operator=(Any&& other) {
        if (this != &other) {
            ykas_na_val = std::move(other.ykas_na_val);
        }
        return *this;
    }
};

