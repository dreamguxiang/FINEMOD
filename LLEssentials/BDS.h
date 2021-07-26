#pragma once
#include "tickdo.h"

void setPlayerGameType(Player* pl, int t);
void* createPacket(int id);
void forceKick(Player* pl,string msg);
void forEachPlayer(Level* lv, std::function<bool(Player*)> func);
void setServerMotd();
void setItemLore(Player* pl, std::vector<string>& lore);
void sendPack(Player* pl, void* pack);
void sendText(ServerPlayer* pl, string text, TextType tp);
void FixSpectator(ServerPlayer* pl);
//void forEachDim(std::function<bool(Dimension*)> func);