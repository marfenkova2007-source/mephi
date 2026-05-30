#pragma once

#include <cstddef>
#include <exception>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <cstdint>

namespace ranges {

// Определяет тип данных, на которые ссылается итератор
template <typename Iterator>
using IteratorValueType = typename std::iterator_traits<Iterator>::value_type;

// Определяет тип возвращаемого функцией значения F(Args...) -> Result
template <typename F, typename... Args>
using FunctionResultType = std::invoke_result_t<F, Args...>;

namespace detali {

template <class T>
using RemoveCvRef = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
using IterOf = decltype(std::begin(std::declval<T&>()));

template <class T>
using SentOf = decltype(std::end(std::declval<T&>()));

template <class Container>
constexpr bool kHasPushBack = requires(Container& c, typename Container::value_type v) {
    c.push_back(std::move(v));
};

template <class Container, class T>
constexpr bool kHasInsertHint = requires(Container& c, T v) {
    c.insert(c.end(), std::move(v));
};

template <class Container, class T>
constexpr bool kHasInsert = requires(Container& c, T v) {
    c.insert(std::move(v));
};

template <class Container, class T>
void Append(Container& c, T&& v) {
    if constexpr (kHasPushBack<Container>) {
        c.push_back(std::forward<T>(v));
    }
    else if constexpr (kHasInsertHint<Container, T>) {
        c.insert(c.end(), std::forward<T>(v));
    }
    else if constexpr (kHasInsert<Container, T>) {
        c.insert(std::forward<T>(v));
    }
    else {
        static_assert(sizeof(Container) == 0, "Мимо, мимо вопросик");
    }
}

}


template <class L, class A>
auto operator|(L&& lhs, A&& adaptor)
    -> decltype(std::forward<A>(adaptor)(std::forward<L>(lhs)))
{
    return std::forward<A>(adaptor)(std::forward<L>(lhs));
}

template <class T>
class Empty {
private:
    class Iterator {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const T*;
        using reference = const T&;

    public:
        reference operator*() const {
            static const T* kDummy = nullptr;
            return *kDummy;
        }

        Iterator& operator++() {
            return *this;
        }

        Iterator operator++(int) {
            return *this;
        }

        friend bool operator==(const Iterator&, const Iterator&) {
            return true;
        }
        friend bool operator!=(const Iterator&, const Iterator&) {
            return false;
        }
    };

public:
    Iterator begin() const {
        return Iterator{};
    }

    Iterator end() const {
        return Iterator{};
    }
};

template <class Container>
class View {
public:
    using Iterator = detali::IterOf<Container>;
    using Sentinel = detali::SentOf<Container>;

public:
    explicit View(Container& c)
        : Container_(&c)
    { }

    Iterator begin() const {
        return std::begin(*Container_);
    }

    Sentinel end() const {
        return std::end(*Container_);
    }

private:
    Container* Container_ = nullptr;
};

template <class Container>
View(Container&) -> View<Container>;

namespace detali {

template <class PrevView>
//Чертова нерабочая фигня
class FirstView {
private:
    using BaseIt = decltype(std::declval<const PrevView&>().begin());
    using BaseEnd = decltype(std::declval<const PrevView&>().end());

    struct Sentinel {
        BaseEnd End;
    };

    class Iterator {
    public:
        using value_type = IteratorValueType<BaseIt>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using reference = decltype(*std::declval<BaseIt&>());

    public:
        Iterator(BaseIt it, BaseEnd end, std::int64_t left)
            : It_(std::move(it))
            , End_(std::move(end))
            , Left_(left)
        { }

        reference operator*() const {
            return *It_;
        }

        Iterator& operator++() {
            if (AtEnd()) {
                return *this;
            }

            if (Left_ <= 1) {
                Left_ = 0;
                return *this;
            }

            ++It_;
            --Left_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& it, const Sentinel& s) {
            return it.AtEnd(s);
        }
        friend bool operator!=(const Iterator& it, const Sentinel& s) {
            return !it.AtEnd(s);
        }
        friend bool operator==(const Sentinel& s, const Iterator& it) {
            return it == s;
        }
        friend bool operator!=(const Sentinel& s, const Iterator& it) {
            return it != s;
        }

    private:
        bool AtEnd() const {
            return Left_ <= 0 || It_ == End_;
        }

        bool AtEnd(const Sentinel& s) const {
            return Left_ <= 0 || It_ == s.End;
        }

    private:
        BaseIt It_;
        BaseEnd End_;
        std::int64_t Left_ = 0;
    };

public:
    FirstView(PrevView prev, std::int64_t n)
        : Prev_(std::move(prev))
        , N_(n)
    { }

    Iterator begin() const {
        const auto end = Prev_.end();
        const auto n = N_ < 0 ? 0 : N_;
        return Iterator(Prev_.begin(), end, n);
    }

    Sentinel end() const {
        return Sentinel{Prev_.end()};
    }

private:
    PrevView Prev_;
    std::int64_t N_ = 0;
};

}  

struct FirstAdaptor {
    std::int64_t N = 0;

    template <class PrevView>
    auto operator()(PrevView view) const {
        return detali::FirstView<detali::RemoveCvRef<PrevView>>(std::move(view), N);
    }
};

inline FirstAdaptor First(std::int64_t n) {
    return FirstAdaptor{n};
}


namespace detali {

template <class PrevView, class Predicate>
class FilterView {
private:
    using BaseIt = decltype(std::declval<const PrevView&>().begin());
    using BaseEnd = decltype(std::declval<const PrevView&>().end());

