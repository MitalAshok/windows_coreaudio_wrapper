windows_coreaudio_wrapper
=========================

TODO: document

```c++
core_audio/
com_context -> CoInitializeEx/CoUninitialize RAII wrapper
device_enumerator -> IMMDeviceEnumerator* (Release RAII wrapper)
device_collection -> IMMDeviceCollection* (Release RAII wrapper / cache GetCount)
device -> IMMDevice* (Release RAII wrapper / TODO: finish wrapping API)

util/smart_pointers
com_pointer<T> -> RAII wrapper around Release on IUnknown*
task_memory_pointer<T> -> RAII wrapper around CoTaskMemFree

util/h_optional
h_optional<T> -> HRESULT/T pair, like optional<T>. operator* throws if FAILED(status)
```
