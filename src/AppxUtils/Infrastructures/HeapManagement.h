// Copyright 2026 IInspectable-Informal
// SPDX-License-Identifier: Apache-2.0
#pragma once

void* __stdcall operator new(size_t size);
void __stdcall operator delete(void* ptr) noexcept;

void* __stdcall operator new[](size_t size);
void __stdcall operator delete[](void* ptr) noexcept;

void* __stdcall operator new(size_t size, void* ptr) noexcept;
