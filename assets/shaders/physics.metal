/**
 * physics.metal - Metal Compute Shaders for Physics Simulation
 *
 * GPU-accelerated physics kernels for Apple Silicon.
 * Uses unified memory for zero-copy CPU/GPU data sharing.
 */

#include <metal_stdlib>
using namespace metal;

/**
 * Rigid body structure - must match C++ RigidBody
 */
struct RigidBody {
    float3 position;
    float mass;
    float3 velocity;
    float invMass;
    float3 angularVelocity;
    float padding1;
    float4 orientation;  // quaternion (x, y, z, w)
    float3 force;
    float padding2;
    float3 torque;
    float padding3;
};

/**
 * Simulation parameters
 */
struct SimulationParams {
    float deltaTime;
    float gravity;
    uint bodyCount;
    uint padding;
};

/**
 * Apply gravity and accumulated forces
 */
kernel void apply_forces(
    device RigidBody* bodies [[buffer(0)]],
    constant SimulationParams& params [[buffer(1)]],
    uint id [[thread_position_in_grid]]
) {
    if (id >= params.bodyCount) return;
    
    device RigidBody& body = bodies[id];
    
    // Skip static bodies (invMass == 0)
    if (body.invMass <= 0.0f) return;
    
    // Apply gravity (in -Z direction for our coordinate system)
    body.force.z -= params.gravity * body.mass;
}

/**
 * Integrate velocities and positions (Semi-implicit Euler)
 */
kernel void integrate_bodies(
    device RigidBody* bodies [[buffer(0)]],
    constant SimulationParams& params [[buffer(1)]],
    uint id [[thread_position_in_grid]]
) {
    if (id >= params.bodyCount) return;
    
    device RigidBody& body = bodies[id];
    
    // Skip static bodies
    if (body.invMass <= 0.0f) return;
    
    float dt = params.deltaTime;
    
    // Linear dynamics: acceleration = force / mass
    float3 acceleration = body.force * body.invMass;
    
    // Semi-implicit Euler: update velocity first, then position
    body.velocity += acceleration * dt;
    body.position += body.velocity * dt;
    
    // Angular dynamics (simplified - no inertia tensor)
    body.angularVelocity += body.torque * body.invMass * dt;
    
    // Update orientation quaternion
    float3 w = body.angularVelocity;
    float4 q = body.orientation;
    
    // Quaternion derivative: q' = 0.5 * omega * q
    float4 dq = 0.5f * float4(
        w.x * q.w + w.z * q.y - w.y * q.z,
        w.y * q.w - w.z * q.x + w.x * q.z,
        w.z * q.w + w.y * q.x - w.x * q.y,
       -w.x * q.x - w.y * q.y - w.z * q.z
    );
    
    body.orientation = normalize(q + dq * dt);
    
    // Clear accumulated forces for next frame
    body.force = float3(0.0f);
    body.torque = float3(0.0f);
}

/**
 * Broad phase collision detection using spatial hashing
 * (To be expanded for full collision system)
 */
kernel void detect_collisions_broad(
    device RigidBody* bodies [[buffer(0)]],
    constant SimulationParams& params [[buffer(1)]],
    device atomic_uint* collisionPairs [[buffer(2)]],
    uint id [[thread_position_in_grid]]
) {
    if (id >= params.bodyCount) return;
    
    // Placeholder for spatial hashing collision detection
    // Will be implemented in Phase 2
}
