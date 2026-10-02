#include "../Public/WindowClassProvider.h"

WindowClass* WindowClassProvider::CreateBlueprint(const WindowClassDescriptor &desc, void *allocatedMemory) {
    WindowClassCreateInfo createInfo;
    createInfo.m_desc = desc;
    InOutCreateParams<Photon::Result> createParams;
    createParams.m_preAllocatedMemory = allocatedMemory;
    return WindowClass::Create(createInfo, &createParams);
}