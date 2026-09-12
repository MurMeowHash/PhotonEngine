add_library(WindowProviderModule.Glfw STATIC)

target_sources(WindowProviderModule.Glfw
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/GlfwWindowProvider.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/GlfwWindowProvider.cpp
)

target_include_directories(WindowProviderModule.Glfw
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(WindowProviderModule.Glfw
        PRIVATE
            glfw3
        WindowProviderModule.Core
            CoreModule
)

if(MSVC)
    target_compile_options(WindowProviderModule.Glfw PRIVATE /W4)
else()
    target_compile_options(WindowProviderModule.Glfw PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(WindowProviderModule.Glfw PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
