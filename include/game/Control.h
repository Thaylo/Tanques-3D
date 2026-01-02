/**
 * Control.h - Input control state
 *
 * C++23 modernization: designated initializers, aggregate initialization
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "core/Vector.h"

/**
 * Structure holding the current state of all input controls.
 * Uses C++20 designated initializers for clean default initialization.
 */
struct Control {
  // Arrow key states
  bool arrowUp{false};
  bool arrowDown{false};
  bool arrowLeft{false};
  bool arrowRight{false};

  // Turbine power controls
  bool power_inc{false};
  bool power_dec{false};

  // Mouse button states
  bool rightPressed{false};
  bool newRightPressed{false};
  bool leftPressed{false};
  bool newLeftPressed{false};
  bool scrollPressed{false};

  // Action keys
  bool space{false};
  bool keyEsc{false};

  // Mouse positions
  Vector pressedPos{};
  Vector releasedPos{};

  // C++20: Default comparison
  [[nodiscard]] constexpr bool
  operator==(const Control &) const noexcept = default;
};

/**
 * Creates and initializes a Control structure with all values set to
 * false/zero.
 */
[[nodiscard]] inline constexpr Control initializeControl() noexcept {
  return Control{};
}

#endif // CONTROL_H
