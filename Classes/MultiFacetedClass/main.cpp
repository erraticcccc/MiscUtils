// this is the main file of this class, which showcases all of its capabilities. As of now, the class can:
/*
pattern scan
trampoline hook
remove spaces from any string
*/
// while not every function will be as useful as others, this is simply a class I'm making which will be able to 
// perform most things that I'd require at any given time.
#include "PScanning.h"

/*
        All of the example code is written for a game called "Cave Crawler".
        https://store.steampowered.com/app/1865440/Cave_Crawler/
        It's a fun little game, and has all of the elements you might want
        if you were to want to practice cheating (but its good on its own too).
        I recommended playing it if you are bored, and it only has a playtime of about
                                  20 minutes or so.
*/
std::string DamageFuncAddress = "\x48\x3B\xCA\x0F\x84 0 0 0 0 \x48\x89";
std::string DamageFuncMask = "xxxxx????xx";
// Declaring the function addresses pattern along with the mask.

int mainThread() {
    FILE* f;

    if (!AllocConsole()) { throw "wtf"; }

    freopen_s(&f, "CONOUT$", "w", stdout);
    // Creating Console and redirecting cout to print to it.

    auto baseMod = (uintptr_t)GetModuleHandle(NULL);
    // baseMod being set to module's handle, which, when NULL is passed in the case of a DLL, gets the EXE's handle.

    PScanner pScanner((HMODULE)baseMod);
    // Creating PScanner object and passing the EXE's base, which is being

    uintptr_t DamageFuncOffset = pScanner.ScanAndDestroyModule(DamageFuncAddress, "xxxxx????xx");
    // Scan returns integer offset from module base, allowing you to easily change the number for whatever reason.
    // (It is integer because I didn't want to deal with making it a BYTE (*)[], and having to parse that.)

    std::cout << "Scan return: " << std::dec << DamageFuncOffset << " Hex: " << std::hex << DamageFuncOffset << "\n";
    std::cout << "Health function address: " << std::hex << (baseMod + DamageFuncOffset) << "\n";
    // As of now, nothing is actually using the healths address, but I'm going to add a trampoline hook function to this class.
    // Maybe a function pointer macro too. That might be kind of useful, though its not as though its difficult to create a function pointer.

    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)baseMod, 0);
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
