add_library(ModuleBaseModule.Engine INTERFACE)

target_sources(ModuleBaseModule.Engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/EngineModuleGlobals.h
)

target_include_directories(ModuleBaseModule.Engine
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

target_link_libraries(ModuleBaseModule.Engine
        INTERFACE
        ModuleBaseModule.Core
        WindowModule.Core
        DebugModule
        VulkanRHLModule
)

if(MSVC)
    target_compile_options(ModuleBaseModule.Engine INTERFACE /W4)
else()
    target_compile_options(ModuleBaseModule.Engine INTERFACE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(ModuleBaseModule.Engine PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
