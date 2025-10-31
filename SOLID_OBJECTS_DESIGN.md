# Solid 3D Objects - Physics System Design

## Overview
Upgrade from point-mass physics to full rigid body dynamics for destructible 3D objects.

## Current System Analysis

### Matter.h/cpp
- Simple point mass: `mass` and `charge`
- No rotational inertia
- No geometry information

### Movable.h/cpp
- Position, velocity, acceleration (linear motion)
- Roll, pitch, yaw (Euler angles - gimbal lock issues)
- Dir/side/up vectors for orientation
- Simple friction and velocity capping
- **Problem**: No angular momentum, no torques, simplified rotation

## Proposed Architecture

### 1. Physics Approach Decision

**Option A: Use Library (ReactPhysics3D)**
- ✅ Proven, optimized collision detection
- ✅ Stable constraint solver
- ❌ External dependency
- ❌ Less educational
- ❌ Harder to customize

**Option B: Custom Implementation** ⭐ **CHOSEN**
- ✅ Educational value
- ✅ Full control and customization
- ✅ Lean, no external deps
- ✅ Perfect for game-specific needs
- ❌ More implementation work

### 2. Core Classes

```
RigidBody (extends/replaces Matter)
├── Properties
│   ├── mass (scalar)
│   ├── inertiaTensor (3x3 matrix)
│   ├── inertiaTensorInv (cached inverse)
│   ├── centerOfMass (Vector3)
│   └── restitution (bounce coefficient)
├── State
│   ├── position (Vector3)
│   ├── orientation (Quaternion)
│   ├── linearVelocity (Vector3)
│   ├── angularVelocity (Vector3)
│   ├── force (Vector3 accumulator)
│   └── torque (Vector3 accumulator)
└── Methods
    ├── applyForce(force, point)
    ├── applyTorque(torque)
    ├── applyImpulse(impulse, point)
    ├── integrate(dt)
    └── getTransformMatrix()

Shape (abstract base)
├── getInertiaTensor(mass) → Matrix3x3
├── checkCollision(other) → CollisionInfo
├── getSupportPoint(direction) → Vector3  // For GJK/EPA
├── getBoundingSphere() → (center, radius)
└── draw()

Concrete Shapes:
├── BoxShape (width, height, depth)
├── SphereShape (radius)
├── PyramidShape (base, height)
├── CylinderShape (radius, height)
└── MeshShape (vertices[], faces[])

SolidObject (combines RigidBody + Shape + Drawable)
├── RigidBody* body
├── Shape* shape
├── Material properties
└── Implements: Movable, oDrawable

CompositeObject (for buildings/structures)
├── SolidObject[] children
├── FixedConstraint[] joints
└── Methods:
    ├── addObject(object, position, orientation)
    ├── applyDamage(point, force)
    └── breakConstraint(joint)
```

### 3. Physics Equations

#### Linear Motion (unchanged)
```
F = ma
v(t+dt) = v(t) + (F/m) * dt
x(t+dt) = x(t) + v(t) * dt
```

#### Rotational Motion (NEW)
```
τ = I * α            // Torque = Inertia * Angular acceleration
L = I * ω            // Angular momentum = Inertia * Angular velocity
dL/dt = τ            // Rate of change of angular momentum

Integration:
ω(t+dt) = ω(t) + I⁻¹ * τ * dt
q(t+dt) = q(t) + 0.5 * [ω * q] * dt  // Quaternion integration
```

#### Inertia Tensors (for common shapes)

**Box (width w, height h, depth d)**:
```
Ixx = (1/12) * m * (h² + d²)
Iyy = (1/12) * m * (w² + d²)
Izz = (1/12) * m * (w² + h²)
```

**Sphere (radius r)**:
```
I = (2/5) * m * r² * Identity
```

**Pyramid (square base side a, height h)**:
```
Ixx = Iyy = m * (a²/20 + h²/10)
Izz = m * a²/10
```

### 4. Collision Detection Strategy

**Phase 1: Broad Phase**
- Bounding sphere checks (fast rejection)
- Spatial partitioning (optional, for many objects)

**Phase 2: Narrow Phase**
- Box-Box: SAT (Separating Axis Theorem)
- Sphere-Sphere: Distance check
- Box-Sphere: Closest point
- Complex shapes: GJK/EPA algorithm (future)

**Phase 3: Collision Response**
- Calculate collision normal and penetration depth
- Apply impulse-based response
- Resolve interpenetration

