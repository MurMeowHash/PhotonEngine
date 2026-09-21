add_library(EngineLoopModule.Engine STATIC)

target_sources(EngineLoopModule.Engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CoreLoop.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/EngineLoopGlobals.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CoreLoop.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/EngineLoopGlobals.cpp
)

target_include_directories(EngineLoopModule.Engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(EngineLoopModule.Engine
        PRIVATE
        EngineLoopModule.Core
        WindowProviderModule.Core
        WindowProviderModule.Glfw
        VulkanRHLModule
        RenderModule
)

if(MSVC)
    target_compile_options(EngineLoopModule.Engine PRIVATE /W4)
else()
    target_compile_options(EngineLoopModule.Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(EngineLoopModule.Engine PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
