add_library(WindowModule.Engine STATIC)

target_sources(WindowModule.Engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/GameWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Public/GameWindowProcessor.h
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/GameWindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Private/GameWindowProcessor.cpp
)

target_include_directories(WindowModule.Engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(WindowModule.Engine
        PRIVATE
        WindowModule.Core
        CoreModule
        RenderModule
)

if(MSVC)
    target_compile_options(WindowModule.Engine PRIVATE /W4)
else()
    target_compile_options(WindowModule.Engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(WindowModule.Engine PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
