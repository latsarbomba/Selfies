#pragma once

#include "Vector.h"
#include "enums.h"
#include "structs.h"

#include "CMaths.h"
#include "Config.h"

class ATslPlayerState
{
public:
	DWORD_PTR BasePointer;
	char pad_0000[920]; //0x0000
	int32_t Score; //0x03A0
	int32_t Ping; //0x03A4
	wchar_t* PlayerName; //0x03A8
	char pad_03B0[24]; //0x03B0
	int32_t PlayerID; //0x03C8
	char pad_03CC[4]; //0x03CC
	int32_t StartTime; //0x03D0
	char pad_03D4[92]; //0x03D4
	int32_t Ranking; //0x0430
	bool bKilled; //0x0434
	bool bShowMapMarker; //0x0435
	char pad_0436[2]; //0x0436
	Vector MapMarkerPosition; //0x0438
	int32_t TeamNumber; //0x0444
	FTslPlayerScores PlayerScores; //0x0448
	FTslPlayerStatistics PlayerStatistics; //0x0454
	char pad_045C[3611]; //0x045C

	std::string GetName()
	{
		wchar_t* szPlayerName[64];
		if (ReadProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)PlayerName), &szPlayerName, sizeof(wchar_t*) * 64, NULL) != 0)
		{
			char szPlayerNameChar[64];
			sprintf(szPlayerNameChar, "%ws", szPlayerName); // Convert to Char
			std::string playername(szPlayerNameChar);
			return playername;
		}
	}
}; //Size: 0x1277

/*class USceneComponent
{
public:
	DWORD_PTR BasePointer; //0x0000
	char pad_0000[364]; //0x0008
	Vector EntityLocation; //0x0174
	char pad_0180[96]; //0x0180
	Vector RelativeLocation; //0x01E0
	Vector RelativeRotation; //0x01EC
	char pad_01F8[40]; //0x01F8
	Vector RelativeScale3D; //0x0220
	Vector RelativeTranslation; //0x022C
	char pad_0238[32]; //0x0238
	Vector ComponentVelocity; //0x0258
}; //Size: 0x0264
*/

class USceneComponent
{
public:
	DWORD_PTR BasePointer; //0x0000
	char pad_0008[364]; //0x0008
	Vector EntityLocation; //0x0174
	Vector Bounds; //0x0180
	char pad_018C[84]; //0x018C
	Vector RelativeLocation; //0x01E0
	Vector RelativeRotation; //0x01EC
	char pad_01F8[40]; //0x01F8
	Vector RelativeScale3D; //0x0220
	Vector RelativeTranslation; //0x022C
	char pad_0238[32]; //0x0238
	Vector ComponentVelocity; //0x0258
}; //Size: 0x0264


class UCharacterMovementComponent
{
public:
	char pad_0000[976]; //0x0000
	int32_t StanceMode; //0x03D0
}; //Size: 0x03D4

class AWeaponProcessor
{
public:
	DWORD_PTR BasePointer; //0x0000
	char pad_0008[1072]; //0x0008
	DWORD_PTR EquippedWeapons; //0x0438
	char pad_03C0[8]; //0x0440
	int32_t CurrentWeaponIndex; //0x0448
}; //Size: 0x03CC

class AActor
{
public:
	DWORD_PTR BasePointer; //0x0000
	char pad_0008[16]; //0x0008
	int32_t ActorID; //0x0018
	char pad_001C[304]; //0x001C
	float NetUpdateTime; //0x014C
	float NetUpdateFrequency; //0x0150
	float MinNetUpdateFrequency; //0x0154
	char pad_0158[40]; //0x0158
	DWORD_PTR RootComponent; //0x0180
	char pad_0188[336]; //0x0188
	DWORD_PTR DroppedItemGroupArray; //0x02D8
	uint32_t iDroppedItemGroupCount; //0x02E0
	char pad_02E4[220]; //0x02E4
	DWORD_PTR PlayerState; //0x03C0
	char pad_03C8[56]; //0x03C8
	DWORD_PTR Mesh; //0x0400
	DWORD_PTR CharacterMovement; //0x0408
	char pad_0410[1496]; //0x0410
	DWORD_PTR WeaponProcessor; //0x09E8
	char pad_09F0[788]; //0x09F0
	float CrouchedCameraMoveSpeed; //0x0D04
	float PronedCameraMoveSpeed; //0x0D08
	float GroggyCameraMoveSpeed; //0x0D0C
	char pad_0D10[140]; //0x0D10
	float TargetingSpeedModifier; //0x0D9C
	//char pad_0DA0[712]; //0x0DA0
	char pad_0DA0[732]; //0x0DA0
	float Health; //0x107C
	float HealthMax; //0x1080
	float GroggyHealth; //0x1084
	float GroggyHealthMax; //0x1088
	float DecreaseGroggyHealthPerSecond; //0x108C
	//char pad_107C[964]; //0x107C

	AWeaponProcessor GetWeaponProcessor()
	{
		return mem->Read<AWeaponProcessor>((DWORD_PTR)WeaponProcessor);
	}

