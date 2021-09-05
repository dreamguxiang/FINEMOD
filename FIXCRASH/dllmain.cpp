#include "pch.h"
#include <api/regCommandHelper.h>

void entry();
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		entry();
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


std::vector<unsigned int> Music_Tones;
unsigned short scale = 1;

void sendNote(unsigned int tone) {
	if (tone == 0) {
		return;
	}
	for (auto pl : liteloader::getAllPlayers()) {
		WBStream ws2;
		auto sp = MakeSP(pl);
		auto bpos = sp->getPos();
		ws2.apply(VarInts(81),
			Vec3{ bpos.x,bpos.y,bpos.z },
			VarInts(tone * 2),
			MCString(""),
			(char)0,
			(char)1);
		MyPkt<0x7B> pkts{ ws2 };
		sp->sendNetworkPacket(pkts);
	}
}

bool onMusic(CommandOrigin const& ori, CommandOutput& outp, std::string& file) {
	std::ifstream fs;
	fs.open("plugins/NoteBlockMusic/" + file + ".txt");
	if (!fs) {
		outp.error("Music not found");
		return false;
	}
	fs.close();
	outp.addMessage("Loading music");
	std::thread th([file]() {
		std::ifstream fs;
		fs.open("plugins/NoteBlockMusic/" + file + ".txt");
		std::string music_cache;
		fs >> music_cache;
		for (int i = 0; i < music_cache.size(); i++) {
			char tone_c = music_cache[i];
			unsigned tone = std::atoi(&tone_c);
			if (i == 0) {
				scale = tone;
				continue;
			}
			Music_Tones.push_back(tone * scale);
		}
		for (unsigned int tone : Music_Tones) {
			sendNote(tone);
			Sleep(250);
		}
		Music_Tones.clear();
		fs.close();
		});
	th.detach();
	return true;
}

void entry() {
	Event::addEventListener([](RegCmdEV e) {
		CMDREG::SetCommandRegistry(e.CMDRg);
		MakeCommand("notemusic", "play music", 1);
		CmdOverload(notemusic, onMusic, "MusicFile");
		}
	);
}