#pragma once

#include "WindowClass.h"
#include "WindowClassDescriptor.h"
#include "Blueprints/BlueprintProvider.h"

class WindowClassProvider : public BlueprintProvider<WindowClass, WindowClassDescriptor> {
protected:
    WindowClass* CreateBlueprint(const WindowClassDescriptor &desc, void *allocatedMemory) override;
};