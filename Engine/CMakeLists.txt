# Note: project() is declared in root CMakeLists.txt
# This file configures the Engine subdirectory

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# Third Party Dependencies
# ==========================================

# ---------- GLM (header-only) ----------
set(GLM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty/glm")
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${GLM_DIR}/include")

# ---------- Vulkan ----------
set(VULKAN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty/vulkan")
add_library(vulkan STATIC IMPORTED)

if(WIN32)
    set(VULKAN_LIB "${VULKAN_DIR}/lib/vulkan-1.lib")
elseif(APPLE)
    set(VULKAN_LIB "${VULKAN_DIR}/lib/libvulkan.dylib")
else()
    set(VULKAN_LIB "${VULKAN_DIR}/lib/libvulkan.so")
endif()

set_target_properties(vulkan PROPERTIES
        IMPORTED_LOCATION "${VULKAN_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_DIR}/include"
)

# ---------- DXC (DirectX Shader Compiler, bundled with Vulkan SDK) ----------
add_library(dxc INTERFACE)
target_include_directories(dxc INTERFACE "${VULKAN_DIR}/include/dxc")

if(WIN32)
    set(DXC_IMPLIB "${VULKAN_DIR}/lib/dxcompiler.lib")
    if(NOT EXISTS "${DXC_IMPLIB}")
        message(FATAL_ERROR "DXC import library not found at ${DXC_IMPLIB}")
    endif()

    add_library(dxcompiler SHARED IMPORTED GLOBAL)
    set(_dxc_imported_props
            IMPORTED_IMPLIB "${DXC_IMPLIB}"
    )
    if(EXISTS "${VULKAN_DIR}/lib/dxcompiler.dll")
        list(APPEND _dxc_imported_props
                IMPORTED_LOCATION "${VULKAN_DIR}/lib/dxcompiler.dll"
        )
    endif()
    set_target_properties(dxcompiler PROPERTIES ${_dxc_imported_props})

    target_link_libraries(dxc INTERFACE dxcompiler)
else()
    message(STATUS "DXC library linking is only configured for Windows")
endif()

# ==========================================
# Engine Runtime
# ==========================================

add_subdirectory(Source/Runtime)

