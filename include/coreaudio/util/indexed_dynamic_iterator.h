#ifndef WINDOWS_COREAUDIO_WRAPPER_INDEXED_DYNAMIC_ITERATOR_H_
#define WINDOWS_COREAUDIO_WRAPPER_INDEXED_DYNAMIC_ITERATOR_H_

#include <cstddef>
#include <iterator>
#include <functional>


namespace coreaudio {
namespace detail {

struct other_type {
    template<typename T>
    friend bool operator<(other_type, const T&) noexcept { return true; }
    template<typename T>
    friend bool operator<(const T&, other_type) noexcept { return false; }
};

/**
 * An iterator over f(i) for i in [0, n), for n natural-number indices i.
 * f(i) should be implemented as the result of `operator*`, returning `f(this->index)` and `operator[](n)`
 * returning `f(this->index + n)`.
 *
 * These functions need to be implemented to follow the iterator protocol:
 * value_type operator*();
 * value_type operator[](difference_type);
 * friend bool operator==(const Self&, const Self&);
 * friend bool operator<(const Self&, const Self&);  // (Only if IsOrdered)
 *
 * @tparam Self CRTP parameter for the derived class
 * @tparam T value_type (type of return value of `operator*`)
 * @tparam IndexType Type of `index`
 * @tparam DifferenceType `difference_type` used in iterator protocol. Ideally `std::make_signed_t<IndexType>`.
 * @tparam IsOrdered If false, do not need to define `operator<`. Will also not define `operator>`, `operator<=` and `operator>=`.
 */
template<typename Self, typename T, typename IndexType = std::size_t, typename DifferenceType = typename std::make_signed<IndexType>::type, bool IsOrdered = true>
struct indexed_dynamic_iterator {

    using index_type = IndexType;
    using difference_type = DifferenceType;
    using value_type = T;
    using pointer = T*;
    using reference = T;
    using iterator_category = std::input_iterator_tag;  // Must be InputIterator since reference is not a reference type
    using iterator_concept = typename std::conditional<IsOrdered, std::random_access_iterator_tag, iterator_category>::type;

private:
    constexpr Self& self() noexcept { return static_cast<Self&>(*this); }
    constexpr const Self& self() const noexcept { return static_cast<const Self&>(*this); }

    // To disable >, >= and <= if not IsOrdered
    using o = typename std::conditional<IsOrdered, const Self&, other_type>::type;
public:
    friend bool operator!=(const Self& left, const Self& right) noexcept(noexcept(static_cast<bool>(left == right))) { return !static_cast<bool>(left == right); }
    friend bool operator> (o left, const Self& right) noexcept(noexcept(static_cast<bool>(right < left))) { return  static_cast<bool>(right < left); }
    friend bool operator>=(o left, const Self& right) noexcept(noexcept(static_cast<bool>(right < left))) { return !static_cast<bool>(right < left); }
    friend bool operator<=(o left, const Self& right) noexcept(noexcept(static_cast<bool>(left < right))) { return !static_cast<bool>(left < right); }

    constexpr Self& operator++() noexcept { ++index; return self(); }
    constexpr Self operator++(int) noexcept { Self copy = self(); ++*this; return copy; }
    constexpr Self& operator--() noexcept { --index; return self(); }
    constexpr Self operator--(int) noexcept { Self copy = self(); --*this; return copy; }

    friend constexpr Self operator+(const Self& it, difference_type n) noexcept {
        Self copy = it;
        copy += n;
        return copy;
    }

    friend constexpr Self operator+(Self&& it, difference_type n) noexcept {
        it += n;
        return static_cast<Self&&>(it);
    }

    friend constexpr Self operator+(difference_type n, const Self& it) noexcept {
        Self copy = it;
        copy += n;
        return copy;
    }

    friend constexpr Self operator+(difference_type n, Self&& it) noexcept {
        it += n;
        return static_cast<Self&&>(it);
    }

    friend constexpr Self operator-(const Self& it, difference_type n) noexcept {
        Self copy = it;
        copy -= n;
        return copy;
    }

    friend constexpr Self operator-(Self&& it, difference_type n) noexcept {
        it -= n;
        return static_cast<Self&&>(it);
    }

    friend constexpr Self operator-(difference_type n, const Self& it) noexcept {
        Self copy = it;
        copy -= n;
        return copy;
    }

    friend constexpr Self operator-(difference_type n, Self&& it) noexcept {
        it -= n;
        return static_cast<Self&&>(it);
    }

    friend constexpr difference_type operator-(const Self& left, const Self& right) noexcept {
        return static_cast<difference_type>(left.index) - static_cast<difference_type>(right.index);
    }

    constexpr Self& operator+=(difference_type n) noexcept {
        index += n;
        return self();
    }

    constexpr Self& operator-=(difference_type n) noexcept {
        index -= n;
        return self();
    }
protected:
    constexpr indexed_dynamic_iterator() noexcept = default;
    constexpr indexed_dynamic_iterator(const indexed_dynamic_iterator&) noexcept = default;
    constexpr indexed_dynamic_iterator(indexed_dynamic_iterator&&) noexcept = default;

    constexpr indexed_dynamic_iterator& operator=(const indexed_dynamic_iterator&) noexcept = default;
    constexpr indexed_dynamic_iterator& operator=(indexed_dynamic_iterator&&) noexcept = default;

    IndexType index = 0;
    explicit constexpr indexed_dynamic_iterator(IndexType index) noexcept : index(index) {}
};

/**
 * As indexed_dynamic_iterator, but also have an additional `Context* context` member, which is used to
 * create an `operator==` and `operator<`. Also has a default constructor which just sets the context to nullptr.
 */
template<typename Self, typename Context, typename T, typename IndexType = std::size_t, typename DifferenceType = typename std::make_signed<IndexType>::type>
struct indexed_dynamic_iterator_with_context : indexed_dynamic_iterator<Self, T, IndexType, DifferenceType, true> {
    friend bool operator==(const Self& left, const Self& right) noexcept {
        return left.index == right.index && left.context == right.context;
    }
    friend bool operator<(const Self& left, const Self& right) noexcept {
        return left.context == right.context ? left.index < right.index : std::less<Context*>{}(left.context, right.context);
    }
protected:
    using super = indexed_dynamic_iterator<Self, T, IndexType, DifferenceType, true>;

    Context* context = nullptr;

    constexpr indexed_dynamic_iterator_with_context() noexcept = default;
    constexpr indexed_dynamic_iterator_with_context(const indexed_dynamic_iterator_with_context&) noexcept = default;
    explicit constexpr indexed_dynamic_iterator_with_context(Context* context, IndexType index = 0) noexcept : super(index), context(context) {}
    explicit constexpr indexed_dynamic_iterator_with_context(IndexType index, Context* context = nullptr) noexcept : super(index), context(context) {}

    constexpr indexed_dynamic_iterator_with_context& operator=(const indexed_dynamic_iterator_with_context&) noexcept = default;
};

}
}

#endif  // WINDOWS_COREAUDIO_WRAPPER_INDEXED_DYNAMIC_ITERATOR_H_