	USceneComponent GetRootComponent()
	{
		return mem->Read<USceneComponent>((DWORD_PTR)RootComponent);
	}

	ATslPlayerState GetPlayerState()
	{
		return mem->Read<ATslPlayerState>((DWORD_PTR)PlayerState);
	}

	UCharacterMovementComponent GetCharacterMovement()
	{
		return mem->Read<UCharacterMovementComponent>((DWORD_PTR)CharacterMovement);
	}

	FTransform ReadFTransform(const uintptr_t& ptr)
	{
		return FTransform(mem->Read<Vector4>(ptr + 0x00), mem->Read<Vector>(ptr + 0x0010), mem->Read<Vector>(ptr + 0x0020));
	}

	FTransform GetBoneIndex(int boneIndex)
	{
		auto arrayBones = mem->Read<uintptr_t>(Mesh + 0x0790); //CachedBoneSpaceTransforms
		auto transform = ReadFTransform(arrayBones + (boneIndex * 0x30));
		return transform;
	}

	Vector GetBoneWithRotation(int id)
	{
		auto bone = GetBoneIndex(id); //get the bone vector without any rotation applied

		auto boneMatrix = bone.ToMatrixWithScale();
		auto componentToWorldMatrix = ReadFTransform(Mesh + 0x190).ToMatrixWithScale(); //bone translations are applied to ComponentToWorld matrix
		auto newMatrix = boneMatrix * componentToWorldMatrix;

		return Vector(newMatrix._41, newMatrix._42, newMatrix._43);
	}


	bool IsPlayer(Cache g_pCache)
	{
		if(ActorID == g_pCache.PlayerFemale_A 
		|| ActorID == g_pCache.PlayerFemale_A_C 
		|| ActorID == g_pCache.PlayerMale_A 
		|| ActorID == g_pCache.PlayerMale_A_C)
		{
			return true;
		}

		return false;
	}

	bool IsAlive()
	{
		if(Health < 0 && Health >= 100)
		{
			return true;
		}

		return false;
	}

	bool IsDowned()
	{
		if (Health == 0 && GroggyHealth < 0 && GroggyHealth >= 100)
		{
			return true;
		}
		return false;
	}

}; //Size: 0x1440

struct DroppedItemName
{
public:
	char pad_0x0000[0x28]; //0x0000
	uintptr_t string; //0x0028 
	__int32 lenght; //0x0030 
	char pad_0x0034[0x7D4]; //0x0034

	std::wstring GetName()
	{
		wchar_t wMyName[64];
		ReadProcessMemory(global->hProcess, (LPVOID)(string), &wMyName, sizeof(wchar_t) * 64, NULL);
		return std::wstring(wMyName);
	}
};

struct UItem
{
public:
	uintptr_t BasePointer; //0x0000 
	char pad_0x0008[0x38]; //0x0008
	uintptr_t pName; //0x0040 
	char pad_0x0048[0x10]; //0x0048
	uintptr_t pCategory; //0x0058 
	char pad_0x0060[0x7D8]; //0x0060

	DroppedItemName ItemName()
	{
		return mem->Read<DroppedItemName>(pName);
	}
};

struct UDroppedItemInteractionComponent
{
public:
	uintptr_t BasePointer; //0x0000 
	char pad_0x0008[0x1D8]; //0x0008
	Vector RelativeLocation; //0x01E0 
	char pad_0x01EC[0x25C]; //0x01EC
	uintptr_t pUItem; //0x0448 
	char pad_0x0450[0x2B8]; //0x0450

	UItem Item()
	{
		return mem->Read<UItem>(pUItem);
	}
};

class USkeleton
{
public:
	char pad_0000[392]; //0x0000
	DWORD_PTR Sockets; //0x0188
	int32_t BoneCount; //0x0190
}; //Size: 0x0194

class USkeletalMesh
{
public:
	char pad_0000[72]; //0x0000
	DWORD_PTR Skeleton; //0x0048

	USkeleton GetSkeleton()
	{
		return mem->Read<USkeleton>((DWORD_PTR)Skeleton);
	}
}; //Size: 0x0050

class USkeletalMeshComponent
{
public:
	char pad_0000[1920]; //0x0000
	DWORD_PTR SkeletalMesh; //0x0780
	char pad_0788[184]; //0x0788
	bool bForceWireframe; //0x0840
	bool bDisplayBones; //0x0841

	USkeletalMesh GetSkeleton()
	{
		return mem->Read<USkeletalMesh>((DWORD_PTR)SkeletalMesh);
	}
}; //Size: 0x0842

class USkeletalMeshSocket
{
public:
	char pad_0000[40]; //0x0000
	char* SocketName; //0x0028
	char* BoneName; //0x0030
	Vector RelativeLocation; //0x0038
	Vector RelativeRotation; //0x0044
	Vector RelativeScale; //0x0050
	bool bForceAlwaysAnimated; //0x005C
}; //Size: 0x005D

