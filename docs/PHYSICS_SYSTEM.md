# Physics System Documentation

## Overview

Phase 1 of the solid object physics system has been successfully implemented. This provides the foundation for creating realistic 3D rigid body physics with proper rotational dynamics using quaternions (avoiding gimbal lock).

## New Components

### 1. Core Math Classes

#### Matrix3x3 ([include/core/Matrix3x3.h](../include/core/Matrix3x3.h))

A 3x3 matrix class for representing inertia tensors and performing rotational transformations.

**Key Features:**
- Row-major storage
- Matrix operations: addition, subtraction, multiplication
- Matrix-vector multiplication
- Transpose, determinant, and inverse calculations
- Factory methods: `identity()`, `diagonal()`, `zero()`

**Example Usage:**
```cpp
// Create inertia tensor for a box
double Ixx = 1.0, Iyy = 2.0, Izz = 3.0;
Matrix3x3 inertiaTensor = Matrix3x3::diagonal(Ixx, Iyy, Izz);

// Transform a vector
Vector v(1, 0, 0);
Vector transformed = inertiaTensor * v;

// Calculate inverse
Matrix3x3 invInertia = inertiaTensor.inverse();
```

#### Quaternion ([include/core/Quaternion.h](../include/core/Quaternion.h))

A quaternion class for representing 3D rotations without gimbal lock.

**Key Features:**
- Hamilton product for quaternion multiplication
- Conversion to/from axis-angle representation
- Conversion to/from Euler angles (roll, pitch, yaw)
- Vector rotation: `rotate(Vector v)`
- Conversion to rotation matrix
- Angular velocity integration: `integrate(omega, dt)`
- Normalization to prevent drift

**Example Usage:**
```cpp
// Create quaternion from axis-angle
Vector axis(0, 0, 1);  // Z-axis
double angle = M_PI / 4;  // 45 degrees
Quaternion q = Quaternion::fromAxisAngle(axis, angle);

// Rotate a vector
Vector v(1, 0, 0);
Vector rotated = q.rotate(v);

// Integrate angular velocity
Vector omega(0, 0, 1.0);  // Spinning around Z-axis at 1 rad/s
double dt = 0.016;  // ~60 FPS
Quaternion newOrientation = q.integrate(omega, dt);
```

### 2. Physics Classes

#### RigidBody ([include/physics/RigidBody.h](../include/physics/RigidBody.h))

Extends both `Matter` and `Movable` to add full rigid body dynamics with rotation.

**Key Features:**
- Quaternion-based orientation (replaces Euler angles)
- Angular velocity and angular momentum
- Inertia tensor (body space) with automatic world-space transformation
- Force and torque accumulators
- Linear and angular damping
- Impulse application at arbitrary points
- Proper physics integration using Newton-Euler equations

**Physics Equations:**
- Linear: `F = ma`, `v = v + a*dt`, `x = x + v*dt`
- Rotational: `τ = dL/dt`, `ω = I^-1 * L`, `q = q.integrate(ω, dt)`

**Example Usage:**
```cpp
// Create a rigid body
RigidBody body(Vector(0, 0, 5), 10.0);  // position, mass

// Set inertia tensor (e.g., for a box)
Matrix3x3 inertia = Matrix3x3::diagonal(5.0, 10.0, 15.0);
body.setInertiaTensor(inertia);

// Set initial orientation
body.setOrientation(0, 0, M_PI/4);  // roll, pitch, yaw

// Apply forces and torques
body.applyForce(Vector(0, 0, -9.8 * body.getMass()));  // Gravity
body.applyTorque(Vector(1, 0, 0));  // Spin around X-axis

// Apply impulse at a point (e.g., collision or projectile hit)
Vector hitPoint = body.getPosition() + Vector(1, 0, 0);
Vector impulse(0, 100, 0);
body.applyImpulse(impulse, hitPoint);

// Integrate physics (called each frame)
body.iterate();  // Uses TIME_STEP from Constants.h

// Query state
Vector position = body.getPosition();
Quaternion orientation = body.getOrientation();
Vector angularVel = body.getAngularVelocity();
Vector dir = body.getDir();  // Forward direction
Vector up = body.getUp();    // Up direction
```

### 3. Shape System

#### Shape ([include/physics/Shape.h](../include/physics/Shape.h))

Abstract base class for 3D shapes. Defines the interface for:
- Inertia tensor calculation
- Axis-Aligned Bounding Box (AABB) for collision detection
- OpenGL rendering
- Characteristic size

#### BoxShape ([include/physics/BoxShape.h](../include/physics/BoxShape.h))

First concrete shape implementation - a rectangular box (cuboid).

**Key Features:**
- Dimensions: width (X), height (Y), depth (Z)
- Automatic inertia tensor calculation: `I = (m/12) * (h² + d², w² + d², w² + h²)`
- AABB calculation with rotation support
- OpenGL quad-based rendering with proper normals
- Vertex extraction for collision detection

**Example Usage:**
```cpp
// Create a box shape
BoxShape box(2.0, 1.0, 0.5);  // width, height, depth

// Calculate inertia tensor for given mass
Matrix3x3 inertia = box.calculateInertiaTensor(10.0);

// Set color
box.setColor(Vector(0.8, 0.2, 0.2));  // Red-ish

// Get AABB for collision detection
Vector position(0, 0, 0);
Quaternion orientation = Quaternion::identity();
AABB bounds = box.getAABB(position, orientation);

// Render (with appropriate OpenGL transforms applied)
glPushMatrix();
glTranslatef(position.getX(), position.getY(), position.getZ());
// Apply rotation from quaternion...
box.render();
glPopMatrix();
```

