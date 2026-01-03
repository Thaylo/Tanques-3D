/**
 * MetalCompute.h - Metal Compute Infrastructure for Apple Silicon Physics
 *
 * Provides GPU-accelerated physics using Metal compute shaders.
 * Designed for Apple M1/M2/M3 unified memory architecture.
 */

#ifndef METAL_COMPUTE_H
#define METAL_COMPUTE_H

#ifdef __APPLE__

#include <cstdint>
#include <memory>
#include <simd/simd.h>
#include <string>
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
  float padding3;
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
 * MetalCompute - Main interface for Metal-accelerated physics
 */
class MetalCompute {
public:
  MetalCompute();
  ~MetalCompute();

  // Initialize Metal device and pipelines
  bool initialize();

  // Shutdown and release resources
  void shutdown();

  // Check if Metal is available
  bool isAvailable() const;

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

private:
  std::unique_ptr<MetalContext> context_;
  std::vector<RigidBody> bodies_;
  float gravity_ = 9.81f;
  bool initialized_ = false;
};

} // namespace Physics

#endif // __APPLE__
#endif // METAL_COMPUTE_H
