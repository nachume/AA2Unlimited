#include "StdAfx.h"


namespace GameTick {

Shared::Triggers::RoomChangeData roomChangeData;
Shared::Triggers::HPositionData hPositionData;

int (__stdcall *orig_MsgHandler)(void *ptr, MSG *msg);
int (__stdcall *orig_GameTick)();

DWORD now;  // real time in ms since first tick in the game
DWORD tick; // game time in ticks

DWORD first_now;

HANDLE *hwnd;

std::vector<MsgFilterFunc> msg_filters;

void RegisterMsgFilter(MsgFilterFunc f) {
	msg_filters.push_back(f);
}

void __stdcall MsgFilter(void *ptr, MSG *msg) {
	for (auto it : msg_filters) {
		if (it(msg)) return;
	}
	if (!orig_MsgHandler(ptr, msg)) {
		TranslateMessage(msg);
		DispatchMessageW(msg);
	}
}

/*
std::set<uint64_t> timers;
void AddTimer(int when, void *fn) {
	uint64_t k = ((uint64_t)(now + when) << 32) | (uint64_t)fn;
	timers.insert(k);
}
*/

void CheckRoomChange() {

	for (int seat = 0; seat < 25; seat = seat + 1) {
		auto roomValue = Shared::GameState::GetRoomNumber(seat);
		CharInstData* instance = &AAPlay::g_characters[seat];
		if (instance->IsValid()) {
			if (instance->m_char != nullptr) {
				if (instance->m_char->m_npcData != nullptr) {
					if (instance->m_char->m_npcData->roomPtr != nullptr) {
						auto roomID = *(((int*)instance->m_char->m_npcData->roomPtr) + 5);
						if (roomValue != roomID) {
							roomChangeData.action = instance->m_forceAction.conversationId;
							roomChangeData.roomTarget = instance->m_forceAction.roomTarget;
							if (instance->m_forceAction.target1 != nullptr) {
								roomChangeData.convotarget = int(instance->m_forceAction.target1->m_thisChar);
							}

							roomChangeData.prevRoom = roomValue;
							Shared::GameState::SetRoomNumber(seat, roomID);

							roomChangeData.card = seat;
							if (roomChangeData.prevRoom < 0) return;
							Shared::Triggers::ThrowEvent(&roomChangeData);
						}
					}
				}

			}
		}
	}
}

void hPositionChange() {
	auto hPositionValue = Shared::GameState::getHPosition();
	const DWORD offset[]{ 0x3761CC, 0x28, 0x38, 0x5F4 };
	DWORD* hPosition = (DWORD*)ExtVars::ApplyRule(offset);
	if (hPosition != nullptr) {
		if (*hPosition != hPositionValue) {

			const DWORD offsetdom[]{ 0x3761CC, 0x28, 0x38, 0xe0, 0x6c, 0xe0, 0x00, 0x3c };
			DWORD* actor0 = (DWORD*)ExtVars::ApplyRule(offsetdom);

			const DWORD offsetsub[]{ 0x3761CC, 0x28, 0x38, 0xe0, 0x6c, 0xe4, 0x00, 0x3c };
			DWORD* actor1 = (DWORD*)ExtVars::ApplyRule(offsetsub);

			if (actor0 && actor1) {
				Shared::GameState::setHPosition(*hPosition);

				hPositionData.actor0 = *actor0;
				hPositionData.actor1 = *actor1;

				hPositionData.position = *hPosition;
				Shared::Triggers::ThrowEvent(&hPositionData);
			}
		}
	}
}

int __stdcall GameTick() {
	if (tick == 0) {
		first_now = GetTickCount();
		LOGPRIONC(Logger::Priority::INFO) "Entering main game event loop\n";
		LUA_EVENT_NORET("first_tick", (DWORD)(*hwnd));
	}
	now = GetTickCount() - first_now;
	tick++;
	/*
	auto it = timers.begin();
	while (it != timers.end()) {
		DWORD expire = *it >> 32;
		DWORD fptr = *it;
		int (*func)();
		func = decltype(func)(fptr);
		if (expire <= now) {
			int ret = func();
			if (ret > 0)
				AddTimer(ret, func);
			timers.erase(it);
		}
		else break;
	}*/
	if (g_Config.bTriggers) {
		CheckRoomChange();
		hPositionChange();
	}
	return orig_GameTick();
}


void Initialize() {
	hwnd = (HANDLE*)(General::GameBase + 0x34526C);
	DWORD call_MsgHandler = General::GameBase + 0x429D;
	DWORD call_GameTick = General::GameBase + 0x42BE;

	if (General::IsAAPlay) {
		call_MsgHandler = General::GameBase + 0x44A1;
		call_GameTick = General::GameBase + 0x44C2;
		hwnd = (HANDLE*)(General::GameBase + 0x368274);
	}

	Hook((BYTE*)call_MsgHandler,
	// call msghandler; test eax, eax; jnz loop
	{ 0xe8, HookControl::ANY_DWORD, 0x85, 0xc0, 0x75 }, 
	{ 0xE8, HookControl::RELATIVE_DWORD, (DWORD)&MsgFilter, 0x90, 0x90, 0xeb },
		(DWORD*)&orig_MsgHandler);

	Hook((BYTE*)call_GameTick,
	// call gametick
	{ 0xe8, HookControl::ANY_DWORD },
	{ 0xE8, HookControl::RELATIVE_DWORD, (DWORD)&GameTick },
		(DWORD*)&orig_GameTick);

}


}