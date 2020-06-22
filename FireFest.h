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
#include "Registry.h"
//#include "MtaStuff.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")
using namespace std;
#define FUNC_AddProjectile 0x737C80
#define LOCAL_CPED 0xB6F5F0
#define MAX_PROJECTILES 32
static DWORD REPEAT_DELAY = 900;
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
	typedef void(__thiscall* callSetFrozen)(void* ECX, bool freeze);
	static callSetFrozen pSetFrozen;
	typedef void(__thiscall* callSetLocked)(void* ECX, bool freeze);
	static callSetLocked pSetLocked;
	typedef void(__thiscall* callSetEngine)(void* ECX, bool freeze);
	static callSetEngine pSetEngine;
	static ptrAddProjectile AddProjectile;
	struct HacksData
	{
		enum AIMING_TYPE
		{
			AIM_MASSIVE = 1,
			AIM_TARGET = 2
		};
		CVector CamPos;
		DWORD ScriptNumber;
		AIMING_TYPE aimMode;
		bool LuaDumper;
		bool FlareEnabled;
		bool BombingEnabled;
		bool StingerEnabled;
		bool MisleadEnabled;
		bool KickerEnabled;
		bool FugasEnabled;
		bool TeargasEnabled;
		bool ExplosionEnabled;
		bool AntiFreeze;
		bool AntiLock;
		bool AntiKeys;
		DWORD FlareKey, BombKey, StingerKey, MisleadKey, KickerKey, FugasKey, TeargasKey, ExplodeKey, OpenerKey;
		DWORD iterationDelay;
		DWORD LastTarget;
		string lua_code;
		eExplosionType ExplosionType;
		bool PerformLuaInjection;
		HacksData()
		{
			AntiLock = true; AntiFreeze = true; AntiKeys = true;
			aimMode = AIMING_TYPE::AIM_MASSIVE; ExplosionType = EXP_TYPE_TANK;
			LastTarget = 0x0; ScriptNumber = 0x1; LuaDumper = false;
			FlareKey = VK_END, BombKey = VK_DELETE, StingerKey = VK_HOME, MisleadKey = VK_INSERT, KickerKey = VK_SNAPSHOT, 
			FugasKey = VK_NUMPAD1, TeargasKey = VK_NUMPAD2, ExplodeKey = VK_NUMPAD3;
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
	//static void __stdcall VehPoolParser(void);
	static CEntity* __stdcall GetLocalEntity(void);
	static CVector __stdcall GetMyOwnPos(void);
	static CClientPlayer* GetClosestRemotePlayer(const CVector& vecTemp, float fMaxDistance);
	static void __stdcall InstallDoPulsePreFrameHook();
	static void __fastcall DoPulsePreFrame(CClientGame* ECX, void* EDX);
	static bool __cdecl CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize);
	static void __fastcall SetFrozen(void *ECX, void *EDX, bool freeze);
	static void __fastcall SetLocked(void* ECX, void* EDX, bool lock);
	static void __fastcall SetEngine(void* ECX, void* EDX, bool status);
};