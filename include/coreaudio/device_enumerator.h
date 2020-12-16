#ifndef WINDOWS_COREAUDIO_WRAPPER_DEVICE_ENUMERATOR_H_
#define WINDOWS_COREAUDIO_WRAPPER_DEVICE_ENUMERATOR_H_

#include "combaseapi.h"
#include "mmdeviceapi.h"

#include <new>

#include "coreaudio/device.h"
#include "coreaudio/com_context.h"
#include "coreaudio/util/throw_com_error.h"
#include "coreaudio/util/smart_pointers.h"
#include "coreaudio/util/h_optional.h"
#include "coreaudio/util/interface_wrapper.h"

namespace coreaudio {

struct device_collection;

struct device_enumerator : detail::interface_wrapper<IMMDeviceEnumerator> {
private:
    static HRESULT create(raw_interface*& to) noexcept {
        return CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(&to)
        );
    }
    using super = detail::interface_wrapper<IMMDeviceEnumerator>;
public:
    static h_optional<raw_interface*> make_raw(const com_context&) noexcept {
        raw_interface* ptr = nullptr;
        HRESULT status = create(ptr);
        return { status, ptr };
    }

    constexpr device_enumerator() noexcept = default;

#ifndef COREAUDIO_NOEXCEPTIONS
    explicit device_enumerator(const com_context&) : super(nullptr) {
        throw_com_error(create(value.get_for_overwrite()));
    }
#endif
    explicit device_enumerator(raw_interface* ptr) noexcept : super(ptr) {}

    device_enumerator(device_enumerator&&) noexcept = default;
    device_enumerator& operator=(device_enumerator&&) noexcept = default;

    static h_optional<device_enumerator> make(const com_context& c) noexcept {
        device_enumerator d_enumerator(nullptr);
        HRESULT status = create(d_enumerator.value.get_for_overwrite());
        return { status, static_cast<device_enumerator&&>(d_enumerator) };
    }

    /* `#include "coreaudio/device_collection.h"` to bring in the definition
     * NOTE: These are static collections. They will not be dynamically updated whenever a device is installed
     * or uninstalled. This means that we cache `IMMDeviceCollection::GetCount`, and any errors thrown by that
     * can be thrown by this function (And this will only throw in very exceptional circumstances, like E_OUTOFMEMORY).
     * This also means that you have to call this again periodically.
     */
#ifndef COREAUDIO_NOEXCEPTIONS
    inline device_collection enum_audio_endpoints(EDataFlow data_flow, DWORD state_mask /* = DEVICE_STATEMASK_ALL */) const;
#endif
    inline h_optional<device_collection> enum_audio_endpoints(std::nothrow_t, EDataFlow data_flow, DWORD state_mask /* = DEVICE_STATEMASK_ALL */) const noexcept;

#ifndef COREAUDIO_NOEXCEPTIONS
    // Returns a null device if not found. Throws _com_error on other errors
    device get_default_endpoint(EDataFlow data_flow, ERole role = eConsole) const {
        IMMDevice* d;
        HRESULT hr = value->GetDefaultAudioEndpoint(data_flow, role, &d);
        if (SUCCEEDED(hr)) return device(d);
        if (hr == E_NOTFOUND) return device(nullptr);
        throw _com_error(hr);
    }
#endif
    h_optional<device> get_default_endpoint(std::nothrow_t, EDataFlow data_flow, ERole role = eConsole) const noexcept {
        if (!value) return { E_INVALIDARG, device(nullptr) };
        IMMDevice* d = nullptr;
        HRESULT hr = value.get()->GetDefaultAudioEndpoint(data_flow, role, &d);
        return { hr, device(d) };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    // Returns a null device if not found. Throws _com_error on other errors
    device get_device(LPCWSTR id) const {
        IMMDevice* d;
        HRESULT hr = value->GetDevice(id, &d);
        if (SUCCEEDED(hr)) return device(d);
        if (hr == E_NOTFOUND) return device(nullptr);
        throw _com_error(hr);
    }
#endif

    h_optional<device> get_device(std::nothrow_t, LPCWSTR id) const noexcept {
        if (!value) return { E_INVALIDARG, device(nullptr) };
        IMMDevice* d = nullptr;
        HRESULT hr = value.get()->GetDevice(id, &d);
        return { hr, device(d) };
    }

    HRESULT register_endpoint_notification_callback(std::nothrow_t, IMMNotificationClient* callback) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->RegisterEndpointNotificationCallback(callback);
    }
#ifndef COREAUDIO_NOEXCEPTIONS
    void register_endpoint_notification_callback(IMMNotificationClient* callback) const {
        throw_com_error(value->RegisterEndpointNotificationCallback(callback));
    }
#endif

    HRESULT unregister_endpoint_notification_callback(std::nothrow_t, IMMNotificationClient* callback) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->UnregisterEndpointNotificationCallback(callback);
    }
#ifndef COREAUDIO_NOEXCEPTIONS
    void unregister_endpoint_notification_callback(IMMNotificationClient* callback) const {
        throw_com_error(value->UnregisterEndpointNotificationCallback(callback));
    }
#endif
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_DEVICE_ENUMERATOR_H_
