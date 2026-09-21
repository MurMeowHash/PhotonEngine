add_library(RenderModule STATIC)

target_sources(RenderModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IRenderer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/ForwardRenderer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/RendererGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/RendererFactory.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/ForwardRenderer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/RendererFactory.cpp
)

target_include_directories(RenderModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(RenderModule
        PRIVATE
        CoreModule
)

if(MSVC)
    target_compile_options(RenderModule PRIVATE /W4)
else()
    target_compile_options(RenderModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(RenderModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
