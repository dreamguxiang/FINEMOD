#pragma once
#include <iostream>

template <typename T, int off>
inline T& dAccess(void* ptr) {
	return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T, int off>
inline T const& dAccess(void const* ptr) {
	return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T>
inline T& dAccess(void* ptr, uintptr_t off) {
	return *(T*)(((uintptr_t)ptr) + off);
}
template <typename T>
inline const T& dAccess(void const* ptr, uintptr_t off) {
	return *(T*)(((uintptr_t)ptr) + off);
}

#define SYM(sym) GetServerSymbol(sym)
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original;					\
};										\
name::fn name::original = *reinterpret_cast<name::fn*>(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)

//�ṩDetours
extern "C" _declspec(dllimport)
int HookFunction(void*, void*, void*);
//��ȡ��
extern "C" _declspec(dllimport)
void* GetServerSymbol(const char*);
//����һ������
template<typename ret = void, typename... Args>
static ret SymCall(const char* sym, Args... args) {
	void* found = SYM(sym);
	if (!found)
		std::cerr << "Failed to call " << sym << std::endl;
	return reinterpret_cast<ret(*)(Args...)>(found)(args...);
}
static void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		std::cerr << "Failed to hook " << sym << std::endl;
	HookFunction(found, org, hook);
	return org;
}
