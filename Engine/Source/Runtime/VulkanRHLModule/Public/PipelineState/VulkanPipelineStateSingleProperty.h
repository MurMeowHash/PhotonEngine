#pragma once

#include "Event.h"
#include "IVulkanPipelineStateProperty.h"

template<typename TProperty>
class VulkanPipelineStateSingleProperty : public IVulkanPipelineStateProperty {
public:
    void Reset() final override {
        m_isDirty = false;
    }

    void Invalidate() final override {
        m_isDirty = true;
    }

    Event<TProperty> m_onPropertyChanged;

    void SetProperty(TProperty property) {
        if (m_property != property) {
            m_property = property;
            m_isDirty = true;
            m_onPropertyChanged.Invoke(property);
        }
    }

    [[nodiscard]] TProperty GetProperty() const {
        return m_property;
    }

protected:
    TProperty m_property{};
    bool m_isDirty{};
};