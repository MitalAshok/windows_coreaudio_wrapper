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
    mutable h_optional<UINT> step_count_cache = { E_POINTER, 0 };
public:
#ifndef COREAUDIO_NOEXCEPTIONS
    UINT get_current_step_index() const {
        UINT step = 0;
        if (!step_count_cache) {
            UINT step_count = 0;
            HRESULT hr = value->GetVolumeStepInfo(&step, &step_count);
            step_count_cache = { hr, step_count };
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
        if (!step_count_cache) {
            UINT step_count = 0;
            hr = value.get()->GetVolumeStepInfo(&step, &step_count);
            step_count_cache = { hr, step_count };
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
        if (!step_count_cache) {
            UINT step_count = 0;
            HRESULT hr = value.get()->GetVolumeStepInfo(nullptr, &step_count);
            step_count_cache = { hr, step_count };
        }
        return step_count_cache;
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    DWORD query_hardware_support() const {
        DWORD result = 0;
        throw_com_error(value->QueryHardwareSupport(&result));
        return result;
    }
#endif
    h_optional<DWORD> query_hardware_support(std::nothrow_t) const noexcept {
        if (!value) return { E_INVALIDARG, 0 };
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

    // TODO: finish
};

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_ENDPOINT_VOLUME_H_
