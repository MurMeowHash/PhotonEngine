add_library(EngineLoopModule.Core INTERFACE)

target_sources(EngineLoopModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IEngineLoop.h
)

target_include_directories(EngineLoopModule.Core
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if(MSVC)
    target_compile_options(EngineLoopModule.Core INTERFACE /W4)
else()
    target_compile_options(EngineLoopModule.Core INTERFACE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(EngineLoopModule.Core PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
