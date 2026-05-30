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
