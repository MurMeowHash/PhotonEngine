add_library(DelegateModule INTERFACE)

target_sources(DelegateModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IDelegate.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ObjectDelegate.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/StaticDelegate.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IFunc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ObjectFunc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/StaticFunc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Event.h
)

target_include_directories(DelegateModule
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if(MSVC)
    target_compile_options(DelegateModule INTERFACE /W4)
else()
    target_compile_options(DelegateModule INTERFACE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(DelegateModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
