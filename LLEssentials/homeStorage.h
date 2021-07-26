#pragma once
#include "pch.h"
#include<stl\LRUMap.h>
#include <stl\viewhelper.h>
extern std::unique_ptr<KVDBImpl> dbtpa;
struct Vec4 {
	Vec3 vc;
	char dimid;
	string toStr()const {
		return "(" + std::to_string(vc.x) + " , " + std::to_string(vc.y) + " , " + std::to_string(vc.z) + " , " + std::to_string(dimid) + " , " + ")";
	}
	template<typename _TP>
	void pack(WBStreamImpl<_TP>& ws) const {
		ws.apply(vc, dimid);
	}
	void unpack(RBStream& rs) {
		rs.apply(vc, dimid);
	}
	void teleport(WPlayer wp) {
		wp.teleport(vc, dimid);
	}
	Vec4(WActor wp) {
		vc = wp->getPos();
		vc.y -= 1.5;
		dimid = wp.getDimID();
	}
	Vec4(WPlayer wp) {
		vc = wp->getPos();
		vc.y -= 1.5;
		dimid = wp.getDimID();
	}
	Vec4(Vec3 x, int dim) :vc(x), dimid(dim) {}
	Vec4() {}
};
struct Homes {
	struct Home {
		Vec4 pos;
		string name;
		Home() {}
		Home(string const& b, WActor ac) :name(b), pos(ac) {
		}
		Home(string const& b, Vec4 const& ac) :name(b), pos(ac) {
		}
		template<typename _TP>
		void pack(WBStreamImpl<_TP>& ws) const {
			ws.apply(pos, name);
		}
		void unpack(RBStream& rs) {
			rs.apply(pos, name);
		}
	};
	xuid_t owner = 0;
	std::list<Home> data;
	Homes() {}
	Homes(xuid_t xid) {
		string val;
		if (dbtpa->get(to_view(xid), val)) {
			RBStream rs{ val };
			rs.apply(data);
		}
		owner = xid;
	}
	Homes(string_view own) {
		string val;
		auto x = XIDREG::str2id(own);
		if (x.Set()) {
			if (dbtpa->get(to_view(x.val()), val)) {
				RBStream rs{ val };
				rs.apply(data);
			}
			owner = x.val();
		}
	}
	template<typename _TP>
	void pack(WBStreamImpl<_TP>& ws) const {
		ws.apply(data);
	}
	void unpack(RBStream& rs) {
		rs.apply(data);
	}
	void save() {
		WBStream ws;
		ws.apply(*this);
		if (owner)
			dbtpa->put(to_view(owner), ws);
	}
};
Homes& getHomeInCache(xuid_t xid);