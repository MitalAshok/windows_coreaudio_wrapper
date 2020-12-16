#ifndef WINDOWS_COREAUDIO_WRAPPER_DEVICE_COLLECTION_H_
#define WINDOWS_COREAUDIO_WRAPPER_DEVICE_COLLECTION_H_

#include <vector>
#include <new>

#include "coreaudio/device_enumerator.h"
#include "coreaudio/device.h"
#include "coreaudio/util/indexed_dynamic_iterator.h"
#include "coreaudio/util/throw_com_error.h"
#include "coreaudio/util/h_optional.h"
#include "coreaudio/util/interface_wrapper.h"

namespace coreaudio {

struct device_collection : detail::interface_wrapper<IMMDeviceCollection> {
private:
    using super = detail::interface_wrapper<IMMDeviceCollection>;
public:
    constexpr device_collection() noexcept : super(), count(0) {}
#ifndef COREAUDIO_NOEXCEPTIONS
    explicit device_collection(raw_interface* ptr) : super(ptr) {
        if (ptr) {
            throw_com_error(ptr->GetCount(&count));
        } else {
            count = 0;
        }
    }
#endif
    explicit device_collection(std::nullptr_t) noexcept : super(nullptr), count(0) {}

    static h_optional<device_collection> make(raw_interface* ptr) noexcept {
        if (!ptr) {
            return { S_OK, device_collection(nullptr) };
        }
        device_collection c(std::nothrow, ptr);
        HRESULT status = ptr->GetCount(&c.count);
        return { status, static_cast<device_collection&&>(c) };
    }

    device_collection(device_collection&&) noexcept = default;
    device_collection& operator=(device_collection&&) noexcept = default;

    using value_type = device;
    using size_type = UINT;
    using difference_type = INT;
    using reference = value_type;

    bool empty() const noexcept {
        return size() == 0;
    }

    size_type size() const noexcept {
        return count;
    }

