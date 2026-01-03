/**
 * MetalCompute.mm - Metal Compute Implementation for Apple Silicon
 *
 * Objective-C++ implementation bridging Metal APIs with C++ physics.
 */

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <simd/simd.h>

#include "physics/MetalCompute.h"
#include <iostream>

namespace Physics {

/**
 * Internal Metal context holding Objective-C objects
 */
struct MetalContext {
  id<MTLDevice> device = nil;
  id<MTLCommandQueue> commandQueue = nil;
  id<MTLLibrary> library = nil;
  id<MTLComputePipelineState> integratePipeline = nil;
  id<MTLComputePipelineState> applyForcesPipeline = nil;
  id<MTLBuffer> bodiesBuffer = nil;
  id<MTLBuffer> paramsBuffer = nil;
};

MetalCompute::MetalCompute() : context_(std::make_unique<MetalContext>()) {}

MetalCompute::~MetalCompute() { shutdown(); }

bool MetalCompute::initialize() {
  @autoreleasepool {
    // Get default Metal device (Apple Silicon GPU)
    context_->device = MTLCreateSystemDefaultDevice();
    if (!context_->device) {
      std::cerr << "[MetalCompute] No Metal device available" << std::endl;
      return false;
    }

    std::cout << "[MetalCompute] Using device: " <<
        [context_->device.name UTF8String] << std::endl;

    // Create command queue
    context_->commandQueue = [context_->device newCommandQueue];
    if (!context_->commandQueue) {
      std::cerr << "[MetalCompute] Failed to create command queue" << std::endl;
      return false;
    }

    // Load Metal shader library
    NSError *error = nil;
    NSString *libraryPath = [[NSBundle mainBundle] pathForResource:@"physics"
                                                            ofType:@"metallib"];

    if (libraryPath) {
      NSURL *libraryURL = [NSURL fileURLWithPath:libraryPath];
      context_->library = [context_->device newLibraryWithURL:libraryURL
                                                        error:&error];
    }

    // Fallback: compile from source at runtime
    if (!context_->library) {
      NSString *shaderPath = @"assets/shaders/physics.metal";
      NSString *shaderSource =
          [NSString stringWithContentsOfFile:shaderPath
                                    encoding:NSUTF8StringEncoding
                                       error:&error];
      if (shaderSource) {
        MTLCompileOptions *options = [[MTLCompileOptions alloc] init];
        options.fastMathEnabled = YES;
        context_->library = [context_->device newLibraryWithSource:shaderSource
                                                           options:options
                                                             error:&error];
      }
    }

    if (!context_->library) {
      std::cerr << "[MetalCompute] Failed to load shader library" << std::endl;
      if (error) {
        std::cerr << "  Error: " << [[error localizedDescription] UTF8String]
                  << std::endl;
      }
      // Continue without GPU acceleration for now
      initialized_ = false;
      return true; // Still "initialize" but CPU fallback
    }

    // Create compute pipelines
    id<MTLFunction> integrateFunc =
        [context_->library newFunctionWithName:@"integrate_bodies"];
    if (integrateFunc) {
      context_->integratePipeline =
          [context_->device newComputePipelineStateWithFunction:integrateFunc
                                                          error:&error];
    }

    id<MTLFunction> forcesFunc =
        [context_->library newFunctionWithName:@"apply_forces"];
    if (forcesFunc) {
      context_->applyForcesPipeline =
          [context_->device newComputePipelineStateWithFunction:forcesFunc
                                                          error:&error];
    }

    // Create parameter buffer
    context_->paramsBuffer =
        [context_->device newBufferWithLength:sizeof(SimulationParams)
                                      options:MTLResourceStorageModeShared];

    initialized_ = (context_->integratePipeline != nil);
    std::cout << "[MetalCompute] Initialized: "
              << (initialized_ ? "GPU" : "CPU fallback") << std::endl;

    return true;
  }
}

void MetalCompute::shutdown() {
  @autoreleasepool {
    context_->bodiesBuffer = nil;
    context_->paramsBuffer = nil;
    context_->integratePipeline = nil;
    context_->applyForcesPipeline = nil;
    context_->library = nil;
    context_->commandQueue = nil;
    context_->device = nil;
    initialized_ = false;
  }
}

bool MetalCompute::isAvailable() const {
  return initialized_ && context_->device != nil;
}

void MetalCompute::setRigidBodies(const std::vector<RigidBody> &bodies) {
  bodies_ = bodies;

  @autoreleasepool {
    if (context_->device && !bodies.empty()) {
      size_t bufferSize = bodies.size() * sizeof(RigidBody);
      context_->bodiesBuffer =
          [context_->device newBufferWithBytes:bodies.data()
                                        length:bufferSize
                                       options:MTLResourceStorageModeShared];
    }
  }
}

void MetalCompute::getRigidBodies(std::vector<RigidBody> &bodies) {
  if (context_->bodiesBuffer && !bodies_.empty()) {
    @autoreleasepool {
      RigidBody *ptr = (RigidBody *)[context_->bodiesBuffer contents];
      bodies.assign(ptr, ptr + bodies_.size());
    }
  } else {
    bodies = bodies_;
  }
}

uint32_t MetalCompute::getBodyCount() const {
  return static_cast<uint32_t>(bodies_.size());
}

void MetalCompute::stepSimulation(float deltaTime) {
  if (bodies_.empty())
    return;

  if (!initialized_ || !context_->integratePipeline) {
    // CPU fallback: simple Euler integration
    for (auto &body : bodies_) {
      if (body.invMass > 0.0f) {
        // Apply gravity
        body.force.y -= gravity_ * body.mass;

        // Integration
        simd_float3 accel = body.force * body.invMass;
        body.velocity += accel * deltaTime;
        body.position += body.velocity * deltaTime;

        // Clear forces
        body.force = simd_make_float3(0, 0, 0);
        body.torque = simd_make_float3(0, 0, 0);
      }
    }
    return;
  }

  @autoreleasepool {
    // Update parameters
    SimulationParams params = {.deltaTime = deltaTime,
                               .gravity = gravity_,
                               .bodyCount =
                                   static_cast<uint32_t>(bodies_.size()),
                               .padding = 0};
    memcpy([context_->paramsBuffer contents], &params, sizeof(params));

    // Create command buffer
    id<MTLCommandBuffer> commandBuffer = [context_->commandQueue commandBuffer];
    id<MTLComputeCommandEncoder> encoder =
        [commandBuffer computeCommandEncoder];

    // Apply forces kernel
    if (context_->applyForcesPipeline) {
      [encoder setComputePipelineState:context_->applyForcesPipeline];
      [encoder setBuffer:context_->bodiesBuffer offset:0 atIndex:0];
      [encoder setBuffer:context_->paramsBuffer offset:0 atIndex:1];

      MTLSize gridSize = MTLSizeMake(bodies_.size(), 1, 1);
      NSUInteger threadGroupSize =
          MIN(context_->applyForcesPipeline.maxTotalThreadsPerThreadgroup,
              bodies_.size());
      MTLSize threadgroupSize = MTLSizeMake(threadGroupSize, 1, 1);

      [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
    }

    // Integration kernel
    [encoder setComputePipelineState:context_->integratePipeline];
    [encoder setBuffer:context_->bodiesBuffer offset:0 atIndex:0];
    [encoder setBuffer:context_->paramsBuffer offset:0 atIndex:1];

    MTLSize gridSize = MTLSizeMake(bodies_.size(), 1, 1);
    NSUInteger threadGroupSize =
        MIN(context_->integratePipeline.maxTotalThreadsPerThreadgroup,
            bodies_.size());
    MTLSize threadgroupSize = MTLSizeMake(threadGroupSize, 1, 1);

    [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
    [encoder endEncoding];

    // Execute and wait
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

    // Sync back to CPU (for reading positions, etc.)
    RigidBody *ptr = (RigidBody *)[context_->bodiesBuffer contents];
    bodies_.assign(ptr, ptr + bodies_.size());
  }
}

void MetalCompute::applyForce(uint32_t bodyIndex, simd_float3 force) {
  if (bodyIndex < bodies_.size()) {
    bodies_[bodyIndex].force += force;
  }
}

void MetalCompute::applyTorque(uint32_t bodyIndex, simd_float3 torque) {
  if (bodyIndex < bodies_.size()) {
    bodies_[bodyIndex].torque += torque;
  }
}

void MetalCompute::setGravity(float g) { gravity_ = g; }

} // namespace Physics
