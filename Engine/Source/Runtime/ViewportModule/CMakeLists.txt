add_library(ViewportModule STATIC)

target_sources(ViewportModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Viewport.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ViewportInteractor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ForwardViewport.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Viewport.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/ViewportInteractor.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/ForwardViewport.cpp
)

target_include_directories(ViewportModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(ViewportModule
        PRIVATE
        RenderModule
        FactoryModule
        CoreModule
        VulkanRHLModule
        ModuleBaseModule.Core
)

if(MSVC)
    target_compile_options(ViewportModule PRIVATE /W4)
else()
    target_compile_options(ViewportModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(ViewportModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
