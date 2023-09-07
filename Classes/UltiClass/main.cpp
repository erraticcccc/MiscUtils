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
        All of the example code is for a game called "Assault Cube", a game which I assume you are 
        more than likely familiar with if you have done any sort of cheating. Note that this will not 
        actually work when compiled, because I'm currently learning VTable hooking and what-not. The 
        pattern scan example will also not work, as I am going to need to learn how to get specific 
        classes using pattern scanning. With that said, every function the class has so far should work.
        Please tell me if there are any issues.
*/

struct AmmoStats {
    int ammo;                            // 0x0
    char padding[36];                    // 0x4 -> 0x24
    int fireRate;                        // 0x28
    char padding2[36];                   // 0x2C -> 0x4C
    int shotsFired;                      // 0x50
};

class Gun {
public:
    virtual void function1() = 0;
    virtual void function2() = 0;
    virtual void function3() = 0;
    virtual void Shoot() = 0; // The 4th function within the gun's virtual tables is the Shoot function, which I'm trying to hook.
};

struct CurrWeapon : public Gun {
    // void* VTable; 0x0 // This is just to show that 0x0 is used, and that its a VTable.
    int WeapID;                         // 0x4
    void* LocalPlayer;                  // 0x8
    char* weaponName;                   // 0xC
    int* currWeaponStock;               // 0x10
    AmmoStats* currWeaponAmmo;          // 0x14
};

struct PlayerEnt {
    uintptr_t* vTable;                  // 0x0
    char padding[48];                   // 0x4 -> 0x30
    float x, y, z;                      // 0x34, 0x38, 0x3C
    float yaw, pitch, roll;             // 0x40, 0x44, 0x48
    char padding2[172];                 // 0x48 -> 0xF4 
    int health;                         // 0xF8
    int armor;                          // 0xFC
    char padding3[293];                 // 0x100 -> 0x221
    char* name;                         // offset 225
    char padding4[259];                 // 0x229 -> 0x329 
    int teamNum;                        // 0x32C
    char padding5[12];                  // 0x330 -> 0x334
    bool isDead;                        // 0x338
    char padding6[60];                  // 0x338 -> 0x370
    CurrWeapon* currWeapon;             // 0x374
};

typedef int(*ShootFn)(CurrWeapon*, Vec3&);
// Declares "ShootFn" as a pointer to a function that returns an int, 
// with the parameters of a Weapon pointer and a vector reference.

ShootFn oShootFn;

int hkShootFn(CurrWeapon* ths, Vec3 &idk) {
    //ths->currWeaponAmmo += 1; // currently commented out because I thought maybe what I was doing to the ammo was 
                                // negatively affecting and/or breaking the function.
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

    UltiClass UtilClass;
    // Along with the changing of the name of the class, I've opted for a "better" type of instatiation, despite
    // it being the default constructor. The previous one required the input for the pattern scanning function,
    // which, for anyone wanting to use the class for reasons other than pattern scanning, would be inconvenient 
    // and pointless.
    // Also, while I personally think that "UltiClass" is quite an arrogant name, I plan to keep it due to the fact
    // that it's an anagram for "UtilClass".

    time(&constructStart);
    UtilClass.PScanner((HMODULE)baseMod);
    // This is the new init function for the pattern scanner. 
    uintptr_t PlayerClassOffset = (UtilClass.Scan(PlayerClassOffsetPattern, PlayerClassOffsetPattern) + baseMod);
    // Along with allowing for the class to be created without requiring a module to be copied, a new function has
    // also been added, being the one used above. It does as inferred, destroying the copied module after finding the pattern.
    // You can also destroy the module yourself with the DestroyModule function.
    //
    // Scan returns an integer offset from module base, allowing you to easily change the number for whatever reason.
    // (It is integer because I didn't want to deal with making it a BYTE (*)[], and having to parse that.)
        
    PlayerEnt* LocalPlayer = (PlayerEnt*)PlayerClassOffset;
    time(&constructEnd);
    long long cAmountTime = constructEnd - constructStart;

    UtilClass.Print();
    std::cout << "Time: " << std::setprecision(8) << cAmountTime << "\n\n";
    std::cout << "PlayerClassOffset: " << std::hex << PlayerClassOffset << " LocalPlayer: " << LocalPlayer << "\n";
    // This currently isn't correct, as it returns a part in memory that I was using to test. I will eventually
    // replace it with something that works, but as of now, it finds patterns fine. 

    LocalPlayer = *(PlayerEnt**)(baseMod + 0x10f4f4);
    auto WeaponPtr = &LocalPlayer->currWeapon;
    auto WeaponVTablePtr = *(uintptr_t**)WeaponPtr;
    auto WeaponVTable = (uintptr_t*)*WeaponVTablePtr;
    // A very annoying, yet probably the easiest, process for getting virtual function addresses.
    // Might try creating a function for it. 
        
    uintptr_t* ShootFunctionAddress = (uintptr_t*)WeaponVTable[3];
    oShootFn = (ShootFn)(UtilClass.THook((BYTE*)ShootFunctionAddress, (BYTE*)hkShootFn, 6));
    // This is the 32 bit hook function, which creates a 5 byte jump which uses the relative address from the function
    // you want to hook to the function you are wanting to replace the original with. The 3rd parameter is the size, and 
    // because of how jumps work, it has to be over 5 bytes in length. While this might seem like quite the limitation,
    // trampoline hooking completely fixes any issues overwriting the original bytes might cause. This class has both 32-bit
    // and 64-bit hooking functions, the latter being named THook64. As of now, this specific declaration doesn't work, as, I'm 
    // unsure as to why, it causes a memory access violation. It's not the hooking function itself, but rather the function I'm wanting
    // to use to hook.
    
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
