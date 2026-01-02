/**
 * VulkanRenderer.h - Core Vulkan rendering infrastructure
 *
 * Manages Vulkan instance, device, swapchain, and command buffers.
 * Uses MoltenVK on macOS for Metal translation.
 */

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "core/Vector.h"

// Vertex structure for Vulkan
struct Vertex {
  float pos[3];
  float color[3];
  float texCoord[2];

  static VkVertexInputBindingDescription getBindingDescription();
  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions();
};

// Queue family indices
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

// Swapchain support details
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

/**
 * Core Vulkan renderer managing GPU resources.
 */
class VulkanRenderer {
public:
  VulkanRenderer();
  ~VulkanRenderer();

  // Disable copy
  VulkanRenderer(const VulkanRenderer &) = delete;
  VulkanRenderer &operator=(const VulkanRenderer &) = delete;

  // Initialization
  bool initialize(const char *appName, int width, int height);
  void cleanup();

  // Frame rendering
  void beginFrame();
  void endFrame();

  // Drawing primitives
  void drawTriangle(const Vector &p1, const Vector &p2, const Vector &p3,
                    float r, float g, float b);
  void drawQuad(const Vector &p1, const Vector &p2, const Vector &p3,
                const Vector &p4, float r, float g, float b);
  void drawLine(const Vector &start, const Vector &end, float r, float g,
                float b);

  // Camera/view matrix
  void setViewMatrix(const float *matrix);
  void setProjectionMatrix(const float *matrix);
  void setMVPMatrix(const float *matrix);

  // Window
  [[nodiscard]] SDL_Window *getWindow() const { return window; }
  [[nodiscard]] bool isRunning() const { return running; }
  void setRunning(bool r) { running = r; }

  // Events
  void pollEvents();

private:
  // Vulkan objects
  VkInstance instance{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
  VkSurfaceKHR surface{VK_NULL_HANDLE};
  VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
  VkDevice device{VK_NULL_HANDLE};
  VkQueue graphicsQueue{VK_NULL_HANDLE};
  VkQueue presentQueue{VK_NULL_HANDLE};
  VkSwapchainKHR swapChain{VK_NULL_HANDLE};
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass{VK_NULL_HANDLE};
  VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
  VkPipeline graphicsPipeline{VK_NULL_HANDLE};
  VkCommandPool commandPool{VK_NULL_HANDLE};
  std::vector<VkCommandBuffer> commandBuffers;

  // Depth buffer for 3D rendering
  VkImage depthImage{VK_NULL_HANDLE};
  VkDeviceMemory depthImageMemory{VK_NULL_HANDLE};
  VkImageView depthImageView{VK_NULL_HANDLE};
  VkFormat depthFormat{VK_FORMAT_D32_SFLOAT};

  // Synchronization
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame{0};
  uint32_t currentImageIndex{0};
  static constexpr int MAX_FRAMES_IN_FLIGHT = 1;

  // Vertex buffer for dynamic geometry
  VkBuffer vertexBuffer{VK_NULL_HANDLE};
  VkDeviceMemory vertexBufferMemory{VK_NULL_HANDLE};
  std::vector<Vertex> vertices;

  // SDL Window
  SDL_Window *window{nullptr};
  bool running{true};
  int windowWidth{800};
  int windowHeight{600};

  // MVP matrix for 3D camera
  float mvpMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  // Initialization helpers
  bool createInstance(const char *appName);
  bool setupDebugMessenger();
  bool createSurface();
  bool pickPhysicalDevice();
  bool createLogicalDevice();
  bool createSwapChain();
  bool createImageViews();
  bool createRenderPass();
  bool createGraphicsPipeline();
  bool createFramebuffers();
  bool createCommandPool();
  bool createCommandBuffers();
  bool createSyncObjects();
  bool createVertexBuffer();
  bool createDepthResources();
  void createImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory);
  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags);

  // Utility functions
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR
  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
  VkPresentModeKHR
  chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &modes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  VkShaderModule createShaderModule(const std::vector<char> &code);
  std::vector<char> readFile(const std::string &filename);
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  void updateVertexBuffer();

  // Validation layers
  static constexpr bool enableValidationLayers =
#ifdef NDEBUG
      false;
#else
      true;
#endif

  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  bool checkValidationLayerSupport();
  std::vector<const char *> getRequiredExtensions();
};

#endif // VULKAN_RENDERER_H
