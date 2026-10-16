#pragma once

#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include "CoreUtils.h"
#include <functional>

class VulkanPipelineStateBlueprintContainer;

template<typename TProperty>
class VulkanPipelinePropertyAssembler {
public:
    explicit VulkanPipelinePropertyAssembler(VulkanPipelineStateBlueprintContainer* container)
    : m_container(container) {}

    ~VulkanPipelinePropertyAssembler();

    template<typename... Args>
    void WithArguments();

private:
    VulkanPipelineStateBlueprintContainer* m_container;
    std::function<void(VulkanPipelineStateBlueprintContainer*)> m_injectFunc;
};

class VulkanPipelineStateBlueprintContainer {
public:
    template<typename TProperty>
    VulkanPipelinePropertyAssembler<TProperty> BindProperty() {
        return VulkanPipelinePropertyAssembler<TProperty>(this);
    }

    void AddPropertyInjectFunc(std::function<void(VulkanPipelineStateBlueprintContainer*)>&& propertyInjectFunc) {
        m_propertyInjectFunctions.emplace_back(std::move(propertyInjectFunc));
    }

    template<typename TProperty>
    void RegisterProperty(TProperty* property) {
        m_properties[std::type_index(typeid(TProperty))] = property;
    }

    template<typename TProperty>
    TProperty* ResolveProperty() {
        auto propertyIterator = m_properties.find(std::type_index(typeid(TProperty)));
        if (propertyIterator == m_properties.end())
            throw std::runtime_error(StringFormatter("Accessing unregistered property {", typeid(TProperty).name(), "} in VulkanPipelineStateBlueprintContainer"));

        return static_cast<TProperty*>(propertyIterator->second);
    }

    void ResolvePropertiesInjection() {
        for (std::function<void(VulkanPipelineStateBlueprintContainer *)>& propertyInjectFunc: m_propertyInjectFunctions) {
            propertyInjectFunc(this);
        }
    }
private:
    std::unordered_map<std::type_index, void*> m_properties;
    std::vector<std::function<void(VulkanPipelineStateBlueprintContainer*)>> m_propertyInjectFunctions;
};

template<typename TProperty>
VulkanPipelinePropertyAssembler<TProperty>::~VulkanPipelinePropertyAssembler() {
    m_container->AddPropertyInjectFunc(std::move(m_injectFunc));
}

template<typename TProperty>
template<typename ... Args>
void VulkanPipelinePropertyAssembler<TProperty>::WithArguments() {
    m_injectFunc = [](VulkanPipelineStateBlueprintContainer* container) {
        TProperty* property = container->ResolveProperty<TProperty>();
        property->InjectProperties(container->ResolveProperty<Args>()...);
    };
}

class VulkanPipelineStateBlueprint {
public:
    virtual ~VulkanPipelineStateBlueprint() = default;
    template<typename... Args>
    void Create(Args... properties) {
        (m_blueprintContainer.RegisterProperty(properties), ...);
        InstallProperties();
        m_blueprintContainer.ResolvePropertiesInjection();
    }

protected:
    VulkanPipelineStateBlueprintContainer m_blueprintContainer;

    virtual void InstallProperties() = 0;
};