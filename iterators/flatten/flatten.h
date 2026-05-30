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
