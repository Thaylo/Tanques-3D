/**
 * MetalCompute.h - Metal Compute Infrastructure for Apple Silicon Physics
 *
 * GPU-accelerated physics using Metal compute shaders.
 * Apple Silicon ONLY - no fallbacks.
 */

#ifndef METAL_COMPUTE_H
#define METAL_COMPUTE_H

#include <cstdint>
#include <memory>
#include <simd/simd.h>
#include <stdexcept>
#include <vector>

namespace Physics {

// Forward declarations (Objective-C++ types)
struct MetalContext;

/**
 * Rigid body data structure - aligned for GPU
 * Uses SIMD types for Metal compatibility
 */
struct alignas(16) RigidBody {
  simd_float3 position;
  float mass;
  simd_float3 velocity;
  float invMass;
  simd_float3 angularVelocity;
  float padding1;
  simd_quatf orientation;
  simd_float3 force;
  float padding2;
  simd_float3 torque;
  float restitution; // Bounciness for collisions
};

/**
 * Physics simulation parameters
 */
struct SimulationParams {
  float deltaTime;
  float gravity;
  uint32_t bodyCount;
  uint32_t padding;
};

/**
 * MetalComputeError - Thrown when Metal operations fail
 */
class MetalComputeError : public std::runtime_error {
public:
  explicit MetalComputeError(const std::string &msg)
      : std::runtime_error(msg) {}
};

/**
 * MetalCompute - Metal-accelerated physics engine
 *
 * NO FALLBACKS - requires Apple Silicon GPU
 */
class MetalCompute {
public:
  MetalCompute();
  ~MetalCompute();

  // Initialize Metal device and pipelines - throws on failure
  void initialize();

  // Shutdown and release resources
  void shutdown();

  // Rigid body management
  void setRigidBodies(const std::vector<RigidBody> &bodies);
  void getRigidBodies(std::vector<RigidBody> &bodies);
  uint32_t getBodyCount() const;

  // Physics simulation step
  void stepSimulation(float deltaTime);

  // Apply forces
  void applyForce(uint32_t bodyIndex, simd_float3 force);
  void applyTorque(uint32_t bodyIndex, simd_float3 torque);
  void setGravity(float g);

  // Query GPU info
  std::string getDeviceName() const;

private:
  std::unique_ptr<MetalContext> context_;
  std::vector<RigidBody> bodies_;
  float gravity_ = 9.81f;
};

} // namespace Physics

#endif // METAL_COMPUTE_H
