#ifndef WINDOWS_COREAUDIO_WRAPPER_THROW_COM_ERROR_H_
#define WINDOWS_COREAUDIO_WRAPPER_THROW_COM_ERROR_H_

#include "comdef.h"

namespace coreaudio {

#ifndef COREAUDIO_NOEXCEPTIONS
/**
 * Throws `_com_error(hr)` if `hr` is not a success result like S_OK or S_FALSE.
 */
inline void throw_com_error(HRESULT hr) {
    if (FAILED(hr)) {
        throw _com_error(hr);
    }
}
#endif

}


#endif  // WINDOWS_COREAUDIO_WRAPPER_THROW_COM_ERROR_H_
