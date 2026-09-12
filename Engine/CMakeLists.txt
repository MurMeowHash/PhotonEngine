# Note: project() is declared in root CMakeLists.txt
# This file configures the Engine subdirectory

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# Third Party Dependencies
# ==========================================

# ---------- GLFW ----------
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/ThirdParty/glfw3")
add_library(glfw3 STATIC IMPORTED)

if(WIN32)
    if(EXISTS "${GLFW_DIR}/lib/glfw3.lib")
        set(GLFW_LIB_FILE "${GLFW_DIR}/lib/glfw3.lib")
    elseif(EXISTS "${GLFW_DIR}/lib/libglfw3.a")
        set(GLFW_LIB_FILE "${GLFW_DIR}/lib/libglfw3.a")
    elseif(EXISTS "${GLFW_DIR}/lib/libglfw3dll.a")
        set(GLFW_LIB_FILE "${GLFW_DIR}/lib/libglfw3dll.a")
    else()
        message(FATAL_ERROR "GLFW library not found")
    endif()

    set_target_properties(glfw3 PROPERTIES
            IMPORTED_LOCATION "${GLFW_LIB_FILE}"
            INTERFACE_INCLUDE_DIRECTORIES "${GLFW_DIR}/include"
    )

elseif(APPLE)
    message(FATAL_ERROR "GLFW setup for macOS not implemented yet")
else()
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)

    set_target_properties(glfw3 PROPERTIES
            IMPORTED_LOCATION "${GLFW_DIR}/lib/libglfw3.a"
            INTERFACE_INCLUDE_DIRECTORIES "${GLFW_DIR}/include"
    )
endif()

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

# ==========================================
# Engine Runtime
# ==========================================

add_subdirectory(Source/Runtime)

