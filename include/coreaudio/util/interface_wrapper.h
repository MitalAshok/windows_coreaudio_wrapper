#ifndef WINDOWS_COREAUDIO_WRAPPER_INTERFACE_WRAPPER_H_
#define WINDOWS_COREAUDIO_WRAPPER_INTERFACE_WRAPPER_H_

#include "coreaudio/util/smart_pointers.h"

namespace coreaudio {
namespace detail {

template<typename T>
struct interface_wrapper {
    using raw_interface = T;

    constexpr interface_wrapper() noexcept = default;
    constexpr explicit interface_wrapper(std::nullptr_t) noexcept {}
    constexpr explicit interface_wrapper(raw_interface* ptr) noexcept : value(ptr) {}
    constexpr interface_wrapper(interface_wrapper&&) noexcept = default;
    constexpr interface_wrapper& operator=(interface_wrapper&&) noexcept = default;

    constexpr explicit operator bool() const noexcept {
        return value.operator bool();
    }
    constexpr com_pointer<raw_interface> release() noexcept {
        return static_cast<com_pointer<raw_interface>&&>(value);
    }
    constexpr raw_interface* get_raw() const noexcept {
        return value.get();
    }
protected:
    com_pointer<raw_interface> value;
};

}
}

#endif  // WINDOWS_COREAUDIO_WRAPPER_INTERFACE_WRAPPER_H_