    device operator[](size_type n) const noexcept {
        return static_cast<device&&>(at(std::nothrow, n).get_unchecked());
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    device at(size_type n) const {
        return static_cast<device&&>(*at(std::nothrow, n));
    }
#endif

    h_optional<device> at(std::nothrow_t, size_type n) const noexcept {
        if (!(0 <= n && n < count) || !value) {
            return { E_INVALIDARG, device(nullptr) };
        }
        IMMDevice* p = nullptr;
        HRESULT status = value.get()->Item(n, &p);
        return { status, device(p) };
    }

    struct const_iterator : detail::indexed_dynamic_iterator_with_context<const_iterator, const device_collection, device, UINT, INT> {
        device operator*() const noexcept {
            return operator[](0);
        }
        device operator[](difference_type n) const noexcept {
            if (!context) return device(nullptr);
            return context->operator[](index + n);
        }

#ifndef COREAUDIO_NOEXCEPTIONS
        device at(difference_type n = 0) const {
            if (!context) throw _com_error(E_INVALIDARG);
            return context->at(index + n);
        }
#endif
        h_optional<device> at(std::nothrow_t, difference_type n = 0) const noexcept {
            if (!context) return { E_INVALIDARG, device(nullptr) };
            return context->at(std::nothrow, index + n);
        }
    private:
        using super = detail::indexed_dynamic_iterator_with_context<const_iterator, const device_collection, device, UINT, INT>;
        friend struct device_collection;
    public:
        using super::super;
    };
    using iterator = const_iterator;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const_iterator begin() const noexcept { return const_iterator(this, 0); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator end() const noexcept { return const_iterator(this, count); }
    const_iterator cend() const noexcept { return end(); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

#ifndef COREAUDIO_NOEXCEPTIONS
    template<typename Alloc = std::allocator<device>>
    std::vector<device, Alloc> to_vector(const Alloc& alloc = {}) const {
        std::vector<device, Alloc> result(alloc);
        result.reserve(count);
        for (UINT i = 0; i < count; ++i) {
            result.push_back(at(i));
        }
        return result;
    }
#endif

    // Throws whatever `vec.reserve` throws (usually `std::bad_alloc`)
    // Never throws if there is enough capacity for `size()` more items.
    // Returns number written and status code
    template<typename Alloc>
    h_optional<UINT> to_vector(std::nothrow_t, std::vector<device, Alloc>& vec) const noexcept(noexcept(vec.reserve(std::size_t{}))) {
        std::size_t required_capacity = vec.size() + count;
        if (vec.capacity() < required_capacity) {
            vec.reserve(required_capacity);
        }
        for (UINT i = 0; i < count; ++i) {
            h_optional<device> d = at(std::nothrow, i);
            if (!d) return { d.get_status(), i };
            vec.push_back(static_cast<device&&>(d.get_unchecked()));
        }
        return { S_OK, count };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    template<typename OutputIt>
    void to_output_iterator(OutputIt it) const {
        for (UINT i = 0; i < count; ++i) {
            *it++ = static_cast<device&&>(at(i));
        }
    }
#endif

    template<typename OutputIt>
    h_optional<UINT> to_output_iterator(std::nothrow_t, OutputIt it) const noexcept(noexcept(*it++ = device())){
        for (UINT i = 0; i < count; ++i) {
            h_optional<device> d = at(std::nothrow, i);
            if (!d) return { d.get_status(), i };
            *it++ = static_cast<device&&>(d.get_unchecked());
        }
        return { S_OK, count };
    }

    template<typename OutputIt, typename Sentinel = OutputIt>
    h_optional<UINT> to_output_iterator(std::nothrow_t, OutputIt it, Sentinel s) const noexcept(noexcept(it == s) && noexcept(*it++ = device())){
        for (UINT i = 0; i < count; ++i) {
            if (it == s) return { E_INVALIDARG, i };
            h_optional<device> d = at(std::nothrow, i);
            if (!d) return { d.get_status(), i };
            *it++ = static_cast<device&&>(d.get_unchecked());
        }
        return { S_OK, count };
    }

#ifndef COREAUDIO_NOEXCEPTIONS
    // Invalidates all iterators if this doesn't throw
    void update(const device_enumerator& d_enum, EDataFlow data_flow, DWORD state_mask = DEVICE_STATEMASK_ALL) {
        *this = static_cast<device_collection&&>(d_enum.enum_audio_endpoints(data_flow, state_mask));
    }
#endif
    // Invalidates all iterators if this succeeds
    HRESULT update(std::nothrow_t, const device_enumerator& d_enum, EDataFlow data_flow, DWORD state_mask = DEVICE_STATEMASK_ALL) noexcept {
        h_optional<device_collection> new_collection = d_enum.enum_audio_endpoints(std::nothrow, data_flow, state_mask);
        if (new_collection) {
            *this = static_cast<device_collection&&>(new_collection.get_unchecked());
        }
        return new_collection.get_status();
    }
private:
    explicit device_collection(std::nothrow_t, raw_interface* ptr) noexcept : super(ptr) {}

    UINT count;
};

#ifndef COREAUDIO_NOEXCEPTIONS
inline device_collection device_enumerator::enum_audio_endpoints(EDataFlow data_flow, DWORD state_mask = DEVICE_STATEMASK_ALL) const  {
    IMMDeviceCollection* ptr;
    throw_com_error(value->EnumAudioEndpoints(data_flow, state_mask, &ptr));
    return device_collection(ptr);
}
#endif

inline h_optional<device_collection> device_enumerator::enum_audio_endpoints(std::nothrow_t, EDataFlow data_flow, DWORD state_mask = DEVICE_STATEMASK_ALL) const noexcept {
    if (!value) {
        return { E_INVALIDARG, device_collection(nullptr) };
    }
    IMMDeviceCollection* ptr;
    HRESULT status = value.get()->EnumAudioEndpoints(data_flow, state_mask, &ptr);
    if (FAILED(status)) {
        return { status, device_collection(nullptr) };
    }
    return device_collection::make(ptr);
}

}

#endif  // WINDOWS_COREAUDIO_WRAPPER_DEVICE_COLLECTION_H_
