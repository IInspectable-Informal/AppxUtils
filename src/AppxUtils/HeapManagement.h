#pragma once

void* __stdcall operator new(size_t size);
void __stdcall operator delete(void* ptr) noexcept;

void* __stdcall operator new[](size_t size);
void __stdcall operator delete[](void* ptr) noexcept;

void* __stdcall operator new(size_t size, void* ptr) noexcept;
