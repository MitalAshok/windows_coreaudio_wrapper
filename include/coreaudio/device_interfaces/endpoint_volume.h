#ifndef WINDOWS_COREAUDIO_WRAPPER_ENDPOINT_VOLUME_H_
#define WINDOWS_COREAUDIO_WRAPPER_ENDPOINT_VOLUME_H_

#include "endpointvolume.h"

#include "coreaudio/util/interface_wrapper.h"

namespace coreaudio {

struct endpoint_volume : detail::interface_wrapper<IAudioEndpointVolume> {
private:
    using super = detail::interface_wrapper<IAudioEndpointVolume>;
public:
    constexpr endpoint_volume() noexcept = default;

    explicit endpoint_volume(raw_interface* ptr) noexcept : super(ptr) {}

    endpoint_volume(endpoint_volume&&) noexcept = default;
    endpoint_volume& operator=(endpoint_volume&&) noexcept = default;

#ifndef COREAUDIO_NOEXCEPTIONS
    UINT get_channel_count() const {
        UINT result = 0;
        throw_com_error(value->GetChannelCount(&result));
        return result;
    }
#endif
    h_optional<UINT> get_channel_count(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        UINT result = 0;
        HRESULT status = value.get()->GetChannelCount(&result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    float get_channel_volume_level_dB(UINT i) const {
        float result = 0;
        throw_com_error(value->GetChannelVolumeLevel(i, &result));
        return result;
    }
#endif
    h_optional<float> get_channel_volume_level_dB(std::nothrow_t, UINT i) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        float result = 0;
        HRESULT status = value.get()->GetChannelVolumeLevel(i, &result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    float get_channel_volume_level_scalar(UINT i) const {
        float result = -1;
        throw_com_error(value->GetChannelVolumeLevelScalar(i, &result));
        return result;
    }
#endif
    h_optional<float> get_channel_volume_level_scalar(std::nothrow_t, UINT i) const noexcept {
        if (!value) return { E_INVALIDARG, -1 };
        float result = -1;
        HRESULT status = value.get()->GetChannelVolumeLevelScalar(i, &result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    float get_master_volume_level_dB() const {
        float result = 0;
        throw_com_error(value->GetMasterVolumeLevel(&result));
        return result;
    }
#endif
    h_optional<float> get_master_volume_level_dB(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        float result = 0;
        HRESULT status = value.get()->GetMasterVolumeLevel(&result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    float get_master_volume_level_scalar() const {
        float result = 0;
        throw_com_error(value->GetMasterVolumeLevelScalar(&result));
        return result;
    }
#endif
    h_optional<float> get_master_volume_level_scalar(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        float result = 0;
        HRESULT status = value.get()->GetMasterVolumeLevelScalar(&result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    bool is_muted() const {
        BOOL result = 0;
        throw_com_error(value->GetMute(&result));
        return result != 0;
    }
#endif
    h_optional<bool> is_muted(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, false };
        BOOL result = 0;
        HRESULT status = value.get()->GetMute(&result);
        return { status, result != 0 };
    }

    struct volume_range {
        float min_dB;
        float max_dB;
        float increment_dB;
    };

#ifndef COREAUDIO_NOEXCEPTIONS
    volume_range get_volume_range() const {
        volume_range result{0, 0, 0};
        throw_com_error(value->GetVolumeRange(&result.min_dB, &result.max_dB, &result.increment_dB));
        return result;
    }
#endif
    h_optional<volume_range> get_volume_range(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, { 0, 0, 0 } };
        volume_range result{0, 0, 0};
        HRESULT status = value.get()->GetVolumeRange(&result.min_dB, &result.max_dB, &result.increment_dB);
        return { status, result };
    }

private:
    mutable HRESULT step_count_cache_status = E_POINTER;
    mutable UINT step_count_cache = 0;
public:
#ifndef COREAUDIO_NOEXCEPTIONS
    UINT get_current_step_index() const {
        UINT step = 0;
        if (FAILED(step_count_cache_status)) {
            UINT step_count = 0;
            HRESULT hr = value->GetVolumeStepInfo(&step, &step_count);
            step_count_cache_status = hr;
            step_count_cache = step_count;
            throw_com_error(hr);
        } else {
            throw_com_error(value->GetVolumeStepInfo(&step, nullptr));
        }
        return step;
    }
#endif
    h_optional<UINT> get_current_step_index(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        UINT step = 0;
        HRESULT hr;
        if (FAILED(step_count_cache_status)) {
            UINT step_count = 0;
            hr = value.get()->GetVolumeStepInfo(&step, &step_count);
            step_count_cache_status = hr;
            step_count_cache = step_count;
        } else {
            hr = value.get()->GetVolumeStepInfo(&step, nullptr);
        }
        return { hr, step };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    UINT get_step_count() const {
        return *get_step_count(std::nothrow);
    }
#endif
    h_optional<UINT> get_step_count(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
        if (FAILED(step_count_cache_status)) {
            UINT step_count = 0;
            HRESULT hr = value.get()->GetVolumeStepInfo(nullptr, &step_count);
            step_count_cache_status = hr;
            step_count_cache = step_count;
        }
        return { step_count_cache_status, step_count_cache };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    DWORD query_hardware_support() const {
        DWORD result = 0;
        throw_com_error(value->QueryHardwareSupport(&result));
        return result;
    }
#endif
    h_optional<DWORD> query_hardware_support(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };  // Check before because it may have been released
        DWORD result = 0;
        HRESULT status = value.get()->QueryHardwareSupport(&result);
        return { status, result };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void register_control_change_notify(IAudioEndpointVolumeCallback* callback) const {
        throw_com_error(value->RegisterControlChangeNotify(callback));
    }
#endif
    HRESULT register_control_change_notify(std::nothrow_t, IAudioEndpointVolumeCallback* callback) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->RegisterControlChangeNotify(callback);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void set_channel_volume_level_dB(UINT i, float level_dB, const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->SetChannelVolumeLevel(i, level_dB, &event_context));
    }
#endif
    HRESULT set_channel_volume_level_dB(std::nothrow_t, UINT i, float level_dB, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->SetChannelVolumeLevel(i, level_dB, &event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void set_channel_volume_level_scalar(UINT i, float level, const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->SetChannelVolumeLevel(i, level, &event_context));
    }
#endif
    HRESULT set_channel_volume_level_scalar(std::nothrow_t, UINT i, float level, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->SetChannelVolumeLevel(i, level, &event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void set_master_volume_level_dB(float level_dB, const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->SetMasterVolumeLevel(level_dB, &event_context));
    }
#endif
    HRESULT set_master_volume_level_dB(std::nothrow_t, float level_dB, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->SetMasterVolumeLevel(level_dB, &event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void set_master_volume_level_scalar(float level, const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->SetMasterVolumeLevelScalar(level, &event_context));
    }
#endif
    HRESULT set_master_volume_level_scalar(std::nothrow_t, float level, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->SetMasterVolumeLevelScalar(level, &event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void set_mute(bool mute, const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->SetMute(mute ? TRUE : FALSE, &event_context));
    }
#endif
    HRESULT set_mute(std::nothrow_t, bool mute, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->SetMute(mute ? TRUE : FALSE, &event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void unregister_control_change_notify(IAudioEndpointVolumeCallback* callback) const {
        throw_com_error(value->UnregisterControlChangeNotify(callback));
    }
#endif
    HRESULT unregister_control_change_notify(std::nothrow_t, IAudioEndpointVolumeCallback* callback) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->UnregisterControlChangeNotify(callback);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void volume_step_down(const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->VolumeStepDown(&event_context));
    }
#endif
    HRESULT volume_step_down(std::nothrow_t, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->VolumeStepDown(&event_context);
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    void volume_step_up(const GUID& event_context = GUID_NULL) const {
        throw_com_error(value->VolumeStepUp(&event_context));
    }
#endif
    HRESULT volume_step_up(std::nothrow_t, const GUID& event_context = GUID_NULL) const noexcept {
        if (!value) return E_INVALIDARG;
        return value.get()->VolumeStepUp(&event_context);
    }
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_ENDPOINT_VOLUME_H_
