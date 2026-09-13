add_library(MemoryModule STATIC)

target_sources(MemoryModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/LinearAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MemoryChunk.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Adopters/IAllocatorAdopter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Adopters/LinearToLinearAllocatorAdopter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/IAllocatorProcessor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/AllocatorProcessor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/SimpleAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/AllocatorBase.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Adopters/SimpleToSimpleAllocatorAdopter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Adopters/LinearToSimpleAllocatorAdopter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Adopters/SimpleToLinearAllocatorAdopter.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MemoryOperationsCommon.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/MemoryRange.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/LinearAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Adopters/LinearToLinearAllocatorAdopter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/AllocatorProcessor.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/SimpleAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/AllocatorBase.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Adopters/SimpleToSimpleAllocatorAdopter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Adopters/LinearToSimpleAllocatorAdopter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Adopters/SimpleToLinearAllocatorAdopter.cpp
)

target_include_directories(MemoryModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(MemoryModule
        PUBLIC
        MathModule
)

if(MSVC)
    target_compile_options(MemoryModule PRIVATE /W4)
else()
    target_compile_options(MemoryModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(MemoryModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
