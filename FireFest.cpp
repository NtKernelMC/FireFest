#include "FireFest.h"
FireFest::HacksData FireFest::hacks;
DWORD FireFest::scanAddr = 0x0; DWORD luaHook = 0x0;
FireFest::CClientGame* FireFest::g_pClientGame = nullptr;
FireFest::CClientPlayer* FireFest::pPlayer = nullptr;
FireFest::ptrAddEventHandler FireFest::callAddEventHandler = nullptr;
FireFest::callSetFrozen FireFest::pSetFrozen = (FireFest::callSetFrozen)0x0;
FireFest::callSetLocked FireFest::pSetLocked = (FireFest::callSetLocked)0x0;
FireFest::callSetEngine FireFest::pSetEngine = (FireFest::callSetEngine)0x0;
FireFest::callGetCustomData FireFest::ptrGetCustomData = (FireFest::callGetCustomData)0x0;
FireFest::callSetCustomData FireFest::ptrSetCustomData = (FireFest::callSetCustomData)0x0;
FireFest::ptrAddProjectile FireFest::AddProjectile = (FireFest::ptrAddProjectile)FUNC_AddProjectile;
typedef bool(__cdecl* ptrCheckUTF8BOMAndUpdate)(char** pcpOutBuffer, unsigned int* puiOutSize);
ptrCheckUTF8BOMAndUpdate callCheckUTF8BOMAndUpdate = (ptrCheckUTF8BOMAndUpdate)0x0;
FireFest::ptrTriggerServerEvent FireFest::callTriggerServerEvent = (FireFest::ptrTriggerServerEvent)0x0;
bool __stdcall FireFest::IsDirectoryExists(const std::string& dirName_in)
{
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
};
void __stdcall FireFest::LogInFile(std::string log_name, const char* log, ...)
{
    char hack_dir[356]; memset(hack_dir, 0, sizeof(hack_dir));
    _getcwd(hack_dir, 356); strcat(hack_dir, "\\FireFest");
    if (!IsDirectoryExists(hack_dir)) CreateDirectoryA(hack_dir, NULL);
    char new_dir[500]; memset(new_dir, 0, sizeof(new_dir));
    sprintf(new_dir, "%s\\%s", hack_dir, log_name.c_str());
    static bool once = false; if (!once) 
    { 
        FILE* hFile = fopen(new_dir, "rb");
        if (hFile) { fclose(hFile); DeleteFileA(new_dir); }
        once = true;
    }
    FILE* hFile = fopen(new_dir, "a+");
    if (hFile)
    {
        time_t t = std::time(0); tm* now = std::localtime(&t);
        char tmp_stamp[500]; memset(tmp_stamp, 0, sizeof(tmp_stamp));
        sprintf(tmp_stamp, "[%d:%d:%d]", now->tm_hour, now->tm_min, now->tm_sec);
        strcat(tmp_stamp, std::string(" " + std::string(log)).c_str());
        va_list arglist; va_start(arglist, log); vfprintf(hFile, tmp_stamp, arglist);
        va_end(arglist); fclose(hFile);
    }
}
void* __fastcall FireFest::GetCustomData(CClientEntity* ECX, void* EDX, const char* szName, bool bInheritData, bool* pbIsSynced)
{
    void* rslt = ptrGetCustomData(ECX, szName, bInheritData, pbIsSynced);
    LogInFile("!0_GetCustomData.log", "GetCustomData: %s\n", szName);
    return rslt;
}
void __fastcall FireFest::SetCustomData(CClientEntity* ECX, void* EDX, const char* szName, void* Variable, bool bSynchronized)
{
    ptrSetCustomData(ECX, szName, Variable, bSynchronized);
    LogInFile("!0_SetCustomData.log", "SetCustomData: %s\n", szName);
}
void __fastcall FireFest::SetFrozen(void* ECX, void* EDX, bool freeze)
{
    pSetFrozen(ECX, false);
}
void __fastcall FireFest::SetLocked(void* ECX, void* EDX, bool lock)
{
    pSetLocked(ECX, false);
}
void __fastcall FireFest::SetEngine(void* ECX, void* EDX, bool status)
{
    pSetEngine(ECX, true);
}
bool __cdecl FireFest::TriggerServerEvent(const char* szName, CClientEntity* CallWithEntity, void* Arguments)
{
    bool rslt = callTriggerServerEvent(szName, CallWithEntity, Arguments);
    LogInFile("!0_TriggerServerEvent.log", "TriggerServerEvent: %s\n", szName);
    return rslt;
}
bool __cdecl FireFest::AddEventHandler(CLuaMain* LuaMain, const char* szName, CClientEntity* Entity,
const CLuaFunctionRef* iLuaFunction, bool bPropagated, DWORD eventPriority, float fPriorityMod)
{
    static const std::map<std::string, bool> TargetEvents =
    { { "onClientElementDataChange", true }, { "onClientExplosion", true }, { "onClientProjectileCreation", true }, { "onClientPlayerDamage", true },
      { "onClientWeaponFire", false }, { "onClientVehicleExplode", false }, { "onClientPlayerWasted", false }, { "onClientPlayerVehicleEnter", false },
      { "onClientPlayerWeaponFire", false}, { "onClientPlayerSpawn", false }, { "onClientPlayerQuit", false }, { "onClientPlayerJoin", false },
      { "onClientPlayerPickupHit", false }, { "onClientPlayerPickupLeave", false },  { "onClientPickupHit", false }, { "onClientPlayerTarget", false },
      { "onClientPickupLeave", false }, { "onClientPedWeaponFire", false }, { "onClientPedDamage", false }, { "onClientPedWasted", false },
      { "onClientMarkerLeave", false }, { "onClientMarkerHit", false } };
    for (const auto& it : TargetEvents)
    {
        if (it.first.find(szName) != std::string::npos && it.second)
        {
            LogInFile("!0_AddEventHandler.log", "AddEventHandler: %s blocked.\n", szName);
            return true;
        }
    }
    return callAddEventHandler(LuaMain, szName, Entity, iLuaFunction, bPropagated, eventPriority, fPriorityMod);
}
void __stdcall FireFest::InitHacks()
{
    MH_Initialize(); 
    while (!GetModuleHandleA("client.dll")) { Sleep(10); }
    auto ReadHackSettings = []() -> bool
    {
        CEasyRegistry* reg = new CEasyRegistry(HKEY_CURRENT_USER, "Software\\FireFest", true);
        if (reg->ReadString("Version").find(HACK_BUILD_VER) == string::npos)
        {
            reg->WriteString("Version", const_cast<char*>(HACK_BUILD_VER));
            reg->WriteInteger("ProtectSelf", 0x1);
            reg->WriteInteger("AutoFindScript", hacks.AutoFindScript);
            reg->WriteInteger("LuaDumper", hacks.LuaDumper);
            reg->WriteInteger("ScriptNumber", hacks.ScriptNumber); 
            reg->WriteInteger("PerformLuaInjection", hacks.PerformLuaInjection);
            reg->WriteInteger("ElemDumper", hacks.ElemDumper);
            reg->WriteInteger("DumpServerEvents", hacks.DumpServerEvents);
            reg->WriteInteger("AimMode", (DWORD)hacks.aimMode);
            reg->WriteInteger("RepeatDelay", REPEAT_DELAY);
            reg->WriteInteger("FlareKey", hacks.FlareKey);
            reg->WriteInteger("BombKey", hacks.BombKey);
            reg->WriteInteger("StingerKey", hacks.StingerKey);
            reg->WriteInteger("MisleadKey", hacks.MisleadKey);
            reg->WriteInteger("KickerKey", hacks.KickerKey);
            reg->WriteInteger("FugasKey", hacks.FugasKey);
            reg->WriteInteger("TeargasKey", hacks.TeargasKey);
            reg->WriteInteger("ExplodeKey", hacks.ExplodeKey);
            reg->WriteInteger("ExplosionType", hacks.ExplosionType);
            reg->WriteInteger("AntiVehicleFreeze", hacks.AntiFreeze);
            reg->WriteInteger("AntiDoorsLock", hacks.AntiLock);
            reg->WriteInteger("AntiKeys", hacks.AntiKeys);
            reg->WriteInteger("EventDisabler", hacks.EventDisabler);
        }
        else
        {
            hacks.ProtectSelf = (bool)reg->ReadInteger("ProtectSelf");
            hacks.ScriptNumber = reg->ReadInteger("ScriptNumber");
            hacks.LuaDumper = (bool)reg->ReadInteger("LuaDumper");
            hacks.PerformLuaInjection = (bool)reg->ReadInteger("PerformLuaInjection");
            hacks.AutoFindScript = (bool)reg->ReadInteger("AutoFindScript");
            hacks.ElemDumper = (bool)reg->ReadInteger("ElemDumper");
            hacks.DumpServerEvents = (bool)reg->ReadInteger("DumpServerEvents");
            hacks.aimMode = (HacksData::AIMING_TYPE)reg->ReadInteger("AimMode");
            REPEAT_DELAY = reg->ReadInteger("RepeatDelay");
            hacks.FlareKey = reg->ReadInteger("FlareKey");
            hacks.BombKey = reg->ReadInteger("BombKey");
            hacks.StingerKey = reg->ReadInteger("StingerKey");
            hacks.MisleadKey = reg->ReadInteger("MisleadKey");
            hacks.KickerKey = reg->ReadInteger("KickerKey");
            hacks.FugasKey = reg->ReadInteger("FugasKey");
            hacks.TeargasKey = reg->ReadInteger("TeargasKey");
            hacks.ExplodeKey = reg->ReadInteger("ExplodeKey");
            hacks.ExplosionType = (eExplosionType)reg->ReadInteger("ExplosionType");
            hacks.AntiFreeze = (bool)reg->ReadInteger("AntiVehicleFreeze");
            hacks.AntiLock = (bool)reg->ReadInteger("AntiDoorsLock");
            hacks.AntiKeys = (bool)reg->ReadInteger("AntiKeys");
            hacks.EventDisabler = (bool)reg->ReadInteger("EventDisabler");
        }
        delete reg; return true;
    }; if (ReadHackSettings())
    {
        auto AddEventHandlerHook = []() -> void
        {
            const char pattern[] = { "\x55\x8B\xEC\x56\x8B\x75\x0C\x85\xF6\x75\x06\x89\x35\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x56\xE8\x00\x00\x00\x00\x85\xC0\x74\x29" };
            const char mask[] = { "xxxxxxxxxxxxxxxxxx?????xx????xxxx" };
            SigScan scn; DWORD Addr = scn.FindPattern("client.dll", pattern, mask);
            if (Addr != NULL)
            {
                MH_CreateHook((PVOID)Addr, &AddEventHandler, reinterpret_cast<PVOID*>(&callAddEventHandler));
                LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::AddEventHandler Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::AddEventHandler - Can`t find sig.\n");
        };
        if (hacks.EventDisabler) AddEventHandlerHook();
        auto ElementDataHook = []() -> void
        {
            const char pattern[] = { "\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\xB4\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x56" };
            const char mask[] = { "xxxxxx????xxxxxxxxxxxxxx????xxxxxx" }; SigScan scn;
            DWORD Addr = scn.FindPattern("client.dll", pattern, mask);
            if (Addr != NULL)
            {
                MH_CreateHook((PVOID)Addr, &GetCustomData, reinterpret_cast<PVOID*>(&ptrSetCustomData));
                LogInFile("!0_FireFest.log", "CClientEntity::SetCustomData Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CClientEntity::SetCustomData - Can`t find sig.\n");
            //////////////////////////////////////////////////////////////////////////////////////
            const char pattern2[] = { "\x55\x8B\xEC\x53\x8A\x5D\x0C" };
            const char mask2[] = { "xxxxxxx" };
            Addr = scn.FindPattern("client.dll", pattern2, mask2);
            if (Addr != NULL)
            {
                MH_CreateHook((PVOID)Addr, &GetCustomData, reinterpret_cast<PVOID*>(&ptrGetCustomData));
                LogInFile("!0_FireFest.log", "CClientEntity::GetCustomData Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CClientEntity::GetCustomData - Can`t find sig.\n");
        };
        if (hacks.ElemDumper) ElementDataHook();
        auto InstallLuaHook = []()
        {
            const char pattern[] = { "\x55\x8B\xEC\x56\x8B\x75\x0C\x57\x8B\x7D\x08\xFF\x36\xFF\x37\xE8\x00\x00\x00\x00\x83\xC4\x08\x84\xC0\x74\x0C\x83\x07\x03\xB0\x01\x83\x06\xFD\x5F\x5E\x5D\xC3" };
            const char mask[] = { "xxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxx" }; SigScan scan;
            luaHook = scan.FindPattern("client.dll", pattern, mask);
            if (luaHook != NULL)
            {
                callCheckUTF8BOMAndUpdate = (ptrCheckUTF8BOMAndUpdate)luaHook;
                MH_CreateHook((PVOID)luaHook, &CheckUTF8BOMAndUpdate, reinterpret_cast<PVOID*>(&callCheckUTF8BOMAndUpdate));
                LogInFile("!0_FireFest.log", "CLuaShared::CheckUTF8BOMAndUpdate Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CLuaShared::CheckUTF8BOMAndUpdate Can`t find sig.\n");
        }; if (hacks.PerformLuaInjection == 0x1 || hacks.LuaDumper) InstallLuaHook(); 
        auto InstallServerEventsHook = []()
        {
            const char pattern[] = { "\x55\x8B\xEC\x51\x53\x56\x57\x8B\x7D\x08\x85" };
            const char mask[] = { "xxxxxxxxxxx" }; SigScan scn;
            DWORD Hook = scn.FindPattern("client.dll", pattern, mask);
            if (Hook != NULL)
            {
                callCheckUTF8BOMAndUpdate = (ptrCheckUTF8BOMAndUpdate)Hook;
                MH_CreateHook((PVOID)Hook, &TriggerServerEvent, reinterpret_cast<PVOID*>(&callTriggerServerEvent));
                LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::TriggerServerEvent Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::TriggerServerEvent Can`t find sig.\n");
        }; if (hacks.DumpServerEvents) InstallServerEventsHook();
        auto PatchLocker = []()
        {
            static const char pattern[] = { "\x55\x8B\xEC\x56\x8B\xF1\x8B\x8E\xA4\x01\x00\x00\x85\xC9\x74\x19\x8B\x01\x53\x8B\x5D\x08\x53\xFF\x90\x94" };
            static const char mask[] = { "xxxxxxxxxxxxxxxxxxxxxxxxxx" };
            static SigScan scn; static DWORD Locker = scn.FindPattern("client.dll", pattern, mask);
            if (Locker != NULL)
            {
                MH_CreateHook((PVOID)Locker, &SetLocked, reinterpret_cast<PVOID*>(&pSetLocked));
                LogInFile("!0_FireFest.log", "CClientVehicle::SetLocked Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CClientVehicle::SetLocked - Can`t find sig.\n");
        }; if (hacks.AntiLock) PatchLocker();
        auto PatchFreezer = []()
        {
            static const char pattern[] = { "\x55\x8B\xEC\x8A\x45\x08\x83" };
            static const char mask[] = { "xxxxxxx" };
            static SigScan scn; static DWORD Frozen = scn.FindPattern("client.dll", pattern, mask);
            if (Frozen != NULL)
            {
                MH_CreateHook((PVOID)Frozen, &SetFrozen, reinterpret_cast<PVOID*>(&pSetFrozen));
                LogInFile("!0_FireFest.log", "CClientVehicle::SetFrozen Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CClientVehicle::SetFrozen - Can`t find sig.\n");
        }; if (hacks.AntiFreeze) PatchFreezer();
        auto PatchEngine = []()
        {
            static const char pattern[] = { "\x55\x8B\xEC\x56\x8B\xF1\x8B\x8E\xA4\x01\x00\x00\x85\xC9\x74\x19\x8B\x01\x53\x8B\x5D\x08\x53\xFF\x90\xCC" };
            static const char mask[] = { "xxxxxxxxxxxxxxxxxxxxxxxxxx" };
            static SigScan scn; static DWORD Engine = scn.FindPattern("client.dll", pattern, mask);
            if (Engine != NULL)
            {
                MH_CreateHook((PVOID)Engine, &SetEngine, reinterpret_cast<PVOID*>(&pSetEngine));
                LogInFile("!0_FireFest.log", "CClientVehicle::SetEngineStatus Hook installed!\n");
            }
            else LogInFile("!0_FireFest.log", "CClientVehicle::SetEngineStatus - Can`t find sig.\n");
        }; if (hacks.AntiKeys) PatchEngine();
        MH_EnableHook(MH_ALL_HOOKS); InstallDoPulsePreFrameHook();
        thread KeysLoop(KeyChecker); KeysLoop.detach();
        thread ParseIt(PedPoolParser); ParseIt.detach();
    }
    else LogInFile("!0_FireFest.log", "Cant read settings!\n");
}
void __stdcall FireFest::KeyChecker(void)
{
    while (true)
    {
        if (GetAsyncKeyState(hacks.FlareKey))
        {
            if (!hacks.FlareEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.FlareEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.BombKey))
        {
            if (!hacks.BombingEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.BombingEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.StingerKey))
        {
            if (!hacks.StingerEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.StingerEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.MisleadKey))
        {
            if (!hacks.MisleadEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.MisleadEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.KickerKey))
        {
            if (!hacks.KickerEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.KickerEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.FugasKey))
        {
            if (!hacks.FugasEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.FugasEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.TeargasKey))
        {
            if (!hacks.TeargasEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.TeargasEnabled ^= true;
        }
        if (GetAsyncKeyState(hacks.ExplodeKey))
        {
            if (!hacks.ExplosionEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.ExplosionEnabled ^= true;
        }
        if (GetAsyncKeyState(VK_RBUTTON))
        {
            DWORD TargetPointer = *(DWORD*)0xB6F3B8;
            DWORD TargetPed = *(DWORD*)(TargetPointer + 0x79C);
            if (TargetPed != NULL)
            {
                hacks.LastTarget = TargetPed;
                hacks.aimMode = HacksData::AIMING_TYPE::AIM_TARGET;
                MessageBeep(MB_ICONHAND); Sleep(1500);
            }
        }
        if (GetAsyncKeyState(VK_MBUTTON))
        {
            hacks.aimMode = HacksData::AIMING_TYPE::AIM_SELF;
            MessageBeep(MB_ICONASTERISK); Sleep(1500);
        }
        Sleep(350);
    }
}
FireFest::CEntity* __stdcall FireFest::GetLocalEntity(void)
{
    return ((CEntity*(__cdecl*)(int))0x56E120)(-1);
}
CVector __stdcall FireFest::GetMyOwnPos(void)
{
    DWORD PEDSELF = *(DWORD*)LOCAL_CPED; CVector MyPos;
    DWORD MyMatrix = *(DWORD*)(PEDSELF + 0x14);
    MyPos.fX = *(float*)(MyMatrix + 0x30);
    MyPos.fY = *(float*)(MyMatrix + 0x34);
    MyPos.fZ = *(float*)(MyMatrix + 0x38);
    return MyPos;
}
FireFest::CClientPlayer* FireFest::GetClosestRemotePlayer(const CVector& vecTemp, float fMaxDistance)
{
    typedef CClientPlayer* (__thiscall* pGetClosestRemotePlayer)(void* ECX, const CVector& vecTemp, float fMaxDistance);
    static const char pattern[] = { "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x1C\x8B" };
    static const char mask[] = { "xxxxxxxxxx" };
    static SigScan scan; static DWORD scannedAddress = scan.FindPattern("client.dll", pattern, mask);
    static pGetClosestRemotePlayer callGetClosestRemotePlayer = (pGetClosestRemotePlayer)scannedAddress;
    if (g_pClientGame == nullptr || scannedAddress == NULL) return nullptr;
    return callGetClosestRemotePlayer(g_pClientGame, vecTemp, fMaxDistance);
}
void __fastcall FireFest::DoPulsePreFrame(CClientGame* ECX, void* EDX)
{
    HWBP::DeleteHWBP(scanAddr); g_pClientGame = ECX;
    typedef void(__thiscall* ptrDoPulsePreFrame)(CClientGame* ECX);
    ptrDoPulsePreFrame callDoPulsePreFrame = (ptrDoPulsePreFrame)scanAddr;
    callDoPulsePreFrame(ECX); MessageBeep(MB_ICONERROR);
}
void __stdcall FireFest::InstallDoPulsePreFrameHook()
{
    static const char pattern[] = { "\x83\x39\x02\x75\x13\x8B\x89\x00\x00\x00\x00\x85\xC9\x74\x09\x80\x39\x00\x0F\x85\x00\x00\x00\x00\xC3" };
    static const char mask[] = { "xxxxxxx????xxxxxxxxx????x" };
    SigScan scan; scanAddr = scan.FindPattern("client.dll", pattern, mask);
    if (scanAddr != NULL) HWBP::InstallHWBP(scanAddr, (DWORD)&DoPulsePreFrame);
}
bool __cdecl FireFest::CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize)
{
    auto DisableLuaHook = []()
    {
        MH_DisableHook((PVOID)luaHook);
        MH_RemoveHook((PVOID)luaHook);
    };
    bool rslt = callCheckUTF8BOMAndUpdate(pcpOutBuffer, puiOutSize);
    auto InjectLuaCode = [](char* luaCode, SIZE_T codeSize)
    {
        static int counter = 0x1;
        if (hacks.LuaDumper)
        {
            char hack_dir[256]; memset(hack_dir, 0, sizeof(hack_dir));
            _getcwd(hack_dir, 256); strcat(hack_dir, "\\FireFest\\LuaDumper");
            if (!IsDirectoryExists(hack_dir)) CreateDirectoryA(hack_dir, NULL);
            char fname[35]; memset(fname, 0, sizeof(fname));
            sprintf(fname, "\\script_%d.lua", counter);
            strcat(hack_dir, fname); FILE* hFile = fopen(hack_dir, "wb");
            fwrite(luaCode, 1, codeSize, hFile); fclose(hFile);
        }
        if ((counter == hacks.ScriptNumber && !hacks.AutoFindScript) || hacks.AutoFindScript)
        {
            std::string runtime_loader = R"(function RuntimeInject(path)
                local sThisResourceName = getResourceName( getThisResource() ) 
                local sPathToFile = ':'..sThisResourceName..'/'..path
                outputChatBox('[Loading] ' .. sPathToFile)
                local opened = fileOpen(sPathToFile)
                if not opened then
                    outputChatBox('[Error] Can`t find resource ' .. sPathToFile)
                    return false 
                end
                local count = fileGetSize(opened)
                local data = fileRead(opened, count)
                fileClose(opened)
                local execute_script = loadstring(data)
                local rslt, errorMsg = pcall(execute_script)
                if not rslt then
                    outputChatBox('[Error] In script ' .. path .. ' | Resource: ' .. errorMsg)
                else 
                    outputChatBox('[Success] Script was successfully loaded!')
                end
            end

        addCommandHandler("loadscript", function(cmd, fileName)
            RuntimeInject(fileName)
        end))";
            if (codeSize >= runtime_loader.length())
            {
                if (!hacks.AutoFindScript)
                {
                    memset(luaCode, 0, codeSize);
                    strcat(luaCode, runtime_loader.c_str());
                }
                else
                {
                    char scr_finder[155]; memset(scr_finder, 0, sizeof(scr_finder));
                    sprintf(scr_finder, "outputChatBox(\"Injectectable script with number %d | size %d | required size %d\")", 
                    counter, codeSize, runtime_loader.length());
                    memset(luaCode, 0, codeSize);
                    strcat(luaCode, scr_finder);
                }
            }
            else 
            {
                if (!hacks.AutoFindScript)
                {
                    MessageBeep(MB_ICONERROR);
                    MessageBoxA(0, "Choosen lua script is too small!", "Error", MB_ICONERROR | MB_OK);
                    hacks.PerformLuaInjection = false;
                }
            }
        }
        counter++;
    }; InjectLuaCode(*pcpOutBuffer, *puiOutSize);
    if (!hacks.LuaDumper && !hacks.PerformLuaInjection) DisableLuaHook();
    return rslt;
}
void __stdcall FlashBlocker(USHORT* count)
{
    if (count == nullptr) return;
    Sleep(6000); *count = 1;
}
bool __stdcall FireFest::IsFlashLimitReached(USHORT *count)
{
    if (count == nullptr) return true;
    if (*count <= 5) return false;
    else
    {
        std::thread BreakFlashLimit(FlashBlocker, count);
        BreakFlashLimit.detach();
    }
    return true;
}
void __stdcall FireFest::PedPoolParser(void)
{ 
    while (true)
    {
        if (!hacks.StingerEnabled && !hacks.BombingEnabled && !hacks.FlareEnabled && 
        !hacks.MisleadEnabled && !hacks.KickerEnabled && !hacks.FugasEnabled && 
        !hacks.TeargasEnabled && !hacks.ExplosionEnabled) continue;
        DWORD pedPoolUsageInfo = *(DWORD*)0xB74490; CVector TargetPos;
        DWORD pedPoolBegining = *(DWORD*)pedPoolUsageInfo;
        DWORD byteMapAddr = *(DWORD*)(pedPoolUsageInfo + 4);
        for (BYTE i = (BYTE)hacks.ProtectSelf; i < 140; i++)
        {
            BYTE activityStatus = *(BYTE*)(byteMapAddr + i);
            if (activityStatus > 0 && activityStatus < 128)
            {
                DWORD CPed = (pedPoolBegining + i * 1988);
                DWORD Matrix = *(DWORD*)(CPed + 0x14);
                TargetPos.fX = *(float*)(Matrix + 0x30);
                TargetPos.fY = *(float*)(Matrix + 0x34);
                TargetPos.fZ = *(float*)(Matrix + 0x38);
                float health = *(float*)(CPed + 0x540);
                if (health > 0.f)
                {
                    if (hacks.BombingEnabled)
                    {
                        CVector direction(0, -180.0f, 0); TargetPos.fZ += 10.0f;
                        AddProjectile(GetLocalEntity(), WEAPONTYPE_FREEFALL_BOMB, TargetPos, 30.0f, &direction, nullptr);
                        Sleep(REPEAT_DELAY);
                    }
                    if (hacks.StingerEnabled)
                    {
                        if (*(BYTE*)(CPed + 0x46C) == 1 || *(DWORD*)(CPed + 0x530) == 50)
                        {
                            DWORD VehEntity = *(DWORD*)(CPed + 0x58C); 
                            CVector direction(0, -180.0f, 0); TargetPos.fZ += 100.0f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_ROCKET_HS, TargetPos, 5.0f, &direction, (CEntity*)VehEntity);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.FlareEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)
                        {
                            if (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET)
                            {
                                AddProjectile(GetLocalEntity(), WEAPONTYPE_FLARE, TargetPos, 5.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                                Sleep(REPEAT_DELAY);
                            }
                            else if (hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)
                            {
                                static USHORT flashes = 1;
                                if (!IsFlashLimitReached(&flashes))
                                {
                                    AddProjectile(GetLocalEntity(), WEAPONTYPE_FLARE, GetMyOwnPos(), 5.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                                    flashes++;
                                }
                                Sleep(500);
                            }
                        }
                    }
                    if (hacks.MisleadEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE))
                        {
                            TargetPos.fZ += 1.0f; 
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_ROCKET, TargetPos, 5.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.FugasEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE))
                        {
                            CVector direction(0.0f, 0.0f, 0.0f);
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_MOLOTOV, TargetPos, 3.0f, &direction, nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.KickerEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE))
                        {
                            TargetPos.fZ -= 0.3f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_TEARGAS, TargetPos, 15.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(300);
                        }
                    }
                    if (hacks.TeargasEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE))
                        {
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_TEARGAS, TargetPos, 0.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.ExplosionEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE))
                        {
                            static char pattern[] = { "\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x83\xEC\x28\x56\x57\xA1\x00\x00\x00\x00\x33\xC5\x50\x8D\x45\xF4\x64\xA3\x00\x00\x00\x00\xC7" };
                            static char mask[] = { "xxxxxx????xxxxxxxxxxxxx????xxxxxxxxxxxxx" };
                            static SigScan scan2; static DWORD scanAddr2 = scan2.FindPattern((char*)"client.dll", pattern, mask);
                            if (scanAddr2 != NULL)
                            {
                                ((void(__thiscall*)(void*, const CVector&, eExplosionType, void*))scanAddr2)
                                ((void*)g_pClientGame, TargetPos, hacks.ExplosionType, nullptr);
                                MessageBeep(MB_ICONERROR);
                            }
                            Sleep(REPEAT_DELAY);
                        }
                    }
                }
            }
        }
        Sleep(hacks.iterationDelay);
    }
}