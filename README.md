# OffsetDumper

This program update offsets for any game by searching for offsets, you only need to make config for your game.
I already made examples for two games: deceit and apex.


//////////////////////////////////////////////////////////////////////////
SaveFileName: Dumps\DeceitOffset.h
ProcessName: Deceit.exe


enum game_data : int32
{
	a_console = 0xFindPattern(Deceit.exe,F3 0F 58 C3 8B C1,address,10,0) , // ‬cl_fov
	PhysicalWorld = 0xFindPattern(Deceit.exe,48 8B 0D ?? ?? ?? ?? 45 33 C9 48 8B 53 08 48 8B 01 FF,address,3,0) , // RayWorldIntersection 
	pRenderer = 0xFindPattern(Deceit.exe,48 8B 0D ?? ?? ?? ?? 48 8D 54 24 ?? 0F 28 D9,address,3,0) , 
	a_renderer_res = 0xFindPattern(Deceit.exe,4C 8B 35 ?? ?? ?? ?? 48 8B F9 48,address,3,0) , // WaterWave
	pEntitySystem = 0xFindPattern(Deceit.exe,48 8B 3D ?? ?? ?? ?? 48 8B 53,address,3,0) , // svoti_debug_probe
	pGameFramework = 0xFindPattern(Deceit.exe,48 8B 0D ?? ?? ?? ?? 4C 89 78 C0 F3,address,3,0) , 
};


////////////////////////////////////////////////////////////////////////////
SaveFileName: Dumps\ApexOffset.h
ProcessName: EasyAntiCheat_launcher.exe
#define OFFSET_LocalPlayer      0xFindPattern(EasyAntiCheat_launcher.exe,48 8B 05 ?? ?? ?? ?? 48 0F 44 C7,address,3,0)
#define OFFSET_EntityList       0xFindPattern(EasyAntiCheat_launcher.exe,48 8D 15 ?? ?? ?? ?? 48 C1 E0 05 C1 E9 10 44 8B 5C,address,3,0)

#define OFFSET_ViewRender       0xFindPattern(EasyAntiCheat_launcher.exe,48 8B 0D ? ? ? ? 41 8B D9 49 8B F8 48 8B F2 48 8B 01,address,3,0)
#define OFFSET_ViewMatrix       0xFindPattern(EasyAntiCheat_launcher.exe,48 3B C2 0F 85 ?? ?? ?? ?? 48 8B B1 ?? ?? ?? ?? 48 8B,offset,12,4)

#define OFFSET_m_CurrentWeapon  0xFindPattern(EasyAntiCheat_launcher.exe,42 89 84 BF ?? ?? ?? ?? E8,offset,4,2) //m_latestPrimaryWeapons
#define OFFSET_m_iLifeState     0xFindPattern(EasyAntiCheat_launcher.exe,0F B6 83 ?? ?? ?? ?? 84 C0 0F 94,offset,3,2)
#define OFFSET_m_iBleed         0xFindPattern(EasyAntiCheat_launcher.exe,39 B0 ?? ?? ?? ?? 0F 45,offset,2,2)
#define OFFSET_m_ihealth        0xFindPattern(EasyAntiCheat_launcher.exe,F3 0F 2A 8E,offset,4,2)
#define OFFSET_m_iTeamNum       0xFindPattern(EasyAntiCheat_launcher.exe,44 8B A7 ?? ?? ?? ?? 4C 8B,offset,3,2)
#define OFFSET_m_iShield        0xFindPattern(EasyAntiCheat_launcher.exe,39 83 ?? ?? ?? ?? 74 39,offset,2,2)
#define OFFSET_m_iMaxShield     0xFindPattern(EasyAntiCheat_launcher.exe,0F 5B F6 FF 90 ?? ?? ?? ?? 48,offset,16,2)
#define OFFSET_m_vecAbsOrigin   0xFindPattern(EasyAntiCheat_launcher.exe,89 86 ?? ?? ?? ?? 0F 28 CE,offset,2,2)
#define OFFSET_ViewAngles       0xFindPattern(EasyAntiCheat_launcher.exe,F2 0F 10 B6,offset,4,2)
#define OFFSET_Bones            0xFindPattern(EasyAntiCheat_launcher.exe,48 8B 97 ?? ?? ?? ?? 48 8D 04 5B 48 C1,offset,3,2)
#define OFFSET_CamPos           0xFindPattern(EasyAntiCheat_launcher.exe,0F 2E 89 ?? ?? ?? ?? 7A 58,offset,3,2)
////////////////////////////////////////////////////////////////////////////


