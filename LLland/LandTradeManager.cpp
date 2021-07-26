#include"pch.h"
#include"shared.h"
#include<llmoney.h>
bool land_buy_first_step(WPlayer wp, CommandOutput& outp, PointSelector& point) {
	if (wp.getPermLvl() == 0) {
		if (point.size() > 100000) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.too.big"));
			return false;
		}
		size_t lsz = point.size();
		if (lsz * BUY_PRICE > 2100000000) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.too.big"));
			return false;
		}
		auto mone = Money::getMoney(offPlayer::getXUID(wp));
		if ((size_t)mone < (lsz * BUY_PRICE)) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.money"));
			return false;
		}
	}
	return true;
}
bool land_buy_next_step(WPlayer wp, CommandOutput& outp, PointSelector& point) {
	if (wp.getPermLvl() < 1) {
		if (!Money::createTrans(offPlayer::getXUID(wp), 0, BUY_PRICE * point.size(), "buy land")) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.money"));
			return false;
		}
	}
	return true;
}
void land_sell_helper(WPlayer wp, CommandOutput& outp, FastLand const* fl) {
	size_t siz = fl->dz - fl->z + 1;
	siz = siz * (fl->dx - fl->x + 1);
	siz *= SELL_PRICE;
	Money::createTrans(0, offPlayer::getXUID(wp), money_t(siz), "sell land " + S(fl->lid));
	outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 你出售领地获得了 $" + S(siz) );
}