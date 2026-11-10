// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

void* __cdecl operator new(size_t size);
void __cdecl operator delete(void* ptr) noexcept;
void __cdecl operator delete(void* ptr, size_t size) noexcept;

void* __cdecl operator new[](size_t size);
void __cdecl operator delete[](void* ptr) noexcept;
void __cdecl operator delete[](void* ptr, size_t size) noexcept;

void* __cdecl operator new(size_t size, void* ptr) noexcept;
