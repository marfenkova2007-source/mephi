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
