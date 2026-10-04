add_library(MathModule STATIC)

target_sources(MathModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Range.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Math.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Range.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Math.cpp
)

target_include_directories(MathModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(MathModule
        PUBLIC
        glm
)

if(MSVC)
    target_compile_options(MathModule PRIVATE /W4)
else()
    target_compile_options(MathModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(MathModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
