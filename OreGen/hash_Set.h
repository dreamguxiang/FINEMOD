#pragma once
#include<iostream>
using namespace std;
template<class hash_type>
class hash_set
{
private:
	hash_type array[100000];
	int hash_fun(hash_type original);
public:
	hash_set();//¹¹Ôìº¯Êý
	void insert(hash_type value);
	void erase(hash_type target);
	bool contain(hash_type query);
};