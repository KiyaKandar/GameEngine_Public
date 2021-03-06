#include "Ragdoll.h"

#include "../nclgl/Rendering/Renderer.h"
#include "../Physics/Engine/PhysicsEngine.h"
#include "../../Structure/PhysicsObject.h"
#include "CharacterModel.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "../nclgl/Maths/Matrix4.h"
#include "../Physics/Colliders/SphereCollider.h"

Ragdoll::Ragdoll(CharacterModel* playerModel, Renderer* renderer, PhysicsEngine* physicsEngine)
{
	this->renderer = renderer;
	this->physicsEngine = physicsEngine;
	this->playerModel = playerModel;
}

void Ragdoll::SpawnRagdoll(DataBase* database, const Matrix4& playerRotation)
{
	for each (SceneNode* modelLimb in playerModel->modelParts)
	{
		PhysicsObject* limbCopy = new PhysicsObject(renderer, physicsEngine, false, true);

		limbCopy->GetRigidBody()->collider = new SphereCollider(1);

		OBJMesh* mesh = static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find("../Data/meshes/robotcube.obj"));
		limbCopy->AddMesh(*mesh);

		limbCopy->GetRigidBody()->parentMesh = limbCopy->GetSceneNode();
		limbCopy->SetPosition(modelLimb->GetWorldTransform().GetPositionVector());
		limbCopy->SetSize(modelLimb->GetModelScale());
		limbCopy->GetSceneNode()->SetColour(modelLimb->GetColour());

		InitialiseRigidBody(limbCopy->GetRigidBody());
		limbCopy->GetSceneNode()->SetTransform(playerModel->GetTransform());

		ragdollLimbs.push_back(limbCopy);
	}
}

void Ragdoll::DeleteRagdoll()
{
	for each (PhysicsObject* limb in ragdollLimbs)
	{
		RemoveLimbFromSubsystems(limb);

		delete limb;
	}

	ragdollLimbs.clear();
}

void Ragdoll::RemoveLimbFromSubsystems(PhysicsObject* limb) const
{
	renderer->RemoveSceneNode(limb->GetSceneNode());
	physicsEngine->RemoveRigidBody(limb->GetRigidBody());
}

void Ragdoll::InitialiseRigidBody(RigidBody* rigidBody)
{
	rigidBody->gravity = -0.01f;
	rigidBody->UpdateMass(100.0f);
	rigidBody->drag = 0.9f;
	rigidBody->atRest = false;
}
