add_library(ModuleBaseModule.Core STATIC)

target_sources(ModuleBaseModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ModuleBase.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ModuleSequence.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ModuleBlueprint.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ModuleGlobals.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/ModuleBase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/ModuleSequence.cpp
)

target_include_directories(ModuleBaseModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(ModuleBaseModule.Core
        PRIVATE
        CoreModule
)

if(MSVC)
    target_compile_options(ModuleBaseModule.Core PRIVATE /W4)
else()
    target_compile_options(ModuleBaseModule.Core PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(ModuleBaseModule.Core PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
