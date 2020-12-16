#ifndef WINDOWS_COREAUDIO_WRAPPER_H_OPTIONAL_H_
#define WINDOWS_COREAUDIO_WRAPPER_H_OPTIONAL_H_

#include "winerror.h"

#include <new>
#include <memory>
#include <type_traits>

#include "coreaudio/util/throw_com_error.h"

namespace coreaudio {

/**
 * Like `std::optional<T>`, but holds an `HRESULT` member like `SUCCESS(status)` and always holds a value.
 * Conceptually holds a `std::pair<const HRESULT, T>`
 *
 * Throws a `_com_error(status)` if you try to access while not holding a value.
 *
 * @tparam T The possibly-held type.
 */
template<typename T>
struct h_optional {
    using value_type = T;

    h_optional() = delete;

    h_optional(const h_optional&) = default;
    h_optional(h_optional&&) = default;
    h_optional(HRESULT status, const T& value) : status(status), value(value) {}
    h_optional(HRESULT status, T&& value) : status(status), value(static_cast<T&&>(value)) {}

    h_optional& operator=(const h_optional&) = default;
    h_optional& operator=(h_optional&&) = default;

#ifndef COREAUDIO_NOEXCEPTIONS
    const T& operator*() const {
        throw_com_error(status);
        return value;
    }
    T& operator*() {
        throw_com_error(status);
        return value;
    }

    const T* operator->() const { return ::std::addressof(**this); }
    T* operator->() { return ::std::addressof(**this); }
#endif

    const T& get_unchecked() const noexcept {
        return value;
    }
    T& get_unchecked() noexcept {
        return value;
    }
    HRESULT get_status() const noexcept {
        return status;
    }
    bool has_value() const noexcept {
        return SUCCEEDED(status);
    }
    explicit operator bool() const noexcept {
        return has_value();
    }
private:
    const HRESULT status;
    T value;
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_H_OPTIONAL_H_
