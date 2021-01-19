# OffsetDumper

This program update offsets for any game by searching for offsets, you only need to make config for your game.
Already make example for two games: deceit and apex.

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