```cpp
// Impulse formula (Newtonian)
Vector3 relativeVelocity = v2 + ω2 × r2 - v1 - ω1 × r1
float normalVelocity = dot(relativeVelocity, normal)
float impulse = -(1 + restitution) * normalVelocity / 
                (1/m1 + 1/m2 + dot(normal, (I1⁻¹ × (r1 × normal)) × r1 + 
                                          (I2⁻¹ × (r2 × normal)) × r2))
```

### 5. Matrix3x3 and Quaternion Classes

We'll need:
- **Matrix3x3**: For inertia tensors
- **Quaternion**: For rotations (avoid gimbal lock)

These will be added to the core/ directory.

### 6. Integration with Existing System

**Backward Compatibility**:
- Keep Agent, Enemy working as-is initially
- SolidObject can coexist with current objects
- Gradually migrate to new physics

**Rendering**:
- SolidObject implements oDrawable
- Uses OpenGL transformations from quaternion
- Each Shape knows how to draw itself

### 7. Destructible Buildings

**Approach**:
```cpp
CompositeObject building;
building.add(BoxShape, position1);  // Floor
building.add(BoxShape, position2);  // Wall
building.add(BoxShape, position3);  // Roof

// Each joint has a break threshold
building.setBreakForce(joint, 1000.0);

// When projectile hits:
building.applyImpulse(contactPoint, impulse);
// If force > threshold → break joint → parts separate
```

### 8. Implementation Phases

**Phase 1: Foundation** (2-3 hours)
- [x] Matrix3x3 class
- [x] Quaternion class  
- [x] RigidBody class
- [x] Basic integration test

**Phase 2: Shapes** (2-3 hours)
- [x] Shape abstract class
- [x] BoxShape implementation
- [x] SphereShape implementation
- [x] PyramidShape implementation
- [x] Drawing methods for each

**Phase 3: Physics** (3-4 hours)
- [x] Collision detection (sphere-sphere, box-box)
- [x] Collision response (impulse-based)
- [x] Force/torque integration
- [x] Testing with falling objects

**Phase 4: Game Integration** (2-3 hours)
- [x] SolidObject class
- [x] Add to GameData
- [x] Projectile collision with solids
- [x] Test scene with destructible objects

**Phase 5: Composite Objects** (2-3 hours)
- [x] CompositeObject class
- [x] Fixed constraints
- [x] Breakable joints
- [x] Build sample destructible building

**Total Estimated Time**: 11-16 hours

### 9. Performance Considerations

- Pre-calculate inertia tensors (don't recalculate each frame)
- Cache inverse inertia tensor
- Use spatial partitioning for collision broad phase
- Consider sleeping objects (stopped moving)
- Limit physics timestep (fixed substeps)

### 10. Testing Strategy

**Unit Tests**:
- Quaternion operations (normalization, multiplication)
- Inertia tensor calculations
- Collision detection accuracy

**Integration Tests**:
- Drop objects, verify they fall correctly
- Spinning objects maintain angular momentum
- Collisions conserve momentum (within tolerance)

**Game Tests**:
- Shoot at single box → it falls/moves
- Shoot at building → parts break away
- Stack boxes → they stay stable
- Knock over stack → realistic tumbling

## File Organization

```
include/
├── core/
│   ├── Matrix3x3.h
│   └── Quaternion.h
├── physics/           ← NEW
│   ├── RigidBody.h
│   ├── Shape.h
│   ├── BoxShape.h
│   ├── SphereShape.h
│   ├── PyramidShape.h
│   ├── CollisionDetector.h
│   └── PhysicsWorld.h
└── entities/
    ├── SolidObject.h
    └── CompositeObject.h

src/
├── core/
│   ├── Matrix3x3.cpp
│   └── Quaternion.cpp
├── physics/           ← NEW
│   ├── RigidBody.cpp
│   ├── Shape.cpp
│   ├── BoxShape.cpp
│   ├── SphereShape.cpp
│   ├── PyramidShape.cpp
│   ├── CollisionDetector.cpp
│   └── PhysicsWorld.cpp
└── entities/
    ├── SolidObject.cpp
    └── CompositeObject.cpp
```

## Next Steps

1. Get approval on this design
2. Start with Phase 1 (Matrix3x3, Quaternion, RigidBody)
3. Implement incrementally with tests
4. Demo at each phase milestone

## References

- Game Physics Engine Development by Ian Millington
- Real-Time Collision Detection by Christer Ericson
- Physics for Game Programmers by Grant Palmer
- Baraff & Witkin papers on rigid body simulation

