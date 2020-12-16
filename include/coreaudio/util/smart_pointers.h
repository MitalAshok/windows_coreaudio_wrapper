#ifndef WINDOWS_COREAUDIO_WRAPPER_SMART_POINTERS_H_
#define WINDOWS_COREAUDIO_WRAPPER_SMART_POINTERS_H_

#include "unknwn.h"
#include "comdef.h"

#include <type_traits>
#include <functional>

namespace coreaudio {

/**
 * Like `std::unique_ptr<T, deleter>` where `deleter::operator()(T* p)` calls `p->Release()`.
 *
 * Does not support casting between different `com_pointer` types. Have to explicity release, cast, and create another
 * if you need this.
 *
 * Also has `get_for_overwrite()` which returns the held pointer. Writing `q` to that pointer is equivalent to
 * `this->release(); this->reset(q);`.
 *
 * `operator*` and `operator->` throw a `_com_error(E_INVALIDARG)` if there is no held pointer (`!*this`). Thus,
 * they are not defined when COREAUDIO_NOEXCEPTIONS.
 *
 * @tparam T The COM interface type
 */
template<typename T = IUnknown>
struct com_pointer {
    static_assert(!std::is_const<T>::value && !std::is_volatile<T>::value, "com_pointer: T must not be const or volatile");
    static_assert(std::is_convertible<T*, IUnknown*>::value, "com_pointer: T must publically and unambiguously inherit from IUnknown");

    constexpr com_pointer() noexcept = default;
    constexpr com_pointer(std::nullptr_t) noexcept : com_pointer() {}
    constexpr explicit com_pointer(T* p) noexcept : p(p) {}
    constexpr com_pointer(com_pointer&& other) noexcept : p(other.p) { other.p = nullptr; }

    ~com_pointer() noexcept {
        if (p) release(p);
    }

    constexpr com_pointer& operator=(com_pointer&& other) noexcept {
        if (this != &other) {
            reset(other.p);
            other.p = nullptr;
        }
        return *this;
    }

    constexpr com_pointer& operator=(std::nullptr_t) noexcept { reset(); }

    constexpr T* release() noexcept {
        T* released = p;
        p = nullptr;
        return released;
    }

    constexpr void reset(T* with = nullptr) noexcept {
        if (p) release(p);
        p = with;
    }

    constexpr void swap(com_pointer& other) noexcept {
        T* tmp = p;
        p = other.p;
        other.p = tmp;
    }

    constexpr void swap(com_pointer&& other) noexcept { swap(other); }

    constexpr friend void swap(com_pointer&  l, com_pointer&  r) noexcept { l.swap(r); }
    constexpr friend void swap(com_pointer&& l, com_pointer&  r) noexcept { l.swap(r); }
    constexpr friend void swap(com_pointer&  l, com_pointer&& r) noexcept { l.swap(r); }
    constexpr friend void swap(com_pointer&& l, com_pointer&& r) noexcept { l.swap(r); }

    constexpr T* get() const noexcept {
        return p;
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    constexpr T& operator*() const {
        if (!p) throw _com_error(E_INVALIDARG);
        return *p;
    }

    constexpr T* operator->() const {
        if (!p) throw _com_error(E_INVALIDARG);
        return p;
    }
#endif

    constexpr explicit operator bool() const noexcept {
        return p;
    }

    constexpr friend bool operator==(const com_pointer& l, const com_pointer& r) noexcept { return l.p == r.p; }
    constexpr friend bool operator!=(const com_pointer& l, const com_pointer& r) noexcept { return l.p != r.p; }
    constexpr friend bool operator< (const com_pointer& l, const com_pointer& r) noexcept { return std::less<T*>{}(l.p, r.p); }
    constexpr friend bool operator>=(const com_pointer& l, const com_pointer& r) noexcept { return !(l < r); }
    constexpr friend bool operator> (const com_pointer& l, const com_pointer& r) noexcept { return r < l; }
    constexpr friend bool operator<=(const com_pointer& l, const com_pointer& r) noexcept { return !(r < l); }

    constexpr T*& get_for_overwrite() noexcept {
        return p;
    }
private:
    constexpr static void release(T* p) noexcept {
        (p->*(&IUnknown::Release))();
    }

    T* p = nullptr;
};

/**
 * Like `std::unique_ptr<T, deleter>` where `deleter::operator()(T* p)` calls `CoTaskMemFree(p)`.
 *
 * @tparam T The type of the allocated memory
 */
template<typename T>
struct task_memory_pointer {
    constexpr task_memory_pointer() noexcept = default;
    constexpr task_memory_pointer(std::nullptr_t) noexcept : task_memory_pointer() {}
    constexpr explicit task_memory_pointer(T* p) noexcept : p(p) {}
    constexpr task_memory_pointer(task_memory_pointer&& other) noexcept : p(other.p) { other.p = nullptr; }

    ~task_memory_pointer() { CoTaskMemFree(p); }

    constexpr task_memory_pointer& operator=(task_memory_pointer&& other) noexcept {
        if (this != &other) {
            reset(other.p);
            other.p = nullptr;
        }
        return *this;
    }
    constexpr task_memory_pointer& operator=(std::nullptr_t) noexcept { reset(); }

    constexpr T* release() noexcept {
        T* released = p;
        p = nullptr;
        return released;
    }

    void reset(T* with = nullptr) noexcept {
        CoTaskMemFree(p);
        p = with;
    }

    constexpr void swap(task_memory_pointer& other) noexcept {
        T* tmp = p;
        p = other.p;
        other.p = tmp;
    }

    constexpr void swap(task_memory_pointer&& other) noexcept { swap(other); }

    constexpr friend void swap(task_memory_pointer&  l, task_memory_pointer&  r) noexcept { l.swap(r); }
    constexpr friend void swap(task_memory_pointer&& l, task_memory_pointer&  r) noexcept { l.swap(r); }
    constexpr friend void swap(task_memory_pointer&  l, task_memory_pointer&& r) noexcept { l.swap(r); }
    constexpr friend void swap(task_memory_pointer&& l, task_memory_pointer&& r) noexcept { l.swap(r); }

    constexpr T* get() const noexcept {
        return p;
    }

    constexpr explicit operator bool() const noexcept {
        return p;
    }

    constexpr T& operator*() const noexcept {
        return *p;
    }
    constexpr T* operator->() const noexcept {
        return p;
    }

    constexpr friend bool operator==(const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return l.p == r.p; }
    constexpr friend bool operator!=(const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return l.p != r.p; }
    constexpr friend bool operator< (const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return std::less<T*>(l.p, r.p); }
    constexpr friend bool operator>=(const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return !(l < r); }
    constexpr friend bool operator> (const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return r < l; }
    constexpr friend bool operator<=(const task_memory_pointer& l, const task_memory_pointer& r) noexcept { return !(r < l); }

    constexpr T*& get_for_overwrite() noexcept {
        return p;
    }
private:
    T* p = nullptr;
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_SMART_POINTERS_H_
