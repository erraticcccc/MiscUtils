// this is the main file of this class, which showcases all of its capabilities. As of now, the class can:
/*
pattern scan
trampoline hook
remove spaces from any string
*/
// while not every function will be as useful as others, this is simply a class I'm making which will be able to 
// perform most things that I'd require at any given time.
#include "UltiClass.h"

/*
        All of the example code is written for a game called "Cave Crawler".
        https://store.steampowered.com/app/1865440/Cave_Crawler/
        It's a fun little game, and has all of the elements you might want
        if you were to want to practice cheating (but its good on its own too).
        I recommended playing it if you are bored, and it only has a playtime of about
                                  20 minutes or so.
*/

struct AmmoStats {
    int ammo;//offset 0
    char padding[36];
    int fireRate;//acts like a timer between shots in ms; offset 28
    char padding2[36];
    int shotsFired;//offset 50
};

class Gun {
public:
    virtual void function1() = 0;
    virtual void function2() = 0;
    virtual void function3() = 0;
    virtual void Shoot() = 0;
};

struct CurrWeapon : public Gun {
    // void* VTable; 0x0
    int WeapID;
    void* LocalPlayer;
    char* weaponName;
    int* currWeaponStock;
    AmmoStats* currWeaponAmmo;
};

struct PlayerEnt {
    uintptr_t* vTable;// offset 0
    char padding[48];
    float x;// offset 34
    float y;// offset 38
    float z;// offset 3C
    float yaw, pitch, roll;// offset 40, 44, 48 respectively
    char padding2[172];//offset F4 from offset 48
    int health;// offset F8
    int armor;// offset FC
    char padding3[293];
    int name;//offset 225
    char padding4[259];
    int teamNum;//offset 32C
    char padding5[12];
    bool isDead;//offset 338
    char padding6[60];
    CurrWeapon* currWeapon;//offset 374
};

// Declaring the function addresses pattern along with the mask.
typedef int(*ShootFn)(CurrWeapon*, void*);

ShootFn oShootFn;

int hkShootFn(CurrWeapon* ths, void* idk) {
    //ths->currWeaponAmmo += 1;
    return oShootFn(ths, idk);
}

std::string PlayerClassOffsetPattern =  "\x8B\x35\x00\x00\x00\x00\x3B\xFE\x74";
std::string PlayerClassOffsetMask =     "xx????xxx";

int mainThread() {
    FILE* f;

    if (!AllocConsole()) { throw "wtf"; }

    freopen_s(&f, "CONOUT$", "w", stdout);
    // Creating Console and redirecting cout to print to it.

    auto baseMod = (uintptr_t)GetModuleHandle(NULL);
    // baseMod being set to module's handle, which, when NULL is passed in the case of a DLL, gets the EXE's handle.

    time_t constructStart, constructEnd;

    UltiClass pScanner;
    // Creating PScanner object and passing the EXE's base, which is being

    time(&constructStart);
    pScanner.PScanner((HMODULE)baseMod);
    uintptr_t PlayerClassOffset = (pScanner.Scan(PlayerClassOffsetPattern, PlayerClassOffsetPattern) + baseMod);
    PlayerEnt* LocalPlayer = (PlayerEnt*)PlayerClassOffset;
    time(&constructEnd);
    long long cAmountTime = constructEnd - constructStart;
    // Scan returns integer offset from module base, allowing you to easily change the number for whatever reason.
    // (It is integer because I didn't want to deal with making it a BYTE (*)[], and having to parse that.)

    pScanner.Print();
    std::cout << "Time: " << std::setprecision(8) << cAmountTime << "\n\n";
    std::cout << "PlayerClassOffset: " << std::hex << PlayerClassOffset << " LocalPlayer: " << LocalPlayer << "\n";
    // As of now, nothing is actually using the healths address, but I'm going to add a trampoline hook function to this class.
    // Maybe a function pointer macro too. That might be kind of useful, though its not as though its difficult to create a function pointer.

    LocalPlayer = *(PlayerEnt**)(baseMod + 0x10f4f4);
    auto WeaponPtr = &LocalPlayer->currWeapon;
    auto WeaponVTablePtr = *(uintptr_t**)WeaponPtr;
    auto WeaponVTable = (uintptr_t*)*WeaponVTablePtr;
    uintptr_t* ShootFunctionAddress = (uintptr_t*)WeaponVTable[3];
    oShootFn = (ShootFn)(pScanner.THook((BYTE*)ShootFunctionAddress, (BYTE*)hkShootFn, 6));
    // To explain this function: 
    // The first paramter is the address of the function that we want to hook, passed in as a byte pointer. This will then copy
    // a specified amount (the third paramter) of bytes into a local BYTE* inside of the function. It will then NOP (no operation)
    // the same amount of bytes at the function, preventing any sort of weird stuff from going on. Afterwords, it places a jump instruction,
    // which jumps to the second paramter, being our hook function. oDamageFunc is being made equal to the stolen bytes, which you then call 
    // inside of the hooking function to prevent it from breaking. The amount of bytes I'm having to write for this is 19. Something to note 
    // is that the hooking process differes depending on whether the program is x86 or x64. This class has both, being THook and THook64.
    while (true) {
        Sleep(10);
    }
    // Freeing conzole and library so you can recompile the DLL as well as close the console window without worrying about closing the game.
    // Externally, you shouldn't use the hModule returned from GetModuleHandle(), but its ok since its a DLL which gets the legitimate HANDLE, compared to GetModuleHandleEx()

    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
