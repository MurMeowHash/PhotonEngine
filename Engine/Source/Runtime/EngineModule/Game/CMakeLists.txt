add_library(EngineModule.Game STATIC)

target_sources(EngineModule.Game
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/GameEngine.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/GameEngineModule.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/GameEngineFactory.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/GameEngine.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/GameEngineModule.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/GameEngineFactory.cpp
)

target_include_directories(EngineModule.Game
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(EngineModule.Game
        PRIVATE
        EngineModule.Core
        ModuleBaseModule.Core
        WindowModule.Core
        WindowModule.Engine
        ViewportModule
)

if(MSVC)
    target_compile_options(EngineModule.Game PRIVATE /W4)
else()
    target_compile_options(EngineModule.Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(EngineModule.Game PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
