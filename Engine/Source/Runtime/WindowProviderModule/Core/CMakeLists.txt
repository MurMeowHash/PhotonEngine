add_library(WindowProviderModule.Core INTERFACE)

target_sources(WindowProviderModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowCreateInfo.h
)

target_include_directories(WindowProviderModule.Core
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if(MSVC)
    target_compile_options(WindowProviderModule.Core INTERFACE /W4)
else()
    target_compile_options(WindowProviderModule.Core INTERFACE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(WindowProviderModule.Core PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
