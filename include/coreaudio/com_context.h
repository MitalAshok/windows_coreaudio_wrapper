#ifndef WINDOWS_COREAUDIO_WRAPPER_COM_CONTEXT_H_
#define WINDOWS_COREAUDIO_WRAPPER_COM_CONTEXT_H_

#include "windows.h"
#include "combaseapi.h"
#include "roapi.h"

#include "coreaudio/util/throw_com_error.h"
#include "coreaudio/util/h_optional.h"


namespace coreaudio {

struct com_context {
#ifndef COREAUDIO_NOEXCEPTIONS
    explicit com_context(DWORD coinit = COINIT_APARTMENTTHREADED) {
        throw_com_error(CoInitializeEx(nullptr, coinit));
    }
#endif

    // Use `com_context(com_context::adopt_initialization{})` to not call `CoInitializeEx` and `CoUninitialize`
    // (If CoInitializeEx has not been called on this thread, many API calls will error with `CO_E_NOTINITIALIZED`)
    struct adopt_initialization {};
    constexpr com_context(adopt_initialization) noexcept : need_to_uninitialize(false) {}
    static h_optional<com_context> make(DWORD coinit = COINIT_APARTMENTTHREADED) noexcept {
        HRESULT status = CoInitializeEx(nullptr, coinit);
        return { status, com_context(static_cast<bool>(SUCCEEDED(status))) };
    }

    constexpr com_context(com_context&& other) noexcept : need_to_uninitialize(other.need_to_uninitialize) {
        other.need_to_uninitialize = false;
    }
    com_context& operator=(com_context&& other) noexcept {
        if (this != &other && !need_to_uninitialize) {
            need_to_uninitialize = other.need_to_uninitialize;
            other.need_to_uninitialize = false;
        }
        return *this;
    }

    ~com_context() {
        if (need_to_uninitialize) CoUninitialize();
    }
private:
    constexpr explicit com_context(bool need) noexcept : need_to_uninitialize(need) {}
    bool need_to_uninitialize = true;
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_COM_CONTEXT_H_
