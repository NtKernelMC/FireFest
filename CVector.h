#pragma once
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cmath>
#include <string>
using namespace std;
#define FLOAT_EPSILON 0.0001f
#pragma warning (disable : 4244)
class CVector4D
{
public:
	CVector4D(void)
	{
		fX = 0;
		fY = 0;
		fZ = 0;
		fW = 0;
	}

	CVector4D(float _fX, float _fY, float _fZ, float _fW)
	{
		fX = _fX;
		fY = _fY;
		fZ = _fZ;
		fW = _fW;
	}

	CVector4D(const CVector4D& vec)
	{
		fX = vec.fX;
		fY = vec.fY;
		fZ = vec.fZ;
		fW = vec.fW;
	}

	CVector4D& operator=(const CVector4D& vec)
	{
		fX = vec.fX;
		fY = vec.fY;
		fZ = vec.fZ;
		fW = vec.fW;
		return *this;
	}

	float DotProduct(CVector4D& other) const { return fX * other.fX + fY * other.fY + fZ * other.fZ; }

	float Length() const { return sqrt(fX * fX + fY * fY + fZ * fZ + fW * fW); }

	float LengthSquared(void) const { return (fX * fX) + (fY * fY) + (fZ * fZ) + (fW * fW); }

	void Normalize(void)
	{
		float fLength = Length();
		if (fLength > 0.0f)
		{
			fX /= fLength;
			fY /= fLength;
			fZ /= fLength;
			fW /= fLength;
		}
	}

	CVector4D operator*(float fRight) const { return CVector4D(fX * fRight, fY * fRight, fZ * fRight, fW * fRight); }

	CVector4D operator/(float fRight) const
	{
		float fRcpValue = 1 / fRight;
		return CVector4D(fX * fRcpValue, fY * fRcpValue, fZ * fRcpValue, fW * fRcpValue);
	}

	CVector4D operator+(const CVector4D& vecRight) const { return CVector4D(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ, fW + vecRight.fW); }

	CVector4D operator-(const CVector4D& vecRight) const { return CVector4D(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ, fW - vecRight.fW); }

	CVector4D operator*(const CVector4D& vecRight) const { return CVector4D(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ, fW * vecRight.fW); }

	CVector4D operator/(const CVector4D& vecRight) const { return CVector4D(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ, fW / vecRight.fW); }

	void operator+=(float fRight)
	{
		fX += fRight;
		fY += fRight;
		fZ += fRight;
		fW += fRight;
	}

	void operator+=(const CVector4D& vecRight)
	{
		fX += vecRight.fX;
		fY += vecRight.fY;
		fZ += vecRight.fZ;
		fW += vecRight.fW;
	}

	void operator-=(float fRight)
	{
		fX -= fRight;
		fY -= fRight;
		fZ -= fRight;
		fW -= fRight;
	}

	void operator-=(const CVector4D& vecRight)
	{
		fX -= vecRight.fX;
		fY -= vecRight.fY;
		fZ -= vecRight.fZ;
		fW -= vecRight.fW;
	}

	void operator*=(float fRight)
	{
		fX *= fRight;
		fY *= fRight;
		fZ *= fRight;
		fW *= fRight;
	}

	void operator*=(const CVector4D& vecRight)
	{
		fX *= vecRight.fX;
		fY *= vecRight.fY;
		fZ *= vecRight.fZ;
		fW *= vecRight.fW;
	}

	void operator/=(float fRight)
	{
		fX /= fRight;
		fY /= fRight;
		fZ /= fRight;
		fW /= fRight;
	}

	void operator/=(const CVector4D& vecRight)
	{
		fX /= vecRight.fX;
		fY /= vecRight.fY;
		fZ /= vecRight.fZ;
		fW /= vecRight.fW;
	}

	bool operator==(const CVector4D& param) const
	{
		return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON) &&
			(fabs(fW - param.fW) < FLOAT_EPSILON));
	}

	bool operator!=(const CVector4D& param) const
	{
		return ((fabs(fX - param.fX) >= FLOAT_EPSILON) || (fabs(fY - param.fY) >= FLOAT_EPSILON) || (fabs(fZ - param.fZ) >= FLOAT_EPSILON) ||
			(fabs(fW - param.fW) >= FLOAT_EPSILON));
	}

	float fX;
	float fY;
	float fZ;
	float fW;
};
class CVector
{
public:
	float fX, fY, fZ;

	CVector()
	{
		this->fX = 0;
		this->fY = 0;
		this->fZ = 0;
	};

	CVector(float fX, float fY, float fZ)
	{
		this->fX = fX;
		this->fY = fY;
		this->fZ = fZ;
	}

