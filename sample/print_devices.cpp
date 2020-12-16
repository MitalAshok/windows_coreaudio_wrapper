#include <Windows.h>
#include <tchar.h>

#include <map>
#include <iostream>

// #define COREAUDIO_NOEXCEPTIONS
#include "coreaudio/coreaudio.h"

#ifdef UNICODE
#define tcout (::std::wcout)
#define tcerr (::std::wcerr)
#else
#define tcout (::std::cout)
#define tcerr (::std::cerr)
#endif


std::map<DWORD, const wchar_t*>* namep;

#ifndef COREAUDIO_NOEXCEPTIONS
void test(const coreaudio::com_context& ctx) {
    using namespace coreaudio;
    device_enumerator e(ctx);
    device_collection coll = e.enum_audio_endpoints(eCapture);
    for (device f : coll) {
        std::wcout << f.id().get() << L": " << (*namep)[f.state()] << L"\n";
    }
}


int main() {
    std::map<DWORD, const wchar_t*> name;
    namep = &name;
    name[DEVICE_STATE_ACTIVE] = L"active";
    name[DEVICE_STATE_DISABLED] = L"disabled";
    name[DEVICE_STATE_NOTPRESENT] = L"not present";
    name[DEVICE_STATE_UNPLUGGED] = L"unplugged";

    try {
        // test(coreaudio::com_context::adopt_initialization{});
        test(coreaudio::com_context{});
    } catch (const _com_error& e) {
        std::cerr << "Caught _com_error(" << e.Error() << ")\n" << std::flush;
        const auto* message = e.ErrorMessage();
        tcerr << message << std::endl;
        MessageBox(nullptr, message, _T("Error"), MB_OK | MB_ICONERROR);
        return e.Error();
    }
}
#else
HRESULT test(const coreaudio::com_context& ctx) noexcept {
    using namespace coreaudio;
    device_enumerator e;
    {
        h_optional<device_enumerator> attempt_e = device_enumerator::make(ctx);
        if (!attempt_e) return attempt_e.get_status();
        e = static_cast<device_enumerator&&>(attempt_e.get_unchecked());
    }
    device_collection coll;
    {
        h_optional<device_collection> attempt_coll = e.enum_audio_endpoints(std::nothrow, eCapture);
        if (!attempt_coll) return attempt_coll.get_status();
        coll = static_cast<device_collection&&>(attempt_coll.get_unchecked());
    }
    for (std::size_t i = 0; i < coll.size(); ++i) {
        h_optional<device> d = coll.at(std::nothrow, i);
        if (!d) return d.get_status();
        h_optional<task_memory_pointer<WCHAR>> id = d.get_unchecked().id(std::nothrow);
        if (!id) return id.get_status();
        std::wcout << id.get_unchecked().get() << L": ";
        h_optional<DWORD> state = d.get_unchecked().state(std::nothrow);
        if (!state) return state.get_status();
        std::wcout << (*namep)[state.get_unchecked()] << L"\n";
    }
    return S_OK;
}


int main() {
    std::map<DWORD, const wchar_t*> name;
    namep = &name;
    name[DEVICE_STATE_ACTIVE] = L"active";
    name[DEVICE_STATE_DISABLED] = L"disabled";
    name[DEVICE_STATE_NOTPRESENT] = L"not present";
    name[DEVICE_STATE_UNPLUGGED] = L"unplugged";

    using namespace coreaudio;

    HRESULT status;

    com_context c = com_context::adopt_initialization{};

    {
        h_optional<com_context> attempt_initialize = com_context::make();
        status = attempt_initialize.get_status();
        if (SUCCEEDED(status)) c = static_cast<com_context&&>(attempt_initialize.get_unchecked());
    }

    if (SUCCEEDED(status)) {
        status = test(c);
    }

    if (FAILED(status)) {
        std::cerr << "HRESULT error(" << status << ")\n" << std::flush;
        const auto* message = _com_error(status).ErrorMessage();
        tcerr << message << std::endl;
        MessageBox(nullptr, message, _T("Error"), MB_OK | MB_ICONERROR);
        return status;
    }
}
#endif
