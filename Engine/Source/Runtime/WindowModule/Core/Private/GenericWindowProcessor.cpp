#include "../Public/GenericWindowProcessor.h"
#include "Window.h"

void GenericWindowProcessor::SetWindow(Window *window) {
    m_window = window;
}

void GenericWindowProcessor::ProcessWindowCloseRequest() {

}

void GenericWindowProcessor::ProcessWindowResize(uint32_t newWidth, uint32_t newHeight) {
    m_window->UpdateViewports();
}