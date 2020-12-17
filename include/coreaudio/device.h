#ifndef WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_
#define WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_

#include "mmdeviceapi.h"
#include "winerror.h"
#include "unknwn.h"

#include <new>
#include <type_traits>

#include "coreaudio/device_interfaces/endpoint_volume.h"
#include "coreaudio/util/smart_pointers.h"
#include "coreaudio/util/h_optional.h"
#include "coreaudio/util/has_uuid.h"
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

#ifndef COREAUDIO_NOEXCEPTIONS
    com_pointer<IPropertyStore> open_property_store(DWORD stgm_access = STGM_READ) const {
        com_pointer<IPropertyStore> result;
        throw_com_error(value->OpenPropertyStore(stgm_access, &result.get_for_overwrite()));
        return result;
    }
#endif
    h_optional<com_pointer<IPropertyStore>> open_property_store(std::nothrow_t, DWORD stgm_access) const noexcept {
        if (!value) return { E_INVALIDARG, nullptr };
        com_pointer<IPropertyStore> result;
        HRESULT status = value.get()->OpenPropertyStore(stgm_access, &result.get_for_overwrite());
        return { status, static_cast<com_pointer<IPropertyStore>&&>(result) };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    template<typename T>
    com_pointer<T> activate(PROPVARIANT* activation_params = nullptr, DWORD class_ctx = CLSCTX_ALL) const {
        static_assert(detail::maybe_com_interface<T>::value, "device::activate<T>(): T is not a valid COM interface to activate");

        com_pointer<T> result;
        throw_com_error(value->Activate(
            __uuidof(T), class_ctx, activation_params, reinterpret_cast<void**>(&result.get_for_overwrite())
        ));
        return result;
    }
#endif
    template<typename T>
    h_optional<com_pointer<T>> activate(std::nothrow_t, PROPVARIANT* activation_params = nullptr, DWORD class_ctx = CLSCTX_ALL) const {
        detail::maybe_com_interface<T>::type::value;
        static_assert(detail::maybe_com_interface<T>::value, "device::activate<T>(): T is not a valid COM interface to activate");
        if (!value) return { E_INVALIDARG, nullptr };

        com_pointer<T> result;
        HRESULT status = value.get()->Activate(
            __uuidof(T), class_ctx, activation_params, reinterpret_cast<void**>(&result.get_for_overwrite())
        );
        return { status, static_cast<com_pointer<T>&&>(result) };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    endpoint_volume activate_endpoint_volume(DWORD class_ctx = CLSCTX_ALL) const {
        return endpoint_volume(activate<typename endpoint_volume::raw_interface>(nullptr, class_ctx).release());
    }
#endif
    h_optional<endpoint_volume> activate_endpoint_volume(std::nothrow_t, DWORD class_ctx = CLSCTX_ALL) const noexcept {
        h_optional<com_pointer<typename endpoint_volume::raw_interface>> result = activate<typename endpoint_volume::raw_interface>(std::nothrow, nullptr, class_ctx);
        return { result.get_status(), endpoint_volume(result.get_unchecked().release()) };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    com_pointer<IMMEndpoint> as_endpoint() const {
        com_pointer<IMMEndpoint> result;
        value->QueryInterface(__uuidof(IMMEndpoint), reinterpret_cast<void**>(&result.get_for_overwrite()));
        return result;
    }
#endif
    h_optional<com_pointer<IMMEndpoint>> as_endpoint(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, nullptr };
        com_pointer<IMMEndpoint> result;
        HRESULT status = value.get()->QueryInterface(__uuidof(IMMEndpoint), reinterpret_cast<void**>(&result.get_for_overwrite()));
        return { status, static_cast<com_pointer<IMMEndpoint>&&>(result) };
    }
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_DEVICE_H_
