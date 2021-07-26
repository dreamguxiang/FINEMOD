#pragma once
#pragma warning(disable:4996)
#include "pch.h"

class Loggers {
	std::string path;
	std::ofstream logfile;
public:
	Loggers(std::string path) {
		this->path = path;
		logfile.open(path, std::ios::app);
	}
	template <typename T>
	Loggers& operator<< (T msg) {
		logfile << msg;
		return *this;
	}
};