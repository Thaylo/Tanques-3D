/**
 * VulkanWindow.h - SDL2/Vulkan window management
 */

#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H



#include "game/Control.h"
#include <SDL2/SDL.h>

/**
 * Handles SDL2 window events and input mapping.
 */
class VulkanWindow {
public:
  VulkanWindow() = default;
  ~VulkanWindow() = default;

  // Process SDL events and update control state
  static bool processEvents(Control &control, bool &running);

  // Map SDL key to control action
  static void handleKeyDown(SDL_Keycode key, Control &control);
  static void handleKeyUp(SDL_Keycode key, Control &control);
};

#endif // VULKAN_WINDOW_H
