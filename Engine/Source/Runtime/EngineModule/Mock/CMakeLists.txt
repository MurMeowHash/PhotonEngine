add_library(EngineModule.Mock STATIC)

target_sources(EngineModule.Mock
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MockEngine.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MockEngineModule.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MockEngineFactory.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/MockEngine.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/MockEngineModule.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/MockEngineFactory.cpp
)

target_include_directories(EngineModule.Mock
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(EngineModule.Mock
        PRIVATE
        EngineModule.Core
        ModuleBaseModule.Core
)

if(MSVC)
    target_compile_options(EngineModule.Mock PRIVATE /W4)
else()
    target_compile_options(EngineModule.Mock PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(EngineModule.Mock PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
