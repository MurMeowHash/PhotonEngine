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

if(WIN32)
    add_custom_command(TARGET LaunchModule POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:LaunchModule>
                $<TARGET_FILE_DIR:LaunchModule>
            COMMAND_EXPAND_LISTS
    )
endif()