add_executable(LaunchModule
        Private/Launcher.cpp
)

target_link_libraries(LaunchModule
        PRIVATE
        EngineLoopModule.Core
        EngineLoopModule.Engine
        CoreModule
)

target_include_directories(LaunchModule
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

if(MSVC)
    target_compile_options(LaunchModule PRIVATE /W4)
else()
    target_compile_options(LaunchModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(LaunchModule PROPERTIES
        OUTPUT_NAME PhotonEngine
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

if(MINGW)
    get_filename_component(PHOTON_MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(PHOTON_MINGW_RUNTIME_DLLS
            "${PHOTON_MINGW_BIN_DIR}/libstdc++-6.dll"
            "${PHOTON_MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${PHOTON_MINGW_BIN_DIR}/libwinpthread-1.dll"
    )

    foreach(_runtime_dll IN LISTS PHOTON_MINGW_RUNTIME_DLLS)
        if(NOT EXISTS "${_runtime_dll}")
            message(FATAL_ERROR "MinGW runtime library not found: ${_runtime_dll}")
        endif()
    endforeach()

    add_custom_command(TARGET LaunchModule POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PHOTON_MINGW_RUNTIME_DLLS}
                $<TARGET_FILE_DIR:LaunchModule>
    )
endif()

if(WIN32)
    add_custom_command(TARGET LaunchModule POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:LaunchModule>
                $<TARGET_FILE_DIR:LaunchModule>
            COMMAND_EXPAND_LISTS
    )
endif()