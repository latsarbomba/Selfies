#pragma once
#include "menu/imgui.h"

struct Config
{
	bool bPlayerHealthESP = false;
	bool bDistanceESP = false;
	bool bLineESP = false;
	
	bool bPlayerNameESP = false;
	bool bRadarESP = false;
	bool bSkeletonESP = false;
	float MaxSkeletonDistance = 100.f;
	bool bDrawBones = false;
	bool bShowLoot = false;
	bool bVehicleESP = false;

	bool bDowned = false;
	bool bDownedDistance = false;
	bool bDownedHP = false;
	bool bDownedTimeToDie = false;

	bool bShowLocalMarker = false;
	bool bShowTeamMarker = false;
	bool bAirDrop = false;
	bool bAirdropContents = false;
	bool BoxESP = false;
	float BoxDistance = 100.f;
	int BoxType = -1;

	bool bNoRecoil = false;
	bool bNoSway = false;
	bool bFastFireRate = false;
	bool bFastBullets = false;
	bool bNoMuzzle = false;
	bool bNoSpread = false;

	bool bDebugMode = false;
	bool bLogIds = false;

	bool bAimbot = false;
	float FOV = 10.f;
	bool bDrawFOV = false;
	float Smooth = 100.f;
	float MaxDist = 0.f;
	std::string aimkey = "sidebtn";
	bool bVelocityPrediction = false;
	bool bBulletDropPrediction = false; //TODO
	int aimbotBone = -1;
	bool ShowAimLockBone = false;
	bool ShowLockIndication = false;
	int LockIndicationType = -1;
	bool locktarget = false;

	bool bAutoLogin = false;
	std::string username = "FORUM_USERNAME";
	std::string password = "FORUM_PASSWORD";


	//Colors 
	ImVec4 SkelColorIMGUI = ImColor(1, 0, 0);
	int SkeletonWidth = 1;
	ImVec4 BoxColorIMGUI = ImColor(1, 0, 0);
	int BoxWidth = 1;

	ImVec4 DrawFOVIMGUI = ImColor(0, 0, 0);

	ImVec4 Distance280IMGUI = ImColor(1, 1, 1);
	ImVec4 Distance560IMGUI = ImColor(1, 0, 0);
	ImVec4 DistanceMorethan560IMGUI = ImColor(0, 0, 1);

	ImVec4 LootDefaultColor = ImColor(0, 0, 1);
	ImVec4 LootL1 = ImColor(0, 0, 1);
	ImVec4 LootL2 = ImColor(0, 0, 1);
	ImVec4 LootL3 = ImColor(0, 0, 1);
	ImVec4 LootGuns = ImColor(0, 0, 1);
	ImVec4 LootAttachements = ImColor(0, 0, 1);
	ImVec4 LootMedical = ImColor(0, 0, 1);
	ImVec4 LootAmmo = ImColor(0, 0, 1);
	ImVec4 LootScopes = ImColor(0, 0, 1);
	ImVec4 LootCustom = ImColor(0, 0, 1);

	float TextMultiplier = 1;
	int TextFont = 0;
	bool TextOutile = false;

	ImVec4 AimbotLockColour = ImColor(0, 0, 1);
	ImVec4 AimbotLockBoneColour = ImColor(0, 0, 1);
	float BoneLockrad = 10.f;

	bool bLootESP = false;
	bool bLootDistanceESP = false;

	//Loot options
	bool
		bLootFilterNone = false, 
		bLootFilterL1 = false,
		bLootFilterL2 = false, 
		bLootFilterL3 = false, 
		bLootFilterGuns = false, 
		bLootFilterAttachments = false,
		bAmmoLoot = false,
		bMedicalLoot = false, 
		bScopesLoot = false, 
		bCustomFilter = false;

	int iSleepRate = 10; // Optimization 

	bool AutoScale = true;
	int ScreenH = 1080;
	int ScreenW = 1920;
	float flOverlayOffset = 0.0f;
};


class Cache {
public:
	int DroppedItemInteractionComponent;
	int DroppedItemGroup;
	int PlayerFemale_A;
	int PlayerFemale_A_C;
	int PlayerMale_A;
	int PlayerMale_A_C;
	int DeathDropItemPackage_C;
	int Boat_PG117_C;
	int Dacia_A_01_C;
	int Dacia_A_02_C;
	int Dacia_A_03_C;
	int BP_Motorbike_04_SideCar_C;
	int BP_Motorbike_03_C;
	int BP_Motorbike_04_C;
	int Buggy_A_01_C;
	int Buggy_A_03_C;
	int Buggy_A_02_C;
	int Dacia_A_04_C;
	int Uaz_B_01_C;
	int Uaz_A_01_C;
	int Uaz_C_01_C;
	int Carapackage_RedBox_C;
};