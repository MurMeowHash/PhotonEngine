#include "../Public/GlfwWindowProvider.h"
#include "PlatformInteractor.h"
#include "GLFW/glfw3.h"
#include "WindowCreateInfo.h"

namespace Photon::Window::Glfw {
    bool m_isInitialized;
    GLFWwindow* m_mainWindow;

    bool CreateMainWindow(const WindowCreateInfo& windowCreateInfo) {
        if(!m_isInitialized) {
            if(glfwInit() == GLFW_TRUE)
                m_isInitialized = true;
            else
                return false;
        }

        if(m_mainWindow != nullptr)
            return false;

        GLFWmonitor *requestedMonitor = nullptr;
        if(windowCreateInfo.m_fullScreen)
            requestedMonitor = glfwGetPrimaryMonitor();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_mainWindow = glfwCreateWindow(windowCreateInfo.m_width,
                                        windowCreateInfo.m_height,
                                        windowCreateInfo.m_title,
                                        requestedMonitor,
                                        nullptr);

        return m_mainWindow != nullptr;
    }

    void Tick() {
        glfwPollEvents();

        if(glfwWindowShouldClose(m_mainWindow))
            Photon::Core::PlatformInteractor::RequestExit(false);
    }

    void Shutdown() {
        glfwDestroyWindow(m_mainWindow);
        glfwTerminate();

        m_isInitialized = false;
        m_mainWindow = nullptr;
    }
}
