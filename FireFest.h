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
//#include "MtaStuff.h"
using namespace std;
#define FUNC_AddProjectile 0x737C80
#define LOCAL_CPED 0xB6F5F0
#define MAX_PROJECTILES 32
#define REPEAT_DELAY 900
class FireFest
{
private:
	enum eExplosionType
	{
		EXP_TYPE_GRENADE,
		EXP_TYPE_MOLOTOV,
		EXP_TYPE_ROCKET,
		EXP_TYPE_TINY_ROCKET,
		EXP_TYPE_VEHICLE,
		EXP_TYPE_FASTVEHICLE,
		EXP_TYPE_BOAT,
		EXP_TYPE_HELICOPTER,
		EXP_TYPE_MINE,
		EXP_TYPE_OBJECT,
		EXP_TYPE_TANK,
		EXP_TYPE_SMALL,
		EXP_TYPE_VERY_TINY
	};
	static DWORD scanAddr;
	typedef void CClientGame;
	static CClientGame* g_pClientGame;
	typedef void CEntity;
	typedef void CPed;
	typedef void CClientPlayer;
	static CClientPlayer* pPlayer;
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
		bool TeargasEnabled;
		bool ExplosionEnabled;
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
			TeargasEnabled = false;
			ExplosionEnabled = false;
			iterationDelay = 105;
		}
	}; static HacksData hacks;
public:
	static void __stdcall InitHacks();
	static void __stdcall KeyChecker(void);
	static void __stdcall PedPoolParser(void);
	static CEntity* __stdcall GetLocalEntity(void);
	static CVector __stdcall GetMyOwnPos(void);
	static CClientPlayer* GetClosestRemotePlayer(const CVector& vecTemp, float fMaxDistance);
	static void __stdcall InstallDoPulsePreFrameHook();
	static void __fastcall DoPulsePreFrame(CClientGame* ECX, void* EDX);
	static bool __cdecl CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize);
};