#pragma once

#include <math.h>
#define M_PI 3.1415926535897931f



struct Vector2
{
public:
	float x;
	float y;

	Vector2() {}
	Vector2(float _X, float _Y) : x(_X), y(_Y) {}
};

struct FQuat
{
public:
	float w;
	float x;
	float y;
	float z;

	FQuat() {}
	FQuat(float _X, float _Y, float _Z, float _W) : x(_X), y(_Y), z(_Z), w(_W) {}
};

class FRotator
{
public:
	float Pitch;
	float Yaw;
	float Roll;

	FRotator() {}
	FRotator(float pitch, float yaw, float roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}

	double Length()
	{
		return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
	}

	FRotator Clamp()
	{
		FRotator& result = *this;

		if (result.Pitch > 180)
			result.Pitch -= 360;

		else if (result.Pitch < -180)
			result.Pitch += 360;

		if (result.Yaw > 180)
			result.Yaw -= 360;

		else if (result.Yaw < -180)
			result.Yaw += 360;

		if (result.Pitch < -89)
			result.Pitch = -89;

		if (result.Pitch > 89)
			result.Pitch = 89;

		while (result.Yaw < -180.0f)
			result.Yaw += 360.0f;

		while (result.Yaw > 180.0f)
			result.Yaw -= 360.0f;

		result.Roll = 0;

		return result;
	}

	/*Vector ToVec()
	{
		float radPitch = (float)(Pitch * M_PI / 180.f);
		float radYaw = (float)(Yaw * M_PI / 180.f);

		float SP = (float)sin(radPitch);
		float CP = (float)cos(radPitch);
		float SY = (float)sin(radYaw);
		float CY = (float)cos(radYaw);

		return Vector(CP * CY, CP * SY, SP);
	}*/

	FRotator operator+(FRotator& r)
	{
		return FRotator(Pitch + r.Pitch, Yaw + r.Yaw, Roll + r.Roll);
	}
	FRotator operator-(FRotator& r)
	{
		return FRotator(Pitch - r.Pitch, Yaw - r.Yaw, Roll - r.Roll);
	}
	FRotator operator*(float r)
	{
		return FRotator(Pitch * r, Yaw * r, Roll * r);
	}
};

struct Vector
{
public:
	float x;
	float y;
	float z;

	Vector() {}
	Vector(float _X, float _Y, float _Z) : x(_X), y(_Y), z(_Z) {}
	Vector(float _X, float _Y) : x(_X), y(_Y), z(0) {}

	Vector& operator=(Vector r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
		return *this;
	}

	Vector operator+(Vector& r)
	{
		return Vector(x + r.x, y + r.y, z + r.z);
	}

	Vector operator-(Vector& r)
	{
		return Vector(x - r.x, y - r.y, z - r.z);
	}

	Vector operator+(float r)
	{
		return Vector(x + r, y + r, z + r);
	}

	Vector operator-(float r)
	{
		return Vector(x - r, y - r, z - r);
	}

	Vector operator/(float r)
	{
		return Vector(x / r, y / r, z / r);
	}

	Vector operator*(float r)
	{
		return Vector(x * r, y * r, z * r);
	}

	Vector operator*(double r)
	{
		return Vector(x * r, y * r, z * r);
	}

	Vector operator*(int r)
	{
		return Vector(x * r, y * r, z * r);
	}

	Vector& operator/=(float fl)
	{
		float oofl = 1.0f / fl;
		x *= oofl;
		y *= oofl;
		z *= oofl;
		return *this;
	}

