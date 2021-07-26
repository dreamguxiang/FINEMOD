#include "pch.h"
#include "tickdo.h"
#include "Helper.h"
#include <queue>

unsigned long long ticks;
std::vector<std::function<void()>> func;

void ScheduleCheck() {
	for (auto& i : CMDSCHEDULE) {
		if (!(ticks % std::atoi(i.first.c_str()))) {
			liteloader::runcmdEx(i.second);
		}
	}
}

THook(void, "?tick@ServerLevel@@UEAAXXZ", void* self) {
	ticks++;
	ScheduleCheck();
	return original(self);
}