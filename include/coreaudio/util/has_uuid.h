#ifndef WINDOWS_COREAUDIO_WRAPPER_HAS_UUID_H_
#define WINDOWS_COREAUDIO_WRAPPER_HAS_UUID_H_

#include "unknwn.h"

#include <type_traits>


namespace coreaudio {
namespace detail {

template<typename T> std::true_type _has_uuid_check(int) noexcept(noexcept(__uuidof(T)));
template<typename> std::false_type _has_uuid_check(long);

template<typename T>
struct has_uuid : decltype(_has_uuid_check<T>(0)) {};

template<typename T>
struct maybe_com_interface : std::integral_constant<bool,
    !std::is_const<T>::value && !std::is_volatile<T>::value &&
    !std::is_reference<T>::value && std::is_class<T>::value &&
    std::is_polymorphic<T>::value && std::is_abstract<T>::value &&
    has_uuid<T>::value && std::is_base_of<IUnknown, T>::value
> {};

}
}

#endif  // WINDOWS_COREAUDIO_WRAPPER_HAS_UUID_H_
