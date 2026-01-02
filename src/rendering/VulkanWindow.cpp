/**
 * VulkanWindow.cpp - SDL2/Vulkan window and input implementation
 */



#include "rendering/VulkanWindow.h"
#include "core/Constants.h"

bool VulkanWindow::processEvents(Control &control, bool &running) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running = false;
      return false;

    case SDL_KEYDOWN:
      handleKeyDown(event.key.keysym.sym, control);
      break;

    case SDL_KEYUP:
      handleKeyUp(event.key.keysym.sym, control);
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        control.leftPressed = true;
        control.newLeftPressed = true;
      } else if (event.button.button == SDL_BUTTON_RIGHT) {
        control.rightPressed = true;
        control.newRightPressed = true;
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        control.leftPressed = false;
      } else if (event.button.button == SDL_BUTTON_RIGHT) {
        control.rightPressed = false;
      }
      break;
    }
  }

  return true;
}

void VulkanWindow::handleKeyDown(SDL_Keycode key, Control &control) {
  switch (key) {
  case SDLK_UP:
    control.arrowUp = true;
    break;
  case SDLK_DOWN:
    control.arrowDown = true;
    break;
  case SDLK_LEFT:
    control.arrowLeft = true;
    break;
  case SDLK_RIGHT:
    control.arrowRight = true;
    break;
  case SDLK_SPACE:
    control.space = true;
    break;
  case SDLK_ESCAPE:
    control.keyEsc = true;
    break;
  case SDLK_w:
    control.power_inc = true;
    break;
  case SDLK_s:
    control.power_dec = true;
    break;
  default:
    break;
  }
}

void VulkanWindow::handleKeyUp(SDL_Keycode key, Control &control) {
  switch (key) {
  case SDLK_UP:
    control.arrowUp = false;
    break;
  case SDLK_DOWN:
    control.arrowDown = false;
    break;
  case SDLK_LEFT:
    control.arrowLeft = false;
    break;
  case SDLK_RIGHT:
    control.arrowRight = false;
    break;
  case SDLK_SPACE:
    control.space = false;
    break;
  case SDLK_ESCAPE:
    control.keyEsc = false;
    break;
  case SDLK_w:
    control.power_inc = false;
    break;
  case SDLK_s:
    control.power_dec = false;
    break;
  default:
    break;
  }
}

