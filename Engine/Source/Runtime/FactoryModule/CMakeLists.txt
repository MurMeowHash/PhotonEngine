add_library(FactoryModule INTERFACE)

target_sources(FactoryModule
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/FactoryGlobals.h
)

target_include_directories(FactoryModule
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if(MSVC)
    target_compile_options(FactoryModule INTERFACE /W4)
else()
    target_compile_options(FactoryModule INTERFACE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(FactoryModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