## System Architecture

```
RigidBody (Physics + Rotation)
  ├─ extends Matter (mass, charge)
  ├─ extends Movable (position, velocity, acceleration)
  └─ adds:
      ├─ Quaternion orientation
      ├─ Vector angularVelocity
      ├─ Matrix3x3 inertiaTensor
      └─ physics integration

Shape (Geometry + Rendering)
  ├─ abstract interface
  └─ BoxShape
      ├─ dimensions (w, h, d)
      ├─ inertia calculation
      ├─ AABB for collision
      └─ OpenGL rendering
```

## Integration with Existing Code

The new physics system is **backward compatible** with existing code:

1. **Movable hierarchy preserved**: RigidBody extends both Matter and Movable
2. **Existing agents work**: Agent and Enemy still use Movable interface
3. **No changes required**: Existing game code continues to work unchanged

## Next Steps (Future Phases)

### Phase 2: Additional Shapes
- SphereShape
- PyramidShape
- CylinderShape
- Custom mesh shapes

### Phase 3: Collision Detection
- Broad phase (AABB tree)
- Narrow phase (SAT for boxes, GJK for general shapes)
- Contact point generation

### Phase 4: Collision Response
- Impulse-based resolution
- Friction and restitution
- Resting contact handling

### Phase 5: Composite Objects & Destruction
- SolidObject class (RigidBody + Shape + rendering)
- CompositeObject for buildings
- Breakable constraints
- Debris generation

### Phase 6: Game Integration
- SolidObject in GameData
- Projectile collision with solids
- Building destruction
- Terrain interaction

## Usage Example: Creating a Spinning Box

Here's a complete example of creating a physics-enabled spinning box:

```cpp
#include "RigidBody.h"
#include "BoxShape.h"

// Create rigid body
RigidBody body(Vector(0, 0, 10), 5.0);  // 10 units high, 5kg

// Create box shape
BoxShape box(1.0, 1.0, 1.0);  // 1x1x1 meter cube
box.setColor(Vector(0.8, 0.4, 0.2));  // Orange

// Configure physics
Matrix3x3 inertia = box.calculateInertiaTensor(body.getMass());
body.setInertiaTensor(inertia);
body.setAngularVelocity(Vector(0, 0, 2.0));  // Spin at 2 rad/s around Z

// Physics update loop (each frame)
void update() {
    // Apply gravity
    body.applyForce(Vector(0, 0, -9.8 * body.getMass()));

    // Integrate
    body.iterate();
}

// Rendering loop
void render() {
    Vector pos = body.getPosition();
    Quaternion orient = body.getOrientation();
    Matrix3x3 rotMatrix = orient.toRotationMatrix();

    glPushMatrix();

    // Translate to position
    glTranslatef(pos.getX(), pos.getY(), pos.getZ());

    // Apply rotation matrix
    GLfloat glMatrix[16];
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            glMatrix[i*4 + j] = rotMatrix.get(i, j);
        }
    }
    glMatrix[3] = glMatrix[7] = glMatrix[11] = 0;
    glMatrix[12] = glMatrix[13] = glMatrix[14] = 0;
    glMatrix[15] = 1;
    glMultMatrixf(glMatrix);

    // Render shape
    box.render();

    glPopMatrix();
}
```

## Performance Considerations

- **Quaternion normalization**: Done automatically during integration to prevent drift
- **Inertia tensor caching**: Inverse is cached in body space, transformed to world space as needed
- **Matrix operations**: Optimized for 3x3 matrices (no dynamic allocation)
- **SIMD potential**: Vector and Matrix operations can be optimized with SIMD in future

## Technical Notes

### Quaternion Integration

The integration uses the formula:
```
dq/dt = 0.5 * [ω * q]
```
where `[ω * q]` is the quaternion product with ω as a pure quaternion (0, ωx, ωy, ωz).

This ensures correct angular velocity integration while maintaining unit quaternion constraint.

### Inertia Tensor Transformation

The inertia tensor is stored in body space and transformed to world space when needed:
```
I_world = R * I_body * R^T
```
where R is the rotation matrix derived from the orientation quaternion.

This is more efficient than storing world-space inertia, which would need updates every frame.

### Coordinate System

- **X-axis**: Right (width)
- **Y-axis**: Forward/Up (height in world space)
- **Z-axis**: Up/Forward (depth in world space)
- OpenGL uses right-handed coordinate system

## Building

The new physics system is fully integrated into the CMake build:

```bash
./build.sh
```

New files automatically included:
- `src/core/Matrix3x3.cpp`
- `src/core/Quaternion.cpp`
- `src/physics/RigidBody.cpp`
- `src/physics/BoxShape.cpp`

## Testing

Build succeeded with all new classes:
- Matrix3x3 operations verified (inverse, transpose, multiplication)
- Quaternion rotation and integration tested
- RigidBody physics integration working
- BoxShape rendering and inertia calculation functional

## Summary

Phase 1 provides a solid foundation for rigid body physics:
- Quaternion-based rotations (no gimbal lock)
- Proper rotational dynamics with inertia tensors
- Extensible shape system
- Backward compatible with existing code

The system is ready for Phase 2 (more shapes) and Phase 3 (collision detection).
