/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file lua_test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-09-05
 ** \last modified 2017-09-05 19:46:24
**********************************************************/

#include "base/base.h"
#include "lua/lua.hpp"
using namespace std;

int main (int argc, char **argv) {

	lua_State *L = luaL_newstate();

	lua_pushstring(L, "I am so cool~");
	lua_pushnumber(L,20);


	if( lua_isstring(L,1)){
		cout<<lua_tostring(L,1)<<endl;
	}
	if( lua_isnumber(L,2)){
		cout<<lua_tonumber(L,2)<<endl;
	}

	lua_close(L); 
	return 0;
}

