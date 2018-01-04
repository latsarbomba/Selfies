#pragma once
#include <d3dx9math.h>

#define M_RADPI		57.295779513082f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

class CMaths
{
private:
	int ScreenW = 0;
	int ScreenH = 0;
public:

	VOID GetScreenSize(int& Screen_W, int& Screen_H)
	{
		Screen_W = ScreenW;
		Screen_H = ScreenH;
	}

	VOID UpdateScreenResolution(int ScreenHeaight, int ScreenWidth)
	{
		ScreenH = ScreenHeaight;
		ScreenW = ScreenWidth;
	}

	D3DXMATRIX ToMatrix(FRotator rot, Vector origin = Vector(0, 0, 0))
	{
		float radPitch = static_cast<float>(rot.Pitch * M_PI / 180.f);
		float radYaw = static_cast<float>(rot.Yaw * M_PI / 180.f);
		float radRoll = static_cast<float>(rot.Roll * M_PI / 180.f);

		float SP = static_cast<float>(sin(radPitch));
		float CP = static_cast<float>(cos(radPitch));
		float SY = static_cast<float>(sin(radYaw));
		float CY = static_cast<float>(cos(radYaw));
		float SR = static_cast<float>(sin(radRoll));
		float CR = static_cast<float>(cos(radRoll));

		D3DXMATRIX matrix;

		{
			matrix.m[0][0] = CP * CY;
			matrix.m[0][1] = CP * SY;
			matrix.m[0][2] = SP;
			matrix.m[0][3] = 0.f;
		}

		{
			matrix.m[1][0] = SR * SP * CY - CR * SY;
			matrix.m[1][1] = SR * SP * SY + CR * CY;
			matrix.m[1][2] = -SR * CP;
			matrix.m[1][3] = 0.f;
		}

		{
			matrix.m[2][0] = -(CR * SP * CY + SR * SY);
			matrix.m[2][1] = CY * SR - CR * SP * SY;
			matrix.m[2][2] = CR * CP;
			matrix.m[2][3] = 0.f;
		}

		{
			matrix.m[3][0] = origin.x;
			matrix.m[3][1] = origin.y;
			matrix.m[3][2] = origin.z;
			matrix.m[3][3] = 1.f;
		}

		return matrix;
	}

	bool WorldToScreen(Vector WorldLocation, FCameraCacheEntry CameraCacheL, Vector& out) //, int Swidth, int Sheight
	{
		auto POV = CameraCacheL.POV;
		FRotator Rotation = POV.Rotation; // FRotator

		D3DXMATRIX tempMatrix = ToMatrix(Rotation); // Matrix

		Vector vAxisX = Vector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]), 
			   vAxisY = Vector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]),
			   vAxisZ = Vector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		Vector vDelta = WorldLocation - POV.Location;
		Vector vTransformed = Vector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		float FovAngle = POV.FOV;
		float ScreenCenterX = ScreenW / 2;
		float ScreenCenterY = ScreenH / 2;

		out.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / static_cast<float>(tan(FovAngle * static_cast<float>(M_PI) / 360.f))) /
			vTransformed.z;
		out.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / static_cast<float>(tan(FovAngle * static_cast<float>(M_PI) / 360.f))) /
			vTransformed.z;

		out.x = out.x;
		out.y = out.y;

		if (out.x > ScreenW || out.y > ScreenH) return false;

		return true;
	}
};
