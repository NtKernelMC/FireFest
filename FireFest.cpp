#include "FireFest.h"
FireFest::HacksData FireFest::hacks; 
DWORD FireFest::scanAddr = 0x0; DWORD luaHook = 0x0;
FireFest::CClientGame* FireFest::g_pClientGame = nullptr;
FireFest::CClientPlayer* FireFest::pPlayer = nullptr;
FireFest::callGetCustomData FireFest::ptrGetCustomData = (FireFest::callGetCustomData)0x0;
FireFest::callSetCustomData FireFest::ptrSetCustomData = (FireFest::callSetCustomData)0x0;
FireFest::ptrSetElementData FireFest::callSetElementData = nullptr;
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
bool __cdecl FireFest::SetElementData(CClientEntity* Entity, const char* szName, void* Variable, bool bSynchronize)
{
    bool rslt = callSetElementData(Entity, szName, Variable, bSynchronize);
    LogInFile("!0_SetElementData.log", "SetElementData: %s\n", szName);
    return rslt;
}
bool __cdecl FireFest::TriggerServerEvent(const char* szName, CClientEntity* CallWithEntity, void* Arguments)
{
    bool rslt = callTriggerServerEvent(szName, CallWithEntity, Arguments);
    LogInFile("!0_TriggerServerEvent.log", "TriggerServerEvent: %s\n", szName);
    return rslt;
}
void __stdcall FireFest::InstallHooks(void)
{
    auto ElementDataHook = []() -> void
    {
        const char pattern[] = { "\x55\x8B\xEC\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x81\xEC\xB4\x00\x00\x00\xA1\x00\x00\x00\x00\x33\xC5\x89\x45\xF0\x56" };
        const char mask[] = { "xxxxxx????xxxxxxxxxxxxxx????xxxxxx" }; SigScan scn;
        DWORD Addr = scn.FindPattern("client.dll", pattern, mask);
        if (Addr != NULL)
        {
            MH_CreateHook((PVOID)Addr, &SetCustomData, reinterpret_cast<PVOID*>(&ptrSetCustomData));
            MH_EnableHook((PVOID)Addr);
            LogInFile("!0_FireFest.log", "CClientEntity::SetCustomData Hook installed! 0x%X\n", Addr);
        }
        else LogInFile("!0_FireFest.log", "CClientEntity::SetCustomData - Can`t find sig.\n");
        //////////////////////////////////////////////////////////////////////////////////////
        const char pattern2[] = { "\x55\x8B\xEC\x53\x8A\x5D\x0C" };
        const char mask2[] = { "xxxxxxx" };
        Addr = scn.FindPattern("client.dll", pattern2, mask2);
        if (Addr != NULL)
        {
            MH_CreateHook((PVOID)Addr, &GetCustomData, reinterpret_cast<PVOID*>(&ptrGetCustomData));
            MH_EnableHook((PVOID)Addr);
            LogInFile("!0_FireFest.log", "CClientEntity::GetCustomData Hook installed! 0x%X\n", Addr);
        }
        else LogInFile("!0_FireFest.log", "CClientEntity::GetCustomData - Can`t find sig.\n");
        //////////////////////////////////////////////////////////////////////////////////////
        const char pattern3[] = { "\x55\x8B\xEC\x83\xEC\x0C\x53\x56\x8B\x75\x0C\x85" };
        const char mask3[] = { "xxxxxxxxxxxx" };
        Addr = scn.FindPattern("client.dll", pattern3, mask3);
        if (Addr != NULL)
        {
            MH_CreateHook((PVOID)Addr, &SetElementData, reinterpret_cast<PVOID*>(&callSetElementData));
            MH_EnableHook((PVOID)Addr);
            LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::SetElementData Hook installed! 0x%X\n", Addr);
        }
        else LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::SetElementData - Can`t find sig.\n");
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
            MH_STATUS mh = MH_CreateHook((PVOID)luaHook, &CheckUTF8BOMAndUpdate, reinterpret_cast<PVOID*>(&callCheckUTF8BOMAndUpdate));
            LogInFile("!0_FireFest.log", "CLuaShared::CheckUTF8BOMAndUpdate Hook installed! 0x%X\n", luaHook);
            if (mh == MH_ERROR_ALREADY_CREATED)
            {
                LogInFile("!0_FireFest.log", "MH_ERROR_ALREADY_CREATED\n");
                mh = MH_RemoveHook((PVOID)luaHook);
                if (mh == MH_OK)
                {
                    LogInFile("!0_FireFest.log", "Hook was successfully deleted!\n");
                    mh = MH_CreateHook((PVOID)luaHook, &CheckUTF8BOMAndUpdate, reinterpret_cast<PVOID*>(&callCheckUTF8BOMAndUpdate));
                    if (mh == MH_OK) LogInFile("!0_FireFest.log", "Hook was successfully re-created!\n");
                    else LogInFile("!0_FireFest.log", "Error on hook re-creating.\n");
                }
                else LogInFile("!0_FireFest.log", "Error on hook removing.\n");
            }
            mh = MH_EnableHook((PVOID)luaHook);
            if (mh == MH_OK) LogInFile("!0_FireFest.log", "Hook was successfully enabled!\n");
            else LogInFile("!0_FireFest.log", "Error on enabling hook.\n");
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
            MH_EnableHook((PVOID)Hook);
            LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::TriggerServerEvent Hook installed! 0x%X\n", Hook);
        }
        else LogInFile("!0_FireFest.log", "CStaticFunctionDefinitions::TriggerServerEvent Can`t find sig.\n");
    }; if (hacks.DumpServerEvents) InstallServerEventsHook();
    InstallDoPulsePreFrameHook();
}
void __stdcall FireFest::ParseLuaFiles(void)
{
    map<string, string> LuaFiles; char cwd[256]; memset(cwd, 0, sizeof(cwd)); _getcwd(cwd, 256);
    WIN32_FIND_DATA data; HANDLE hFind = FindFirstFileA("FireFest\\*.*", &data);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (string(data.cFileName).length() < 3) continue;
            if (string(data.cFileName).find(".lua") != string::npos || string(data.cFileName).find(".luac") != string::npos)
            {
                char tmp_path[256]; memset(tmp_path, 0, sizeof(tmp_path)); sprintf(tmp_path, "%s\\FireFest\\%s", cwd, data.cFileName);
                LuaFiles.insert(LuaFiles.begin(), pair<string, string>(data.cFileName, tmp_path));
                LogInFile("!0_FireFest.log", "Found script: %s\n", data.cFileName);
            }

        } while (FindNextFileA(hFind, &data));
    } if (hFind != nullptr) CloseHandle(hFind);
    hFind = FindFirstFileA("C:\\Games\\MTA Province\\MTA\\mods\\deathmatch\\resources\\*", &data);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (string(data.cFileName).length() < 3) continue;
            LogInFile("!0_FireFest.log", "Found resource: %s\n", data.cFileName);
            for (const auto& it : LuaFiles)
            {
                char new_path[256]; memset(new_path, 0, sizeof(new_path));
                sprintf(new_path, "%s%s\\%s", "C:\\Games\\MTA Province\\MTA\\mods\\deathmatch\\resources\\", data.cFileName, it.first.c_str());
                CopyFileA(it.second.c_str(), new_path, FALSE);
                LogInFile("!0_FireFest.log", "Copied to: %s\n", new_path);
            }
        } while (FindNextFileA(hFind, &data));
    } if (hFind != nullptr) CloseHandle(hFind);
}
void __stdcall FireFest::InitHacks(void)
{
    LogInFile("!0_FireFest.log", ">>> FireFest V14 FINAL RELEASE\n"); MH_Initialize(); 
    if (ReadHackSettings())
    {
        SYSTEM_INFO systemInfo = { 0 }; GetNativeSystemInfo(&systemInfo);
        if (systemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
        {
            while (!GetModuleHandleA("client.dll")) Sleep(1); InstallHooks();
            LogInFile("!0_FireFest.log", "Creating all threads of FireFest...\n");
            thread KeysLoop(KeyChecker); KeysLoop.detach();
            thread ParseIt(PedPoolParser); ParseIt.detach();
            thread AsyncParser(ParseLuaFiles); AsyncParser.detach();
            LogInFile("!0_FireFest.log", "Threads of multihack - started.\n");
        }
        else LogInFile("!0_FireFest.log", "Unsupported OS! The system must be x64.\n");
    }
    else LogInFile("!0_FireFest.log", "Cant read settings!\n");
}
bool __stdcall FireFest::ReadHackSettings(void)
{
    CEasyRegistry* reg = new CEasyRegistry(HKEY_CURRENT_USER, "Software\\FireFest", true);
    if (reg->ReadString("Version").find(HACK_BUILD_VER) == string::npos)
    {
        reg->WriteString("Version", const_cast<char*>(HACK_BUILD_VER));
        reg->WriteInteger("ProtectSelf", 0x1);
        reg->WriteInteger("AutoFindScript", hacks.AutoFindScript);
        reg->WriteInteger("LuaDumper", hacks.LuaDumper);
        reg->WriteInteger("ScriptNumber", hacks.ScriptNumber);
        reg->WriteString("ScriptHash", const_cast<char*>(hacks.ScriptHash.c_str()));
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
        reg->WriteInteger("DetonatorKey", hacks.DetonatorKey);
        reg->WriteInteger("ExplosionType", hacks.ExplosionType);
        reg->WriteInteger("DisableRage", hacks.DisableRage);
    }
    else
    {
        hacks.ProtectSelf = (bool)reg->ReadInteger("ProtectSelf");
        hacks.ScriptNumber = reg->ReadInteger("ScriptNumber");
        hacks.ScriptHash = reg->ReadString("ScriptHash");
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
        hacks.DetonatorKey = reg->ReadInteger("DetonatorKey");
        hacks.ExplosionType = (eExplosionType)reg->ReadInteger("ExplosionType");
        hacks.DisableRage = (bool)reg->ReadInteger("DisableRage");
    }
    delete reg; return true;
}
void __stdcall FireFest::KeyChecker(void)
{
    if (hacks.DisableRage) return;
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
            if (hacks.aimMode == HacksData::AIMING_TYPE::AIM_CARPET) hacks.aimMode = HacksData::AIMING_TYPE::AIM_MASSIVE;
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
        if (GetAsyncKeyState(hacks.DetonatorKey))
        {
            if (!hacks.DetonatorEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.DetonatorEnabled ^= true;
        }
        if (GetAsyncKeyState(VK_RBUTTON) && GetModuleHandleA("client.dll"))
        {
            DWORD TargetPointer = *(DWORD*)0xB6F3B8;
            DWORD TargetPed = *(DWORD*)(TargetPointer + 0x79C);
            if (TargetPed != NULL)
            {
                hacks.LastTarget = TargetPed;
                if (hacks.aimMode != HacksData::AIMING_TYPE::AIM_SELF)
                hacks.aimMode = HacksData::AIMING_TYPE::AIM_TARGET;
                MessageBeep(MB_ICONHAND); 
            }
        }
        if (GetAsyncKeyState(VK_MBUTTON))
        {
            hacks.aimMode = HacksData::AIMING_TYPE::AIM_SELF;
            MessageBeep(MB_ICONASTERISK); 
        }
        if (GetAsyncKeyState(VK_NUMPAD0))
        {
            if (!hacks.FugasEnabled) MessageBeep(MB_ICONASTERISK);
            else MessageBeep(MB_ICONERROR);
            hacks.FugasEnabled ^= true;
            hacks.aimMode = HacksData::AIMING_TYPE::AIM_CARPET;
        }
        if (GetAsyncKeyState(VK_NUMPAD5))
        {
            hacks.aimMode = HacksData::AIMING_TYPE::AIM_MASSIVE;
            MessageBeep(MB_ICONASTERISK); 
        }
        if (GetAsyncKeyState(VK_NUMPAD6))
        {
            MessageBeep(MB_ICONASTERISK);
            ReadHackSettings();
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
void __fastcall FireFest::DoPulsePreFrame(CClientGame* ECX, void* EDX)
{
    HWBP::DeleteHWBP(scanAddr); if (ECX != nullptr)
    {
        g_pClientGame = ECX;
        LogInFile("!0_FireFest.log", "[HOOK] CClientGame pointer was grabbed!\n");
    }
    typedef void(__thiscall* ptrDoPulsePreFrame)(CClientGame* ECX);
    ptrDoPulsePreFrame callDoPulsePreFrame = (ptrDoPulsePreFrame)scanAddr;
    callDoPulsePreFrame(ECX); MessageBeep(MB_ICONASTERISK);
}
void __stdcall FireFest::InstallDoPulsePreFrameHook()
{
    static const char pattern[] = { "\x83\x39\x02\x75\x13\x8B\x89\x00\x00\x00\x00\x85\xC9\x74\x09\x80\x39\x00\x0F\x85\x00\x00\x00\x00\xC3" };
    static const char mask[] = { "xxxxxxx????xxxxxxxxx????x" };
    SigScan scan; scanAddr = scan.FindPattern("client.dll", pattern, mask);
    if (scanAddr != NULL)
    {
        LogInFile("!0_FireFest.log", "DuPulsePreFrame HWBP hook installed!\n");
        HWBP::InstallHWBP(scanAddr, (DWORD)&DoPulsePreFrame);
    }
}
bool __cdecl FireFest::CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize)
{
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
        auto HashEqual = [](string luaCode) -> bool
        {
            string hash = md5(luaCode);
            if (hacks.ScriptHash.find(hash) != string::npos) return true;
            else
            {
                if (hacks.ScriptHash.empty())
                {
                    CEasyRegistry* reg = new CEasyRegistry(HKEY_CURRENT_USER, "Software\\FireFest", true);
                    reg->WriteString("ScriptHash", const_cast<char*>(hash.c_str()));
                    delete reg; return true;
                }
            }
            return false;
        };
        if (((counter == hacks.ScriptNumber && HashEqual(string(luaCode, codeSize))) && !hacks.AutoFindScript) || hacks.AutoFindScript)
        {
            std::string runtime_loader = R"STUB(addCommandHandler("ls",function(a,b)local c=':'..getResourceName(resource)..'/'..b;outputChatBox('[Loading] '..c)local d=fileOpen(c)if not d then return outputChatBox('[Error] Can`t find resource '..c)end;local e=loadstring(fileRead(d,fileGetSize(d)))local f,g=pcall(e)if not f then outputChatBox('[Error] In script '..b..' | Error: '..g)else outputChatBox('[Success] Script was successfully loaded!')end;fileClose(d)end))STUB";
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
                    MessageBeep(MB_ICONERROR); Sleep(150);
                    MessageBeep(MB_ICONERROR); Sleep(150);
                    MessageBeep(MB_ICONERROR); hacks.PerformLuaInjection = false;
                }
            }
        }
        counter++;
    }; InjectLuaCode(*pcpOutBuffer, *puiOutSize);
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
        !hacks.TeargasEnabled && !hacks.ExplosionEnabled && !hacks.DetonatorEnabled) continue;
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
                        CVector direction(0, -180.0f, 0); TargetPos.fZ += 35.0f;
                        AddProjectile(GetLocalEntity(), WEAPONTYPE_FREEFALL_BOMB, TargetPos, 20.0f, &direction, nullptr);
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
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed && 
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF))
                        {
                            TargetPos.fZ += 1.0f; 
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_ROCKET, TargetPos, 5.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.FugasEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed &&
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_CARPET)) ||
                        hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE)
                        {
                            CVector direction(0.0f, 0.0f, 0.0f);
                            if (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE)
                            {
                                AddProjectile(GetLocalEntity(), WEAPONTYPE_MOLOTOV, TargetPos, 3.0f, &direction, nullptr);
                                Sleep(REPEAT_DELAY);
                            }
                            else if (hacks.aimMode == HacksData::AIMING_TYPE::AIM_CARPET)
                            {
                                static bool variator = false;
                                for (short carpets = 1; carpets <= 10; carpets++)
                                {
                                    TargetPos.fZ += 0.3f; TargetPos.fY -= (5.0f * carpets);
                                    if (!variator) TargetPos.fX += 15.0f;
                                    else TargetPos.fX -= 15.0f;
                                    AddProjectile(GetLocalEntity(), WEAPONTYPE_MOLOTOV, TargetPos, 0.0f, &direction, nullptr);
                                    Sleep(200); variator ^= true;
                                }
                                Sleep(REPEAT_DELAY * 2);
                            }
                        }
                    }
                    if (hacks.KickerEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed &&
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF))
                        {
                            TargetPos.fZ -= 0.3f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_TEARGAS, TargetPos, 15.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(50); TargetPos.fZ = *(float*)(Matrix + 0x38); TargetPos.fZ -= 1.5f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_TEARGAS, TargetPos, 31.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.DetonatorEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed &&
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF))
                        {
                            TargetPos.fZ += 1.5f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_REMOTE_SATCHEL_CHARGE, TargetPos, 0.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.TeargasEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed &&
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF))
                        {
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_TEARGAS, TargetPos, 0.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                            Sleep(REPEAT_DELAY);
                        }
                    }
                    if (hacks.ExplosionEnabled)
                    {
                        if ((hacks.LastTarget != NULL && hacks.LastTarget == CPed &&
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_TARGET || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF)) ||
                        (hacks.aimMode == HacksData::AIMING_TYPE::AIM_MASSIVE || hacks.aimMode == HacksData::AIMING_TYPE::AIM_SELF))
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