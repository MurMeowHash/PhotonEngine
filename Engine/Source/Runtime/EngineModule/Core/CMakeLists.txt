add_library(EngineModule.Core STATIC)

target_sources(EngineModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IEngine.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/EngineModule.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IEngineFactory.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/EngineModule.cpp
)

target_include_directories(EngineModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(EngineModule.Core
        PUBLIC
        CoreModule
        PRIVATE
        ModuleBaseModule.Core
)

if(MSVC)
    target_compile_options(EngineModule.Core PRIVATE /W4)
else()
    target_compile_options(EngineModule.Core PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(EngineModule.Core PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
