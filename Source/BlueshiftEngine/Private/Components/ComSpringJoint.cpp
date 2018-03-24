// Copyright(c) 2017 POLYGONTEK
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Precompiled.h"
#include "Render/Render.h"
#include "Components/ComTransform.h"
#include "Components/ComRigidBody.h"
#include "Components/ComSpringJoint.h"
#include "Game/GameWorld.h"

BE_NAMESPACE_BEGIN

OBJECT_DECLARATION("Spring Joint", ComSpringJoint, ComJoint)
BEGIN_EVENTS(ComSpringJoint)
END_EVENTS

void ComSpringJoint::RegisterProperties() {
    REGISTER_ACCESSOR_PROPERTY("anchor", "Anchor", Vec3, GetAnchor, SetAnchor, Vec3::zero, "Joint position in local space", PropertyInfo::EditorFlag);
    REGISTER_MIXED_ACCESSOR_PROPERTY("angles", "Angles", Angles, GetAngles, SetAngles, Vec3::zero, "Joint angles in local space", PropertyInfo::EditorFlag);
    REGISTER_ACCESSOR_PROPERTY("useLimits", "Use Limits", bool, GetEnableLimitDistances, SetEnableLimitDistances, false, "Activate joint limits", PropertyInfo::EditorFlag);
    REGISTER_ACCESSOR_PROPERTY("minDist", "Minimum Distance", float, GetMinimumDistance, SetMinimumDistance, 0.f, "", PropertyInfo::EditorFlag);
    REGISTER_ACCESSOR_PROPERTY("maxDist", "Maximum Distance", float, GetMaximumDistance, SetMaximumDistance, 0.f, "", PropertyInfo::EditorFlag);
    REGISTER_ACCESSOR_PROPERTY("stiffness", "Stiffness", float, GetStiffness, SetStiffness, 30.f, "", PropertyInfo::EditorFlag);
    REGISTER_ACCESSOR_PROPERTY("damping", "Damping", float, GetDamping, SetDamping, 0.2f, "", PropertyInfo::EditorFlag)
        .SetRange(0, 1, 0.01f);
}

ComSpringJoint::ComSpringJoint() {
}

ComSpringJoint::~ComSpringJoint() {
}

void ComSpringJoint::Init() {
    ComJoint::Init();

    // Mark as initialized
    SetInitialized(true);
}

void ComSpringJoint::Start() {
    ComJoint::Start();

    const ComTransform *transform = GetEntity()->GetTransform();
    const ComRigidBody *rigidBody = GetEntity()->GetComponent<ComRigidBody>();
    assert(rigidBody);

    // Fill up a constraint description 
    PhysConstraintDesc desc;
    desc.type = PhysConstraint::GenericSpring;
    desc.collision = collisionEnabled;
    desc.breakImpulse = breakImpulse;

    desc.bodyA = rigidBody->GetBody();
    desc.axisInA = localAxis;
    desc.anchorInA = transform->GetScale() * localAnchor;

    if (connectedBody) {
        Mat3 worldAxis = desc.bodyA->GetAxis() * localAxis;
        Vec3 worldAnchor = desc.bodyA->GetOrigin() + desc.bodyA->GetAxis() * desc.anchorInA;

        desc.bodyB = connectedBody->GetBody();
        desc.axisInB = connectedBody->GetBody()->GetAxis().TransposedMul(worldAxis);
        desc.anchorInB = connectedBody->GetBody()->GetAxis().TransposedMulVec(worldAnchor - connectedBody->GetBody()->GetOrigin());
    } else {
        desc.bodyB = nullptr;
    }

    // Create a constraint by description
    constraint = physicsSystem.CreateConstraint(&desc);

    PhysGenericSpringConstraint *genericSpringConstraint = static_cast<PhysGenericSpringConstraint *>(constraint);

    // Apply limit distances
    genericSpringConstraint->SetLinearLowerLimit(Vec3(0, 0, MeterToUnit(minDist)));
    genericSpringConstraint->SetLinearUpperLimit(Vec3(0, 0, MeterToUnit(maxDist)));
    genericSpringConstraint->EnableLinearLimits(true, true, enableLimitDistances);

    // Apply spring stiffness & damping
    genericSpringConstraint->SetLinearStiffness(Vec3(0, 0, stiffness));
    genericSpringConstraint->SetLinearDamping(Vec3(0, 0, damping));

    if (IsActiveInHierarchy()) {
        constraint->AddToWorld(GetGameWorld()->GetPhysicsWorld());
    }
}

const Vec3 &ComSpringJoint::GetAnchor() const {
    return localAnchor;
}

void ComSpringJoint::SetAnchor(const Vec3 &anchor) {
    this->localAnchor = anchor;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetFrameA(anchor, localAxis);
    }
}

Angles ComSpringJoint::GetAngles() const {
    return localAxis.ToAngles();
}

void ComSpringJoint::SetAngles(const Angles &angles) {
    this->localAxis = angles.ToMat3();
    this->localAxis.FixDegeneracies();

    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetFrameA(localAnchor, localAxis);
    }
}

void ComSpringJoint::SetEnableLimitDistances(bool enable) {
    this->enableLimitDistances = enable;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->EnableLinearLimits(true, true, enableLimitDistances);
    }
}

void ComSpringJoint::SetMinimumDistance(float minDist) {
    this->minDist = minDist;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetLinearLowerLimit(Vec3(0, 0, MeterToUnit(minDist)));
    }
}

void ComSpringJoint::SetMaximumDistance(float maxDist) {
    this->maxDist = maxDist;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetLinearUpperLimit(Vec3(0, 0, MeterToUnit(maxDist)));
    }
}

void ComSpringJoint::SetStiffness(float stiffness) {
    this->stiffness = stiffness;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetLinearStiffness(Vec3(0, 0, stiffness));
    }
}

void ComSpringJoint::SetDamping(float damping) {
    this->damping = damping;
    if (constraint) {
        ((PhysGenericSpringConstraint *)constraint)->SetLinearDamping(Vec3(0, 0, damping));
    }
}

void ComSpringJoint::DrawGizmos(const SceneView::Parms &sceneView, bool selected) {
    RenderWorld *renderWorld = GetGameWorld()->GetRenderWorld();

    const ComTransform *transform = GetEntity()->GetTransform();

    if (transform->GetOrigin().DistanceSqr(sceneView.origin) < 20000.0f * 20000.0f) {
        Vec3 worldOrigin = transform->GetTransform() * localAnchor;
        Mat3 worldAxis = transform->GetAxis() * localAxis;

        renderWorld->SetDebugColor(Color4::red, Color4::zero);
        renderWorld->DebugLine(worldOrigin - worldAxis[0] * CentiToUnit(2.5), worldOrigin + worldAxis[0] * CentiToUnit(2.5), 1);
        renderWorld->DebugLine(worldOrigin - worldAxis[1] * CentiToUnit(2.5), worldOrigin + worldAxis[1] * CentiToUnit(2.5), 1);

        renderWorld->DebugCircle(worldOrigin - worldAxis[2] * CentiToUnit(2), worldAxis[2], CentiToUnit(2.5));
        renderWorld->DebugCircle(worldOrigin, worldAxis[2], CentiToUnit(2.5));
        renderWorld->DebugCircle(worldOrigin + worldAxis[2] * CentiToUnit(2), worldAxis[2], CentiToUnit(2.5));
    }
}

BE_NAMESPACE_END
