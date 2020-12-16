#ifndef WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_
#define WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_

#include "mmdeviceapi.h"
#include "winerror.h"

#include <new>

#include "coreaudio/util/smart_pointers.h"
#include "coreaudio/util/h_optional.h"
#include "coreaudio/util/throw_com_error.h"
#include "coreaudio/util/interface_wrapper.h"


namespace coreaudio {

struct device : detail::interface_wrapper<IMMDevice> {
private:
    using super = detail::interface_wrapper<IMMDevice>;
public:
    constexpr device() noexcept = default;

    explicit device(raw_interface* ptr) noexcept : super(ptr) {}

    device(device&&) noexcept = default;
    device& operator=(device&&) noexcept = default;

#ifndef COREAUDIO_NOEXCEPTIONS
    task_memory_pointer<WCHAR> id() const {
        LPWSTR x;
        throw_com_error(value->GetId(&x));
        return task_memory_pointer<WCHAR>(x);
    }
#endif
    h_optional<task_memory_pointer<WCHAR>> id(std::nothrow_t) const {
        if (!value) return { E_INVALIDARG, nullptr };
        LPWSTR x = nullptr;
        HRESULT status = value.get()->GetId(&x);
        return { status, task_memory_pointer<WCHAR>(x) };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    DWORD state() const {
        DWORD x;
        throw_com_error(value->GetState(&x));
        return x;
    }
#endif
    h_optional<DWORD> state(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        DWORD x = 0;
        HRESULT status = value.get()->GetState(&x);
        return { status, x };
    }

    // TODO: value->Activate, value->OpenPropertyStore
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_
