add_library(WindowModule.Core STATIC)

target_sources(WindowModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Window.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowClass.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowClassDescriptor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowClassProvider.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowModule.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IWindowProcessor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/GenericWindowProcessor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/WindowEventDispatcher.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Window.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/WindowClass.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/WindowClassProvider.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/WindowModule.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/GenericWindowProcessor.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/WindowEventDispatcher.cpp
)

target_include_directories(WindowModule.Core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(WindowModule.Core
        PUBLIC
        FactoryModule
        ModuleBaseModule.Core
        RenderModule
        PRIVATE
        CoreModule
        VulkanRHLModule
        MathModule
        ViewportModule
)

if(MSVC)
    target_compile_options(WindowModule.Core PRIVATE /W4)
else()
    target_compile_options(WindowModule.Core PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(WindowModule.Core PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