	float Normalize(void)
	{
		float t = sqrt(fX * fX + fY * fY + fZ * fZ);
		if (t > FLOAT_EPSILON)
		{
			float fRcpt = 1 / t;
			fX *= fRcpt;
			fY *= fRcpt;
			fZ *= fRcpt;
		}
		else
			t = 0;
		return t;
	}

	float Length(void) const { return sqrt((fX * fX) + (fY * fY) + (fZ * fZ)); }

	float LengthSquared(void) const { return (fX * fX) + (fY * fY) + (fZ * fZ); }

	float DotProduct(const CVector* param) const { return fX * param->fX + fY * param->fY + fZ * param->fZ; }

	void CrossProduct(const CVector* param)
	{
		float _fX = fX, _fY = fY, _fZ = fZ;
		fX = _fY * param->fZ - param->fY * _fZ;
		fY = _fZ * param->fX - param->fZ * _fX;
		fZ = _fX * param->fY - param->fX * _fY;
	}

	// Convert (direction) to rotation
	CVector ToRotation(void) const
	{
		CVector vecRotation;
		vecRotation.fZ = atan2(fY, fX);
		CVector vecTemp(sqrt(fX * fX + fY * fY), fZ, 0);
		vecTemp.Normalize();
		vecRotation.fY = atan2(vecTemp.fX, vecTemp.fY) - M_PI / 2;
		return vecRotation;
	}

	// Return a perpendicular direction
	CVector GetOtherAxis(void) const
	{
		CVector vecResult;
		if (std::abs(fX) > std::abs(fY))
			vecResult = CVector(fZ, 0, -fX);
		else
			vecResult = CVector(0, -fZ, fY);
		vecResult.Normalize();
		return vecResult;
	}

	CVector Clone(void) const
	{
		CVector vecResult;
		vecResult.fX = fX;
		vecResult.fY = fY;
		vecResult.fZ = fZ;
		return vecResult;
	}

	CVector operator+(const CVector& vecRight) const { return CVector(fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ); }

	CVector operator-(const CVector& vecRight) const { return CVector(fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ); }

	CVector operator*(const CVector& vecRight) const { return CVector(fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ); }

	CVector operator*(float fRight) const { return CVector(fX * fRight, fY * fRight, fZ * fRight); }

	CVector operator/(const CVector& vecRight) const { return CVector(fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ); }

	CVector operator/(float fRight) const
	{
		float fRcpValue = 1 / fRight;
		return CVector(fX * fRcpValue, fY * fRcpValue, fZ * fRcpValue);
	}

	CVector operator-() const { return CVector(-fX, -fY, -fZ); }

	void operator+=(float fRight)
	{
		fX += fRight;
		fY += fRight;
		fZ += fRight;
	}

	void operator+=(const CVector& vecRight)
	{
		fX += vecRight.fX;
		fY += vecRight.fY;
		fZ += vecRight.fZ;
	}

	void operator-=(float fRight)
	{
		fX -= fRight;
		fY -= fRight;
		fZ -= fRight;
	}

	void operator-=(const CVector& vecRight)
	{
		fX -= vecRight.fX;
		fY -= vecRight.fY;
		fZ -= vecRight.fZ;
	}

	void operator*=(float fRight)
	{
		fX *= fRight;
		fY *= fRight;
		fZ *= fRight;
	}

	void operator*=(const CVector& vecRight)
	{
		fX *= vecRight.fX;
		fY *= vecRight.fY;
		fZ *= vecRight.fZ;
	}

	void operator/=(float fRight)
	{
		float fRcpValue = 1 / fRight;
		fX *= fRcpValue;
		fY *= fRcpValue;
		fZ *= fRcpValue;
	}

	void operator/=(const CVector& vecRight)
	{
		fX /= vecRight.fX;
		fY /= vecRight.fY;
		fZ /= vecRight.fZ;
	}

	bool operator==(const CVector& param) const
	{
		return ((fabs(fX - param.fX) < FLOAT_EPSILON) && (fabs(fY - param.fY) < FLOAT_EPSILON) && (fabs(fZ - param.fZ) < FLOAT_EPSILON));
	}

	bool operator!=(const CVector& param) const
	{
		return ((fabs(fX - param.fX) >= FLOAT_EPSILON) || (fabs(fY - param.fY) >= FLOAT_EPSILON) || (fabs(fZ - param.fZ) >= FLOAT_EPSILON));
	}

	CVector& operator=(const CVector4D& vec)
	{
		fX = vec.fX;
		fY = vec.fY;
		fZ = vec.fZ;
		return *this;
	}
};