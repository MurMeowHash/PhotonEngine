add_library(CoreModule STATIC)

target_sources(CoreModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CoreGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/PlatformInteractor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/RuntimeAttributes.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CoreUtils.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/EngineDataConfiguration.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/BuildConfiguration.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Blueprints/BlueprintDescriptor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Blueprints/BlueprintProvider.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/PhotonGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pools/PhotonPool.h
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CoreGlobals.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/PlatformInteractor.cpp
)

target_include_directories(CoreModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(CoreModule
        PUBLIC
        MemoryModule
        FactoryModule
        INTERFACE
        PhotonBuildConfig
)

if(MSVC)
    target_compile_options(CoreModule PRIVATE /W4)
else()
    target_compile_options(CoreModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(CoreModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
