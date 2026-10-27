#include "../../Public/Shaders/VulkanShaderReflectModule.h"
#include <spirv_reflect.h>
#include <algorithm>
#include <cctype>
#include <charconv>
#include <string>
#include <string_view>

#define VALIDATE_SPV_RESULT(reflectResult)              \
    if(reflectResult != SPV_REFLECT_RESULT_SUCCESS)     \
        return Photon::Result::UnknownFailure;


#define POSITION_SEMANTIC_NAME "POSITION"

VulkanShaderReflectModule::~VulkanShaderReflectModule() {
    spvReflectDestroyShaderModule(&m_handle);
}

VulkanShaderReflectModule* VulkanShaderReflectModule::Create(const VulkanShaderReflectModuleCreateInfo &createInfo,
                                                             InOutCreateParams<Photon::Result> *inOutCreateParams) {
    SpvReflectShaderModule reflectModule;
    SpvReflectResult reflectResult =
        spvReflectCreateShaderModule2(SPV_REFLECT_MODULE_FLAG_NO_COPY, createInfo.m_spirV.GetCodeSize(), createInfo.m_spirV.GetCode(), &reflectModule);
    if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanShaderReflectModule* instance = Photon::AllocateObject<VulkanShaderReflectModule>(inOutCreateParams);
    instance->m_handle = reflectModule;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

Photon::Result VulkanShaderReflectModule::GenerateShaderMetaData(VulkanShaderMetaData &outMetaData) const {
    return ReflectInputVariables(outMetaData);
}

Photon::Result VulkanShaderReflectModule::ReflectInputVariables(VulkanShaderMetaData &outMetaData) const {
    uint32_t inputVariablesCount{};
    VALIDATE_SPV_RESULT(spvReflectEnumerateInputVariables(&m_handle, &inputVariablesCount, nullptr))
    std::vector<SpvReflectInterfaceVariable*> inputVariables(inputVariablesCount);
    VALIDATE_SPV_RESULT(spvReflectEnumerateInputVariables(&m_handle, &inputVariablesCount, inputVariables.data()))

    outMetaData.m_vertexAttributes.reserve(inputVariablesCount);
    for (SpvReflectInterfaceVariable* inputVariable : inputVariables) {
        if ((inputVariable->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) == SPV_REFLECT_DECORATION_BUILT_IN)
            continue;

        outMetaData.m_vertexAttributes.emplace_back(TranslateSemanticToRHL(inputVariable->semantic),
            inputVariable->location, TranslateReflectFormatToRHL(inputVariable->format));
    }

    return Photon::Result::Success;
}

VulkanRHLShaderSemantic VulkanShaderReflectModule::TranslateSemanticToRHL(const char *semantic) {
    VulkanRHLShaderSemantic rhlSemantic{VulkanRHLShaderSemanticType::Undefined, 0};
    if (semantic == nullptr)
        return rhlSemantic;

    std::string_view semanticText(semantic);
    size_t indexBegin = semanticText.size();
    while (indexBegin > 0 && std::isdigit(static_cast<unsigned char>(semanticText[indexBegin - 1])))
        --indexBegin;

    std::from_chars(semanticText.data() + indexBegin, semanticText.data() + semanticText.size(), rhlSemantic.m_semanticIndex);

    std::string semanticName(semanticText.substr(0, indexBegin));
    std::ranges::transform(semanticName, semanticName.begin(), [](unsigned char character) {
        return static_cast<char>(std::toupper(character));
    });

    if (semanticName == POSITION_SEMANTIC_NAME)
        rhlSemantic.m_semanticType = VulkanRHLShaderSemanticType::Position;

    return rhlSemantic;
}

VulkanRHLShaderFormat VulkanShaderReflectModule::TranslateReflectFormatToRHL(SpvReflectFormat spvFormat) {
    switch (spvFormat) {
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return VulkanRHLShaderFormat::R32G32_Float;
        default:
            return VulkanRHLShaderFormat::Undefined;
    }
}