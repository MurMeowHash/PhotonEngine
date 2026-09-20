add_library(VulkanRHLModule STATIC)

target_sources(VulkanRHLModule
        PUBLIC
        ## Memory
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanMemoryCommon.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanDeviceMemory.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/IVulkanDeviceAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/DefaultVulkanDeviceAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanMemoryPage.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/IVulkanDedicatedDeviceAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/DefaultVulkanDedicatedDeviceAllocator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/IVulkanDeviceMemoryProvider.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanDeviceMemoryProvider.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanResourceTilingType.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanDeviceAllocatorBundle.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanMemoryProviderType.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/IVulkanDeviceMemoryProviderFactory.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanDeviceMemoryProviderFactory.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/VulkanMemoryHeap.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/Configurations/VulkanAllocationPreset.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Memory/Configurations/VulkanAllocationConfiguration.h

        ## Entity
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Entity/VulkanExtension.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Entity/VulkanEntity.h

        ## Instance
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Instance/VulkanInstance.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Instance/VulkanDebugger.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Instance/VulkanInstanceFactory.h

        ## Device
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Device/VulkanDevice.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Device/VulkanDeviceFeature.h

        ## Globals
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/VulkanGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/VulkanUtils.h

        ## Configurations
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Configurations/VulkanExtensionsConfiguration.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Configurations/VulkanInstanceConfiguration.h

        ## Command Buffers
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CommandBuffers/VulkanCommandBuffer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CommandBuffers/VulkanCommandBufferPool.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CommandBuffers/VulkanCommandBufferGlobals.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CommandBuffers/VulkanCommandBufferFactory.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/CommandBuffers/VulkanCommandBufferPoolFactory.h

        ## Queue
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Queue/VulkanQueue.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Queue/VulkanQueueFactory.h

        ## Shaders
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Shaders/VulkanShaderModule.h

        ## Pipelines
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanPipeline.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanPipelineProvider.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanComputePipeline.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanComputePipelineDescriptor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanGraphicsPipeline.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanGraphicsPipelineDescriptor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Public/Pipelines/VulkanPipelineFactory.h

        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanMemoryCommon.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanDeviceMemory.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/DefaultVulkanDeviceAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanMemoryPage.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/DefaultVulkanDedicatedDeviceAllocator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanDeviceMemoryProvider.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanDeviceAllocatorBundle.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanDeviceMemoryProviderFactory.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Memory/VulkanMemoryHeap.cpp

        ## Entity
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Entity/VulkanExtension.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Entity/VulkanEntity.cpp

        ## Instance
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Instance/VulkanInstance.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Instance/VulkanDebugger.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Instance/VulkanInstanceFactory.cpp

        ## Device
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Device/VulkanDevice.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Device/VulkanDeviceFeature.cpp

        ## Command Buffers
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CommandBuffers/VulkanCommandBuffer.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CommandBuffers/VulkanCommandBufferPool.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CommandBuffers/VulkanCommandBufferFactory.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/CommandBuffers/VulkanCommandBufferPoolFactory.cpp

        ## Queue
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Queue/VulkanQueue.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Queue/VulkanQueueFactory.cpp

        ## Shaders
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Shaders/VulkanShaderModule.cpp

        ## Pipelines
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Pipelines/VulkanPipeline.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Pipelines/VulkanPipelineProvider.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Pipelines/VulkanComputePipeline.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Pipelines/VulkanGraphicsPipeline.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Private/Pipelines/VulkanPipelineFactory.cpp
)

target_include_directories(VulkanRHLModule
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(VulkanRHLModule
        PUBLIC
        vulkan
        CoreModule
        MemoryModule
        PRIVATE
        DebugModule
)

if(MSVC)
    target_compile_options(VulkanRHLModule PRIVATE /W4)
else()
    target_compile_options(VulkanRHLModule PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(VulkanRHLModule PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
