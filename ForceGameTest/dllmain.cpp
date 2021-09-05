// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <iostream>

#define SYM(sym) GetServerSymbol(sym)
#define HOOK(name, ret, sym, ...)		\
struct name {							\
	typedef ret(*fn)(__VA_ARGS__);		\
	static ret _hook(__VA_ARGS__);		\
	static fn original;					\
};										\
name::fn name::original = *reinterpret_cast<name::fn*>(SymHook(sym, name::_hook, &name::original)); \
ret name::_hook(__VA_ARGS__)

//提供Detours
extern "C" _declspec(dllimport)
int HookFunction(void*, void*, void*);
//获取符
extern "C" _declspec(dllimport)
void* GetServerSymbol(const char*);
//调用一个函数
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

HOOK(fuck,bool, "?Gametest@Experiments@@QEBA_NXZ", void* _this) {
    std::cout << u8"[ForceGameTest]强制开启Gametest成功！\n";
    return 1;
}