	Vector& operator+=(const Vector& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Vector& operator+=(float v)
	{
		x += v; y += v; z += v;
		return *this;
	}

	Vector& operator-=(const Vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	Vector& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	inline void VectorClear(Vector& a)
	{
		a.x = a.y = a.z = 0.0f;
	}

	inline void Zero()
	{
		x = y = z = 0.0f;
	}

	float length()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	float lengthSqr()
	{
		return (pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	float distto(Vector to)
	{
		Vector& temp = *this;
		float tempdist = sqrt(pow(temp.x - to.x, 2) + pow(temp.y - to.y, 2) + pow(temp.z - to.z, 2));
		return tempdist;
	}

	inline float Dot(const Vector& vOther) const
	{
		const Vector& a = *this;

		return(a.x*vOther.x + a.y*vOther.y + a.z*vOther.z);
	}

	inline float Dot(const float* fOther) const
	{
		const Vector& a = *this;

		return(a.x*fOther[0] + a.y*fOther[1] + a.z*fOther[2]);
	}

	FRotator ToFRotator()
	{
		Vector& result = *this;
		FRotator rot = FRotator(0,0,0);

		float RADPI = (float)(180.f / M_PI);
		rot.Yaw = (float)atan2(result.y, result.x) * RADPI;
		rot.Pitch = (float)atan2(result.z, sqrt((result.x * result.x) + (result.y * result.y))) * RADPI;
		rot.Roll = 0;

		return rot;
	}

	Vector Clamp()
	{
		Vector& result = *this;

		if (result.x > 180)
			result.x -= 360;

		else if (result.x < -180)
			result.x += 360;

		if (result.y > 180)
			result.y -= 360;

		else if (result.y < -180)
			result.y += 360;

		if (result.x < -89)
			result.x = -89;

		if (result.x > 89)
			result.x = 89;

		while (result.y < -180.0f)
			result.y += 360.0f;

		while (result.y > 180.0f)
			result.y -= 360.0f;

		result.z = 0;

		return result;
	}

};


struct Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4() {}
	Vector4(float _X, float _Y, float _Z, float _W) : x(_X), y(_Y), z(_Z), w(_W) {}
	Vector4(float _X, float _Y) : x(_X), y(_Y), z(0), w(0) {}

	Vector4& operator=(Vector4 r)
	{
		x = r.x;
		y = r.y;
		z = r.z;
		w = r.w;
		return *this;
	}

	Vector4 operator+(Vector4& r)
	{
		return Vector4(x + r.x, y + r.y, z + r.z, w + r.w);
	}

	Vector4 operator-(Vector4& r)
	{
		return Vector4(x - r.x, y - r.y, z - r.z, w - r.w);
	}

	Vector4 operator+(float r)
	{
		return Vector4(x + r, y + r, z + r, w + r);
	}

	Vector4 operator-(float r)
	{
		return Vector4(x - r, y - r, z - r, w - r);
	}

	Vector4 operator/(float r)
	{
		return Vector4(x / r, y / r, z / r, w / r);
	}

	Vector4 operator*(float r)
	{
		return Vector4(x * r, y * r, z * r, w * r);
	}

	Vector4 operator*(double r)
	{
		return Vector4(x * r, y * r, z * r, w * r);
	}

	Vector4 operator*(int r)
	{
		return Vector4(x * r, y * r, z * r, w * r);
	}

	Vector4& operator/=(float fl)
	{
		float oofl = 1.0f / fl;
		x *= oofl;
		y *= oofl;
		z *= oofl;
		w *= oofl;
		return *this;
	}

	Vector4& operator+=(const Vector4& v)
	{
		x += v.x; y += v.y; z += v.z; w += v.z;
		return *this;
	}

	Vector4& operator+=(float v)
	{
		x += v; y += v; z += v; w += v;
		return *this;
	}

	Vector4& operator-=(const Vector4& v)
	{
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
		return *this;
	}

	Vector4& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		w *= fl;
		return *this;
	}

	Vector4& operator*=(const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	inline void VectorClear(Vector4& a)
	{
		a.x = a.y = a.z = a.w = 0.0f;
	}

	inline void Zero()
	{
		x = y = z = w= 0.0f;
	}
	
	Vector toVector(Vector4& a)
	{
		Vector temp;
		temp.x = a.x;
		temp.y = a.y;
		temp.z = a.z;
		return temp;
	}
};

class FTransform
{
public:
	Vector4 Rotation;
	Vector Translation;
	Vector Scale3D;

	FTransform() {}
	FTransform(Vector4 rot, Vector translation, Vector scale) : Rotation(rot), Translation(translation), Scale3D(scale) {}

	/*D3DMATRIX ToMatrixWithScale()
	{
		D3DXMATRIX OutMatrix;

		OutMatrix._41 = Translation.x;
		OutMatrix._42 = Translation.y;
		OutMatrix._43 = Translation.z;

		float x2 = Rotation.x + Rotation.x;
		float y2 = Rotation.y + Rotation.y;
		float z2 = Rotation.z + Rotation.z;

		{
			float xx2 = Rotation.x * x2;
			float yy2 = Rotation.y * y2;
			float zz2 = Rotation.z * z2;
			OutMatrix._11 = (1.0f - (yy2 + zz2)) * Scale3D.x;
			OutMatrix._22 = (1.0f - (xx2 + zz2)) * Scale3D.y;
			OutMatrix._33 = (1.0f - (xx2 + yy2)) * Scale3D.z;
		}

		{
			float yz2 = Rotation.y * z2;
			float wx2 = Rotation.w * x2;

			OutMatrix._32 = (yz2 - wx2) * Scale3D.z;
			OutMatrix._23 = (yz2 + wx2) * Scale3D.y;
		}

		{
			float xy2 = Rotation.x * y2;
			float wz2 = Rotation.w * z2;

			OutMatrix._21 = (xy2 - wz2) * Scale3D.y;
			OutMatrix._12 = (xy2 + wz2) * Scale3D.x;
		}

		{
			float xz2 = Rotation.x * z2;
			float wy2 = Rotation.w * y2;

			OutMatrix._31 = (xz2 + wy2) * Scale3D.z;
			OutMatrix._13 = (xz2 - wy2) * Scale3D.x;
		}

		OutMatrix._14 = 0.0f;
		OutMatrix._24 = 0.0f;
		OutMatrix._34 = 0.0f;
		OutMatrix._44 = 1.0f;

		return OutMatrix;
	}*/

	D3DXMATRIX ToMatrixWithScale() const
	{
		D3DXMATRIX m;

		m._41 = Translation.x;
		m._42 = Translation.y;
		m._43 = Translation.z;

		float x2 = Rotation.x + Rotation.x;
		float y2 = Rotation.y + Rotation.y;
		float z2 = Rotation.z + Rotation.z;

		float xx2 = Rotation.x * x2;
		float yy2 = Rotation.y * y2;
		float zz2 = Rotation.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * Scale3D.x;
		m._22 = (1.0f - (xx2 + zz2)) * Scale3D.y;
		m._33 = (1.0f - (xx2 + yy2)) * Scale3D.z;


		float yz2 = Rotation.y * z2;
		float wx2 = Rotation.w * x2;
		m._32 = (yz2 - wx2) * Scale3D.z;
		m._23 = (yz2 + wx2) * Scale3D.y;


		float xy2 = Rotation.x * y2;
		float wz2 = Rotation.w * z2;
		m._21 = (xy2 - wz2) * Scale3D.y;
		m._12 = (xy2 + wz2) * Scale3D.x;


		float xz2 = Rotation.x * z2;
		float wy2 = Rotation.w * y2;
		m._31 = (xz2 + wy2) * Scale3D.z;
		m._13 = (xz2 - wy2) * Scale3D.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}

};
