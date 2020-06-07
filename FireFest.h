#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include "CVector.h"
#include "WepTypes.h"
#include "sigscan.h"
#include "HWBP.h"
using namespace std;
#define FUNC_AddProjectile 0x737C80
#define LOCAL_CPED 0xB6F5F0
#define MAX_PROJECTILES 32
#define REPEAT_DELAY 900
class FireFest
{
private:
	typedef void CClientGame;
	typedef void CEntity;
	typedef void CPed;
	typedef bool(__cdecl* ptrAddProjectile)(CEntity* creator, eWeaponType weaponType, CVector posn,
	float force, CVector* direction, CEntity* victim);
	static ptrAddProjectile AddProjectile;
	struct HacksData
	{
		bool FlareEnabled;
		bool BombingEnabled;
		bool StingerEnabled;
		bool MisleadEnabled;
		bool KickerEnabled;
		bool FugasEnabled;
		DWORD iterationDelay;
		DWORD LastTarget;
		HacksData()
		{
			LastTarget = 0x0;
			FlareEnabled = false;
			BombingEnabled = false;
			StingerEnabled = false;
			MisleadEnabled = false;
			KickerEnabled = false;
			FugasEnabled = false;
			iterationDelay = 105;
		}
	}; static HacksData hacks;
public:
	static void __stdcall InitHacks();
	static void __stdcall KeyChecker(void);
	static void __stdcall PedPoolParser(void);
	static CEntity* __stdcall GetLocalEntity(void);
	static CVector __stdcall GetMyOwnPos(void);
};