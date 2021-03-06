#include "WeaponChange.h"

#include "../../Gameplay/Weapon Control/Gun.h"
#include "../../Gameplay/Weapon Types/RocketLauncher.h"
#include "../../Gameplay/Weapon Types/Pistol.h"

#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "../nclgl/Rendering/Renderer.h"
#include "../nclgl/Meshes/Mesh.h"
#include "../Physics/Engine/PhysicsEngine.h"

const bool WeaponChange::RocketLauncherAvailable(const std::string& playerTag)
{
	return MessageSystem::GetInstance()->MessageTransmitting(StringUtility::Hash(playerTag + "rocketlauncher"));
}

const bool WeaponChange::MachineGunAvailable(const std::string& playerTag)
{
	return MessageSystem::GetInstance()->MessageTransmitting(StringUtility::Hash(playerTag + "machinegun"));
}

Gun* WeaponChange::CopyToRocketLauncher(Gun* currentWeapon)
{
	const string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	RocketLauncher* newweapon = new RocketLauncher(database, renderer, physicsEngine, weaponMesh);
	newweapon->parent = playerTag;

	newWeapon = playerTag + "hasrocketlauncher";
	newWeaponAnnouncementToStop = playerTag + "hasmachinegun";
	previousAnnouncement = playerTag + "rocketlauncher";

	return newweapon;
}

Gun* WeaponChange::CopyToMachineGun(Gun* currentWeapon)
{
	const string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	Pistol* newweapon = new Pistol(database, renderer, physicsEngine, weaponMesh);
	newweapon->fireDelay = 150;
	newweapon->parent = playerTag;

	newWeapon = playerTag + "hasmachinegun";
	newWeaponAnnouncementToStop = playerTag + "hasrocketlauncher";
	previousAnnouncement = playerTag + "machinegun";

	return newweapon;
}

void WeaponChange::AnnounceChange() const
{
	MessageSystem::GetInstance()->BeginEvent(StringUtility::Hash(newWeapon));
}

void WeaponChange::StopPreviousEvents() const
{
	MessageSystem::GetInstance()->StopEvent(StringUtility::Hash(newWeaponAnnouncementToStop));
	MessageSystem::GetInstance()->StopEvent(StringUtility::Hash(previousAnnouncement));
}
