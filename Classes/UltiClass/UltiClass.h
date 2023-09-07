#pragma once
#include <Windows.h>
#include <vector>
#include <tuple>
#include <Psapi.h>
#include <iostream>
#include <iomanip>
#include <cstdio>

class UltiClass {

	unsigned int Size = 0;
	DWORD patternSize = 0;
	HMODULE	mBase = 0;
	std::vector <BYTE> copiedModule;

	BYTE* stolenBytes;

	std::string RemoveWhitespace(std::string input);

public:
	
	BYTE* THook(BYTE* to, BYTE* from, int amount);
	// to is the function you want to hook, from is the function
	// that overrides "to".
	BYTE* THook64(BYTE* to, BYTE* from, int amount);

	void PScanner(HMODULE moduleBase);

	int Scan(std::string pattern, std::string mask, unsigned int baseOffset = 0);
	/*Returns the offset from the base as an integer. Takes a std::string pattern, std::string mask, and optional offset from base.
	The idea of the offset is to make debugging a bit easier for patterns further in the module. Working on implementing the ability
	to input different styles of patterns, e.g. allowing for both "\x50\x70\x23" and "59 f0 4e 23".*/

	int ScanAndDestroyModule(std::string pattern, std::string mask, unsigned int baseOffset = 0);

	void DestroyModule();
	/* This does as implied, destroying the copied module. I will probably implement this into the default scanning function at some point,
	because its quite a thing to have an entire module copied into some part of memory. For now, only used in the wrapped function.*/

	~UltiClass() = default;

	void Print();
	/*Debug print. No real use.*/

	uintptr_t ReturnOffset(BYTE foundPattern[], unsigned int offset = 0, unsigned int amountOfBytes = 0);
	/*Not yet implemented, but will return the address to a set of bytes that is offset bytes away from a given address.
	Really not THAT helpful, but might be useful.*/
};
