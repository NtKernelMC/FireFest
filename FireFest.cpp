#include "FireFest.h"
FireFest::HacksData FireFest::hacks;
FireFest::ptrAddProjectile FireFest::AddProjectile = (FireFest::ptrAddProjectile)FUNC_AddProjectile;
void __stdcall FireFest::InitHacks()
{
    thread KeysLoop(KeyChecker); 
    KeysLoop.detach();
    thread ParseIt(PedPoolParser);
    ParseIt.detach();
}
void __stdcall FireFest::KeyChecker(void)
{
    while (true)
    {
        if (GetAsyncKeyState(VK_END))
        {
            if (!hacks.FlareEnabled) MessageBeep(MB_ICONERROR);
            else MessageBeep(MB_ICONASTERISK);
            hacks.FlareEnabled ^= true;
        }
        if (GetAsyncKeyState(VK_DELETE))
        {
            if (!hacks.BombingEnabled) MessageBeep(MB_ICONERROR);
            else MessageBeep(MB_ICONASTERISK);
            hacks.BombingEnabled ^= true;
        }
        if (GetAsyncKeyState(VK_HOME))
        {
            if (!hacks.StingerEnabled) MessageBeep(MB_ICONERROR);
            else MessageBeep(MB_ICONASTERISK);
            hacks.StingerEnabled ^= true;
        }
        Sleep(150);
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
void __stdcall FireFest::PedPoolParser(void)
{ 
    while (true)
    {
        if (!hacks.StingerEnabled && !hacks.BombingEnabled && !hacks.FlareEnabled) continue;
        DWORD pedPoolUsageInfo = *(DWORD*)0xB74490; CVector TargetPos;
        DWORD pedPoolBegining = *(DWORD*)pedPoolUsageInfo;
        DWORD byteMapAddr = *(DWORD*)(pedPoolUsageInfo + 4);
        for (BYTE i = 1; i < 140; i++)
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
                    if (hacks.FlareEnabled)
                    AddProjectile(GetLocalEntity(), WEAPONTYPE_FLARE, TargetPos, 5.0f, &CVector(0.0f, 0.0f, 0.0f), nullptr);
                    if (hacks.BombingEnabled)
                    {
                        CVector direction(0, -180.0f, 0); TargetPos.fZ += 30.0f;
                        AddProjectile(GetLocalEntity(), WEAPONTYPE_FREEFALL_BOMB, TargetPos, 5.0f, &direction, nullptr);
                        Sleep(400);
                    }
                    if (hacks.StingerEnabled)
                    {
                        if (*(BYTE*)(CPed + 0x46C) == 1 || *(DWORD*)(CPed + 0x530) == 50)
                        {
                            DWORD VehEntity = *(DWORD*)(CPed + 0x58C); 
                            CVector direction(0, -180.0f, 0); TargetPos.fZ += 100.0f;
                            AddProjectile(GetLocalEntity(), WEAPONTYPE_ROCKET_HS, TargetPos, 5.0f, &direction, (CEntity*)VehEntity);
                            Sleep(900);
                        }
                    }
                }
            }
        }
        Sleep(hacks.iterationDelay);
    }
}