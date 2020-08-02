#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <map>
#include <vector>
#include <ctime>
#include <direct.h>
#include "CVector.h"
#include "WepTypes.h"
#include "sigscan.h"
#include "HWBP.h"
#include "Registry.h"
#include "MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")
using namespace std;
typedef long CIMTYPE;
#define FUNC_AddProjectile 0x737C80
#define LOCAL_CPED 0xB6F5F0
#define HACK_BUILD_VER "1411"
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
	typedef void CLuaFunctionRef;
	typedef void CClientGame;
	static CClientGame* g_pClientGame;
	typedef void CEntity;
	typedef void CPed;
	typedef void CClientPlayer;
	typedef void CClientEntity;
	typedef void CLuaMain;
	static CClientPlayer* pPlayer;
	typedef bool(__cdecl* ptrTriggerServerEvent)(const char* szName, CClientEntity* CallWithEntity, void* Arguments);
	static ptrTriggerServerEvent callTriggerServerEvent;
	typedef bool (__cdecl* ptrAddEventHandler)(CLuaMain* LuaMain, const char* szName, CClientEntity* Entity,
	const CLuaFunctionRef* iLuaFunction, bool bPropagated, DWORD eventPriority, float fPriorityMod);
	static ptrAddEventHandler callAddEventHandler;
	typedef void* (__thiscall *callGetCustomData)(CClientEntity* ECX, const char* szName, bool bInheritData, bool* pbIsSynced);
	static callGetCustomData ptrGetCustomData;
	typedef void (__thiscall* callSetCustomData)(void *ECX, const char* szName, void* Variable, bool bSynchronized);
	static callSetCustomData ptrSetCustomData;
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
			AIM_TARGET = 2,
			AIM_SELF = 3,
			AIM_CARPET = 4
		};
		CVector CamPos;
		AIMING_TYPE aimMode;
		DWORD ScriptNumber;
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
		bool ElemDumper;
		bool EventDisabler;
		bool DumpServerEvents;
		bool AutoFindScript;
		DWORD FlareKey, BombKey, StingerKey, MisleadKey, KickerKey, FugasKey, TeargasKey, ExplodeKey;
		DWORD iterationDelay;
		DWORD LastTarget;
		bool ProtectSelf;
		string lua_code;
		eExplosionType ExplosionType;
		bool PerformLuaInjection;
		HacksData()
		{
			AntiLock = false; AntiFreeze = false; AntiKeys = false; ElemDumper = false; bool ProtectSelf = true;
			aimMode = AIMING_TYPE::AIM_MASSIVE; ExplosionType = EXP_TYPE_TANK; AutoFindScript = true; DumpServerEvents = false;
			LastTarget = 0x0; LuaDumper = false; PerformLuaInjection = false; ScriptNumber = 0x1; EventDisabler = false;
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
	static void __stdcall LogInFile(std::string log_name, const char* log, ...);
	static bool __stdcall IsDirectoryExists(const std::string& dirName_in);
	static void __stdcall InitHacks();
	static void __stdcall KeyChecker(void);
	static void __stdcall PedPoolParser(void);
	static bool __stdcall IsFlashLimitReached(USHORT *count);
	static CEntity* __stdcall GetLocalEntity(void);
	static CVector __stdcall GetMyOwnPos(void);
	static CClientPlayer* GetClosestRemotePlayer(const CVector& vecTemp, float fMaxDistance);
	static void __stdcall InstallDoPulsePreFrameHook();
	static void __fastcall DoPulsePreFrame(CClientGame* ECX, void* EDX);
	static bool __cdecl CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize);
	static void __fastcall SetFrozen(void *ECX, void *EDX, bool freeze);
	static void __fastcall SetLocked(void* ECX, void* EDX, bool lock);
	static void __fastcall SetEngine(void* ECX, void* EDX, bool status);
	static void* __fastcall GetCustomData(CClientEntity* ECX, void *EDX, const char* szName, bool bInheritData, bool* pbIsSynced);
	static void __fastcall SetCustomData(CClientEntity* ECX, void* EDX, const char* szName, void* Variable, bool bSynchronized = true);
	static bool __cdecl AddEventHandler(CLuaMain* LuaMain, const char* szName, CClientEntity* Entity,
	const CLuaFunctionRef* iLuaFunction, bool bPropagated, DWORD eventPriority, float fPriorityMod);
	static bool __cdecl TriggerServerEvent(const char* szName, CClientEntity* CallWithEntity, void* Arguments);
};