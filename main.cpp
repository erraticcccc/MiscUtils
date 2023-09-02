#include "PScanning.h"

/*
        This is for a game called "Cave Crawler", a free game on steam.
        https://store.steampowered.com/app/1865440/Cave_Crawler/
        It's a fun little game, and has all of the elements you might 
        want if you were to want to practice cheating. I recommended playing
        it if you are bored, and it only has a playtime of about 20 minutes or so.
*/
std::string DamageFuncAddress = "\x48\x3B\xCA\x0F\x84 0 0 0 0 \x48\x89";
std::string DamageFuncMask = "xxxxx????xx";
// Declaring the function addresses pattern along with the mask.

int mainThread() {
    FILE* f;

    if (!AllocConsole()) { throw "wtf"; }

    freopen_s(&f, "CONOUT$", "w", stdout);
    // Creating Console and redirectiong cout to print to it.

    auto baseMod = GetModuleHandle(NULL);
    // auto funkyBaseMod = (uintptr_t)GetModuleHandle(NULL);
    // baseMod being set to module's handle, which, when NULL is passed in the case of a DLL, gets the EXE's handle.

    PScanner pScanner(baseMod);
    // PScanner pScanner (&funkyBaseMod);               -- This is also fine, but not recommended. 
    // PScanner's constructor uses a void* as its first argument, so you have to pass a reference,
    // whereas with using GetModuleHandle, it returns a reference to the modules base. You can use 
    // whatever you'd like though, or just change my class. 
    // 
    // Creating PScanner object and passing the EXE's base, which is being 

    pScanner.Print();

    int PatternOffset = 0;
    PatternOffset = pScanner.Scan(DamageFuncAddress , "xxxxx????xx", 0x164eba6);
    // Scan returns integer offset from module base, allowing you to easily change the number for whatever reason.
    // (It is integer because I didn't want to deal with making it a BYTE (*)[], and having to parse that.)

    std::cout << "Scan return: " << std::dec << PatternOffset  << " Hex: " << std::hex << PatternOffset << "\n";
    std::cout << "Health function address: " << std::hex << baseMod + PatternOffset << "\n";


    Sleep(5000);
    // Sleeping so the address can be read.

    FreeConsole();


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
