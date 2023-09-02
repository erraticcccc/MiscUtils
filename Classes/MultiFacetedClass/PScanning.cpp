#include "PScanning.h"

std::string PScanner::RemoveWhitespace(std::string input) {
	// I am sure this is so extremely bad in terms of efficiency,
	// but I don't care 8)
	int additive;
	char tempChar;
	bool endOfStr = false;
	int strSize = input.size();
	if (input.find_first_of(" ") == strSize) return input;

	for (unsigned int i = 0; i < strSize - 1; i++) {
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


PScanner::PScanner(HMODULE moduleBase) {

	HANDLE procAddress = GetCurrentProcess();

	mBase = moduleBase;

	MODULEINFO modInfo;

	GetModuleInformation(procAddress, mBase, &modInfo, sizeof(MODULEINFO));

	Size = modInfo.SizeOfImage;

	copiedModule.resize(Size);

  // same with this. I am almost certain theres a better way to do this, but 
  // it seemed like it would probably be the best for avoiding anticheat at a basic
  // level (purely off speculation and assumption).
	std::memcpy(copiedModule.data(), moduleBase, Size);

}

int PScanner::Scan(std::string pattern, std::string mask, unsigned int baseOffset) {
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

void PScanner::DestroyModule() {
	copiedModule.clear();
	copiedModule.~vector();
}

int PScanner::ScanAndDestroyModule(std::string pattern, std::string mask, unsigned int baseOffset) {
	int offsetFromBase = Scan(pattern, mask, baseOffset);
	DestroyModule();
	return offsetFromBase;
}

uintptr_t PScanner::ReturnOffset(unsigned int Offset) {
	return 0;
}

void PScanner::Print() {

	std::cout << "Module Size: " << std::hex << this->Size << "\n";

	std::cout << "Copied module address and size: " << &copiedModule << " " << Size << " " << "\n";

}
