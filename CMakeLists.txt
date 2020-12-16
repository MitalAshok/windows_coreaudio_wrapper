cmake_minimum_required(VERSION 3.13.0)
project(windows_coreaudio_wrapper)

set(CMAKE_CXX_STANDARD 14)

add_library(windows_coreaudio_wrapper INTERFACE)

target_sources(windows_coreaudio_wrapper INTERFACE
        include/coreaudio/coreaudio.h
        include/coreaudio/util/utils.h
        include/coreaudio/util/h_optional.h
        include/coreaudio/util/indexed_dynamic_iterator.h
        include/coreaudio/util/interface_wrapper.h
        include/coreaudio/util/smart_pointers.h
        include/coreaudio/util/throw_com_error.h
        include/coreaudio/com_context.h
        include/coreaudio/device.h
        include/coreaudio/device_collection.h
        include/coreaudio/device_enumerator.h
)
target_include_directories(windows_coreaudio_wrapper INTERFACE include)

add_executable(windows_coreaudio_sample_print_devices sample/print_devices.cpp)
target_link_libraries(windows_coreaudio_sample_print_devices PRIVATE windows_coreaudio_wrapper)
