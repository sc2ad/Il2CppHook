#pragma once

// Location of libil2cpp.so
//#define IL2CPP_SO_PATH "/data/app/com.cloudheadgames.pistolwhip-1/lib/arm64/libil2cpp.so"

#include <stdlib.h>
#include "And64InlineHook.hpp"
//#include "../../PolyHook_2_0/headers/Detour/x64Detour.hpp"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	static const char* IL2CPP_SO_PATH = "/data/app/com.cloudheadgames.pistolwhip-1/lib/arm64/libil2cpp.so";

	long long getRealOffset(void* offset);
	long long baseAddr(const char* soname);

#define MAKE_HOOK(name, addr, retval, ...) \
void* addr_ ## name = (void*) addr; \
retval (*name)(__VA_ARGS__) = NULL; \
retval hook_ ## name(__VA_ARGS__) 

#define MAKE_HOOK_OFFSETLESS(name, retval, ...) \
retval (*name)(__VA_ARGS__) = NULL; \
retval hook_ ## name(__VA_ARGS__)

#define MAKE_HOOK_NAT(name, addr, retval, ...) \
void* addr_ ## name = (void*) addr; \
retval (*name)(__VA_ARGS__) = NULL; \
retval hook_ ## name(__VA_ARGS__) 

#ifdef __aarch64__

	// ARM64 HOOKS

#define INSTALL_HOOK(name) \
log(INFO, "Installing 64 bit hook!"); \
A64HookFunction((void*)getRealOffset(addr_ ## name),(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_OFFSETLESS(name, methodInfo) \
log(INFO, "Installing 64 bit offsetless hook!"); \
A64HookFunction((void*)methodInfo->methodPointer,(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_NAT(name) \
log(INFO, "Installing 64 bit native hook!"); \
A64HookFunction((void*)(addr_ ## name),(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_DIRECT(name, addr) \
log(INFO, "Installing 64 bit direct hook!"); \
A64HookFunction((void*)addr, (void*) hook_ ## name, (void**)&name); \

#else

#ifdef __x86_64__

	// PC HOOKS

#define INSTALL_HOOK(name) \
log(INFO, "Installing x86/64 bit hook!"); \
A64HookFunction((void*)getRealOffset(addr_ ## name),(void*) hook_ ## name, (void**)&name);
//PolyHook::((void*)getRealOffset(addr_ ## name),(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_OFFSETLESS(name, methodInfo) \
log(INFO, "Installing x86/64 bit offsetless hook!"); \
A64HookFunction((void*)methodInfo->methodPointer,(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_NAT(name) \
log(INFO, "Installing x86/64 bit native hook!"); \
A64HookFunction((void*)(addr_ ## name),(void*) hook_ ## name, (void**)&name); \

#define INSTALL_HOOK_DIRECT(name, addr) \
log(INFO, "Installing x86/64 bit direct hook!"); \
A64HookFunction((void*)addr, (void*) hook_ ## name, (void**)&name); \

#else

	// ARM32 HOOKS

#define INSTALL_HOOK(name) \
log(INFO, "Installing 32 bit hook!"); \
registerInlineHook((uint32_t)getRealOffset(addr_ ## name), (uint32_t)hook_ ## name, (uint32_t **)&name); \
inlineHook((uint32_t)getRealOffset(addr_ ## name)); \

#define INSTALL_HOOK_OFFSETLESS(name, methodInfo) \
log(INFO, "Installing 32 bit offsetless hook!"); \
registerInlineHook((uint32_t)methodInfo->methodPointer, (uint32_t)hook_ ## name, (uint32_t **)&name); \
inlineHook((uint32_t)methodInfo->methodPointer); \

#define INSTALL_HOOK_NAT(name) \
log(INFO, "Installing 32 bit native hook!"); \
registerInlineHook((uint32_t)(addr_ ## name), (uint32_t)hook_ ## name, (uint32_t **)&name); \
inlineHook((uint32_t)(addr_ ## name)); \

#define INSTALL_HOOK_DIRECT(name, addr) \
log(INFO, "Installing 32 bit offsetless hook!"); \
registerInlineHook((uint32_t)addr, (uint32_t)hook_ ## name, (uint32_t **)&name); \
inlineHook((uint32_t)addr); \

#endif /* __x86_64__ or other */

#endif /* __aarch64__ or other */

#ifdef __cplusplus
}
#endif /* __cplusplus */
