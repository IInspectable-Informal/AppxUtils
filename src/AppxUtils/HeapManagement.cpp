#include "pch.h"
#include "HeapManagement.h"
#include <stdio.h>

void* __stdcall operator new(size_t size)
{
	printf("Allocating mem...");
	auto* ptr = HeapAlloc(GetProcessHeap(), 0, size);
	printf(" addr: 0x%016p\n", ptr);
	return ptr;
}

void __stdcall operator delete(void* ptr) noexcept
{
	if (ptr)
	{
		printf("Free mem... addr: 0x%016p\n", ptr);
		HeapFree(GetProcessHeap(), 0, ptr);
	}
}

void* __stdcall operator new[](size_t size)
{
	printf("Allocating array mem...");
	auto* ptr = HeapAlloc(GetProcessHeap(), 0, size);
	printf(" addr: 0x%016p\n", ptr);
	return ptr;
}

void __stdcall operator delete[](void* ptr) noexcept
{
	if (ptr)
	{
		printf("Free array mem... addr: 0x%016p\n", ptr);
		HeapFree(GetProcessHeap(), 0, ptr);
	}
}

void* __stdcall operator new(size_t size, void* ptr) noexcept
{ return ptr; }
