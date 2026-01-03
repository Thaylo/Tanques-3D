/**
 * MetalCompute.mm - Metal Compute Implementation for Apple Silicon
 *
 * GPU-accelerated physics - NO CPU FALLBACKS.
 * Throws exceptions on failure.
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
  NSString *deviceName = nil;
};

MetalCompute::MetalCompute() : context_(std::make_unique<MetalContext>()) {}

MetalCompute::~MetalCompute() { shutdown(); }

void MetalCompute::initialize() {
  @autoreleasepool {
    // Get default Metal device (Apple Silicon GPU)
    context_->device = MTLCreateSystemDefaultDevice();
    if (!context_->device) {
      throw MetalComputeError(
          "No Metal device available - Apple Silicon required");
    }

    context_->deviceName = [context_->device.name copy]; // Retain the string
    std::cout << "[MetalCompute] Device: " << [context_->deviceName UTF8String]
              << std::endl;

    // Verify this is Apple Silicon (not Intel integrated)
    if (![context_->device.name containsString:@"Apple"]) {
      throw MetalComputeError("Apple Silicon GPU required, found: " +
                              std::string([context_->device.name UTF8String]));
    }

    // Create command queue
    context_->commandQueue = [context_->device newCommandQueue];
    if (!context_->commandQueue) {
      throw MetalComputeError("Failed to create Metal command queue");
    }

    // Load Metal shader library - compile from source
    NSError *error = nil;
    NSString *shaderPath = @"assets/shaders/physics.metal";
    NSString *shaderSource =
        [NSString stringWithContentsOfFile:shaderPath
                                  encoding:NSUTF8StringEncoding
                                     error:&error];

    if (!shaderSource) {
      throw MetalComputeError("Failed to load physics.metal shader file");
    }

    MTLCompileOptions *options = [[MTLCompileOptions alloc] init];
    options.mathMode = MTLMathModeFast;
    context_->library = [context_->device newLibraryWithSource:shaderSource
                                                       options:options
                                                         error:&error];

    if (!context_->library) {
      std::string errMsg = "Failed to compile Metal shaders";
      if (error) {
        errMsg += ": " + std::string([[error localizedDescription] UTF8String]);
      }
      throw MetalComputeError(errMsg);
    }

    // Create integrate_bodies pipeline
    id<MTLFunction> integrateFunc =
        [context_->library newFunctionWithName:@"integrate_bodies"];
    if (!integrateFunc) {
      throw MetalComputeError("Metal shader 'integrate_bodies' not found");
    }
    context_->integratePipeline =
        [context_->device newComputePipelineStateWithFunction:integrateFunc
                                                        error:&error];
    if (!context_->integratePipeline) {
      throw MetalComputeError("Failed to create integrate_bodies pipeline");
    }

    // Create apply_forces pipeline
    id<MTLFunction> forcesFunc =
        [context_->library newFunctionWithName:@"apply_forces"];
    if (!forcesFunc) {
      throw MetalComputeError("Metal shader 'apply_forces' not found");
    }
    context_->applyForcesPipeline =
        [context_->device newComputePipelineStateWithFunction:forcesFunc
                                                        error:&error];
    if (!context_->applyForcesPipeline) {
      throw MetalComputeError("Failed to create apply_forces pipeline");
    }

    // Create parameter buffer
    context_->paramsBuffer =
        [context_->device newBufferWithLength:sizeof(SimulationParams)
                                      options:MTLResourceStorageModeShared];
    if (!context_->paramsBuffer) {
      throw MetalComputeError("Failed to create simulation params buffer");
    }

    std::cout << "[MetalCompute] Initialized successfully with GPU acceleration"
              << std::endl;
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
  }
}

void MetalCompute::setRigidBodies(const std::vector<RigidBody> &bodies) {
  bodies_ = bodies;

  @autoreleasepool {
    if (!bodies.empty()) {
      size_t bufferSize = bodies.size() * sizeof(RigidBody);
      context_->bodiesBuffer =
          [context_->device newBufferWithBytes:bodies.data()
                                        length:bufferSize
                                       options:MTLResourceStorageModeShared];
      if (!context_->bodiesBuffer) {
        throw MetalComputeError("Failed to create rigid bodies buffer");
      }
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
    [encoder setComputePipelineState:context_->applyForcesPipeline];
    [encoder setBuffer:context_->bodiesBuffer offset:0 atIndex:0];
    [encoder setBuffer:context_->paramsBuffer offset:0 atIndex:1];

    MTLSize gridSize = MTLSizeMake(bodies_.size(), 1, 1);
    NSUInteger threadGroupSize =
        MIN(context_->applyForcesPipeline.maxTotalThreadsPerThreadgroup,
            bodies_.size());
    MTLSize threadgroupSize = MTLSizeMake(threadGroupSize, 1, 1);

    [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];

    // Integration kernel
    [encoder setComputePipelineState:context_->integratePipeline];
    [encoder setBuffer:context_->bodiesBuffer offset:0 atIndex:0];
    [encoder setBuffer:context_->paramsBuffer offset:0 atIndex:1];

    threadGroupSize =
        MIN(context_->integratePipeline.maxTotalThreadsPerThreadgroup,
            bodies_.size());
    threadgroupSize = MTLSizeMake(threadGroupSize, 1, 1);

    [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
    [encoder endEncoding];

    // Execute and wait
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

    // Sync back to CPU (unified memory - zero copy on Apple Silicon)
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

std::string MetalCompute::getDeviceName() const {
  if (context_->deviceName) {
    return std::string([context_->deviceName UTF8String]);
  }
  return "Unknown";
}

} // namespace Physics
