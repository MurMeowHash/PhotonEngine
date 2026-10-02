add_library(DebugModule STATIC)

target_sources(DebugModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Logger.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/DebugModule.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Logger.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/DebugModule.cpp
)

target_include_directories(DebugModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(DebugModule
        PRIVATE
        ModuleBaseModule.Core
        CoreModule
)

if(MSVC)
    target_compile_options(DebugModule PRIVATE /W4)
else()
    target_compile_options(DebugModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(DebugModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
