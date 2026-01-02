/**
 * VulkanPipeline.h - Graphics pipeline management
 */

#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H



#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanPipeline {
public:
  VulkanPipeline() = default;
  ~VulkanPipeline() = default;

  bool create(VkDevice device, VkRenderPass renderPass, VkExtent2D extent);
  void destroy(VkDevice device);

  [[nodiscard]] VkPipeline getPipeline() const { return pipeline; }
  [[nodiscard]] VkPipelineLayout getLayout() const { return layout; }

private:
  VkPipeline pipeline{VK_NULL_HANDLE};
  VkPipelineLayout layout{VK_NULL_HANDLE};

  static std::vector<char> readShaderFile(const std::string &filename);
  VkShaderModule createShaderModule(VkDevice device,
                                    const std::vector<char> &code);
};

#endif // VULKAN_PIPELINE_H
