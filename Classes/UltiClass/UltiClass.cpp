#include "UltiClass.h"

BYTE* UltiClass::THook(BYTE* toHook, BYTE* from, int amount) {
	// to is the function you want to hook, from is the function
	// that overrides "to".
	if (amount < 5) return 0;
	DWORD p;
	VirtualProtect(toHook, amount, PAGE_EXECUTE_READWRITE, &p);
	stolenBytes = (BYTE*)VirtualAlloc(0, amount + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!stolenBytes) return 0;
	memcpy(stolenBytes, toHook, amount);
	uintptr_t relAddress = from - toHook - 5;
	uintptr_t jumpbackAddress = toHook - stolenBytes - 5;
	memset(toHook, 0x90, amount);

	BYTE relJump[] = {
		0xE9, 0x0, 0x0, 0x0, 0x0
	};
	memcpy(&relJump[1], &relAddress, 4);
	memcpy(toHook, relJump, 5);

	memcpy(&relJump[1], &jumpbackAddress, 4);
	memcpy(stolenBytes + amount, relJump, 5);

	VirtualProtect(toHook, amount, p, &p);

	return stolenBytes;
}

BYTE* UltiClass::THook64(BYTE* toHook, BYTE* from, int amount) {
	// to is the function you want to hook, from is the function
	// that overrides "to".
	if (amount < 13) return 0;
	DWORD p;
	VirtualProtect(toHook, amount, PAGE_EXECUTE_READWRITE, &p);
	stolenBytes = (BYTE*)VirtualAlloc(0, amount + 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!stolenBytes) return 0; // virtualAlloc failed!
	memcpy(stolenBytes, toHook, amount);

	BYTE absJump64[] = {
		0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x41, 0xFF, 0xE2
	};
	uint64_t testingVar = (uint64_t)from;
	memcpy(&absJump64[2], &testingVar, 8);
	memset(toHook, 0x90, amount);
	memcpy(toHook, absJump64, sizeof(absJump64));

	uint64_t jumpbackAddress = (uint64_t)(toHook + 13);
	memcpy(&absJump64[2], &jumpbackAddress, 8);
	memcpy((stolenBytes + amount), absJump64, 13);

	VirtualProtect(toHook, amount, p, &p);

	return stolenBytes;
}

std::string UltiClass::RemoveWhitespace(std::string input) {
	// I am sure this is so extremely bad in terms of efficiency,
	// but I don't care 8)

	std::size_t additive = 0;
	char tempChar;
	bool endOfStr = false;
	std::size_t strSize = input.size();
	if (input.find_first_of(" ") == input.npos) return input;

	for (std::size_t i = 0; i < strSize - 1; i++) {
		if (input[i] == ' ') {
			additive = i;
			while (input[additive] == ' ') {	
				if (additive >= strSize) {
					endOfStr = true;
					break;
				}
				additive++;
			}
			if (endOfStr) break;
			tempChar = input[additive];
			input[additive] = ' ';
			input[i] = tempChar;
		}
	}

	input.resize(input.find_first_of(' ') - 1);
	return input;
}


void UltiClass::PScanner(HMODULE moduleBase) {

	HANDLE procAddress = GetCurrentProcess();

	mBase = moduleBase;

	MODULEINFO modInfo;

	GetModuleInformation(procAddress, mBase, &modInfo, sizeof(MODULEINFO));

	Size = modInfo.SizeOfImage;

	copiedModule.resize(Size);

	std::memcpy(copiedModule.data(), moduleBase, Size);

}

int UltiClass::Scan(std::string pattern, std::string mask, unsigned int baseOffset) {
  // Next update, this will be changed to be more efficient,
  // skipping pages of memory that can't be written and whatnot.
  pattern = RemoveWhitespace(pattern);

	patternSize = pattern.length();

	std::cout << "Pattern Size: " << std::dec << patternSize << "\n";

	for (unsigned int pAddress = 0 + baseOffset; pAddress < Size; pAddress++) {
		if (pattern[0] == static_cast<signed char>(copiedModule[pAddress])) {
			for (unsigned int j = 0; j < patternSize; j++) {
				if (mask[j] == '?') continue;
				if (pattern[j] != static_cast<signed char>(copiedModule[pAddress + j])) {
					break;
				}
				else {
					if (j == (patternSize - 1)) {
						return pAddress;
					}
					continue;
				}
			}
		}
	}
	return 0;
}

void UltiClass::DestroyModule() {
	copiedModule.clear();
	copiedModule.~vector();
}

int UltiClass::ScanAndDestroyModule(std::string pattern, std::string mask, unsigned int baseOffset) {
	int offsetFromBase = Scan(pattern, mask, baseOffset);
	DestroyModule();
	return offsetFromBase;
}

uintptr_t UltiClass::ReturnOffset(BYTE foundPattern[], unsigned int Offset, unsigned int amountOfBytes) {
	if ((Offset > sizeof(foundPattern)) || (amountOfBytes > sizeof(foundPattern))) return 0;
	

	return 0;
}

void UltiClass::Print() {

	std::cout << "Module Size: " << std::hex << this->Size << "\n";

	std::cout << "Copied module address and size: " << &copiedModule << " " << Size << " " << "\n";

}
