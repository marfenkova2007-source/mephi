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