    struct Sentinel {
        BaseEnd End;
    };

    class Iterator {
    public:
        using value_type = IteratorValueType<BaseIt>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using reference = decltype(*std::declval<BaseIt&>());

    public:
        Iterator(BaseIt it, BaseEnd end, const Predicate* pred)
            : It_(std::move(it))
            , End_(std::move(end))
            , Pred_(pred)
        {
            Satisfy();
        }

        reference operator*() const {
            return *It_;
        }

        Iterator& operator++() {
            ++It_;
            Satisfy();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& it, const Sentinel& s) {
            return it.It_==s.End;
        }
        friend bool operator!=(const Iterator& it, const Sentinel& s) {
            return !(it==s);
        }
        friend bool operator==(const Sentinel& s, const Iterator& it) {
            return it==s;
        }
        friend bool operator!=(const Sentinel& s, const Iterator& it) {
            return it!=s;
        }

    private:
        void Satisfy() {
            while (It_ != End_ and !(*Pred_)(*It_)) {
                ++It_;
            }
        }

    private:
        BaseIt It_;
        BaseEnd End_;
        const Predicate* Pred_ = nullptr;
    };

public:
    FilterView(PrevView prev, Predicate pred)
        : Prev_(std::move(prev))
        , Pred_(std::move(pred))
    { }

    Iterator begin() const {
        return Iterator(Prev_.begin(), Prev_.end(), &Pred_);
    }

    Sentinel end() const {
        return Sentinel{Prev_.end()};
    }

private:
    PrevView Prev_;
    Predicate Pred_;
};

}  

template <class Predicate>
struct FilterAdaptor {
    Predicate Pred;

    template <class PrevView>
    auto operator()(PrevView view) const {
        return detali::FilterView<detali::RemoveCvRef<PrevView>, Predicate>(std::move(view), Pred);
    }
};

template <class Predicate>
auto Filter(Predicate pred) {
    return FilterAdaptor<Predicate>{std::move(pred)};
}

namespace detali {

template <class PrevView, class Mapper>
// и тут тоже
class MapView {
private:
    using BaseIt = decltype(std::declval<const PrevView&>().begin());
    using BaseEnd = decltype(std::declval<const PrevView&>().end());
    using BaseRef = decltype(*std::declval<BaseIt&>());

    struct Sentinel {
        BaseEnd End;
    };

public:
    using Mapped = FunctionResultType<const Mapper&, BaseRef>;

private:
    class Iterator {
    public:
        using value_type = detali::RemoveCvRef<Mapped>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

    public:
        Iterator(BaseIt it, BaseEnd end, const Mapper* mapper)
            : It_(std::move(it))
            , End_(std::move(end))
            , Mapper_(mapper)
        { }

        auto operator*() const {
            return (*Mapper_)(*It_);
        }

        Iterator& operator++() {
            ++It_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& it, const Sentinel& s) {
            return it.It_==s.End;
        }
        friend bool operator!=(const Iterator& it, const Sentinel& s) {
            return !(it==s);
        }
        friend bool operator==(const Sentinel& s, const Iterator& it) {
            return it==s;
        }
        friend bool operator!=(const Sentinel& s, const Iterator& it) {
            return it!=s;
        }

    private:
        BaseIt It_;
        BaseEnd End_;
        const Mapper* Mapper_ = nullptr;
    };

public:
    MapView(PrevView prev, Mapper mapper)
        : Prev_(std::move(prev))
        , Mapper_(std::move(mapper))
    { }

    Iterator begin() const {
        return Iterator(Prev_.begin(), Prev_.end(), &Mapper_);
    }

    Sentinel end() const {
        return Sentinel{Prev_.end()};
    }

private:
    PrevView Prev_;
    Mapper Mapper_;
};

}  

template <class Mapper>
struct MapAdaptor {
    Mapper MapperFn;

    template <class PrevView>
    auto operator()(PrevView view) const {
        return detali::MapView<detali::RemoveCvRef<PrevView>, Mapper>(std::move(view), MapperFn);
    }
};

template <class Mapper>
auto Map(Mapper mapper) {
    return MapAdaptor<Mapper>{std::move(mapper)};
}

namespace detali {

template <class Mapper>
struct SafeWrapper {
    Mapper M;

    template <class T>
    auto operator()(T&& x) const {
        using Res = RemoveCvRef<decltype(M(std::forward<T>(x)))>;
        try {
            return std::optional<Res>(M(std::forward<T>(x)));
        }catch (...) {
            return std::optional<Res>();
        }
    }
};

} 

template <class Mapper>
auto SafeMap(Mapper mapper) {
    return Map(detali::SafeWrapper<Mapper>{std::move(mapper)});
}

struct UnwrapAdaptor {
    template <class PrevView>
    auto operator()(PrevView view) const {
        auto has = [](const auto& opt) {
            return static_cast<bool>(opt);
        };
        auto val = [](const auto& opt) -> decltype(*opt) { 
            return *opt;
        };
        return std::move(view) | Filter(has) | Map(val);
    }
};

inline UnwrapAdaptor Unwrap() {
    return UnwrapAdaptor{};
}


template <class Container>
struct ToAdaptor {
    template <class PrevView>
    Container operator()(PrevView view) const {
        Container out;
        for (auto it = view.begin(); it != view.end(); ++it) {
            detali::Append(out, *it);
        }
        return out;
    }
};

template <class Container>
ToAdaptor<Container> To() {
    return ToAdaptor<Container>{};
}

}  