// Copyright (c) 2006, 2007, Niels Martin Hansen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Aegisub Project http://www.aegisub.org/

/// @file auto4_lua_progresssink.cpp
/// @brief Lua 5.1-based scripting engine
/// @ingroup scripting
///

#include "config.h"

#include "auto4_lua.h"

#include "auto4_lua_utils.h"

#include <wx/filedlg.h>

namespace {
	void set_field_to_closure(lua_State *L, const char *name, lua_CFunction fn, int ps_idx = -3)
	{
		lua_pushvalue(L, ps_idx);
		lua_pushcclosure(L, fn, 1);
		lua_setfield(L, -2, name);
	}

	void set_field_to_nil(lua_State *L, int idx, const char *name)
	{
		lua_pushnil(L);
		lua_setfield(L, idx, name);
	}
}

namespace Automation4 {
	LuaProgressSink::LuaProgressSink(lua_State *L, ProgressSink *ps, bool allow_config_dialog)
		: L(L)
	{
		ProgressSink **ud = (ProgressSink**)lua_newuserdata(L, sizeof(ProgressSink*));
		*ud = ps;

		// register progress reporting stuff
		lua_getglobal(L, "aegisub");

		// Create aegisub.progress table
		lua_newtable(L);
		set_field_to_closure(L, "set", LuaSetProgress);
		set_field_to_closure(L, "task", LuaSetTask);
		set_field_to_closure(L, "title", LuaSetTitle);
		set_field_to_closure(L, "is_cancelled", LuaGetCancelled);
		lua_setfield(L, -2, "progress");

		// Create aegisub.debug table
		lua_newtable(L);
		set_field_to_closure(L, "out", LuaDebugOut);
		lua_setfield(L, -2, "debug");

		// Set aegisub.log
		set_field_to_closure(L, "log", LuaDebugOut, -2);

		if (allow_config_dialog) {
			lua_newtable(L);
			set_field_to_closure(L, "display", LuaDisplayDialog);
			set_field_to_closure(L, "open", LuaDisplayOpenDialog);
			set_field_to_closure(L, "save", LuaDisplaySaveDialog);
			lua_setfield(L, -2, "dialog");
		}

		// reference so other objects can also find the progress sink
		lua_pushvalue(L, -2);
		lua_setfield(L, LUA_REGISTRYINDEX, "progress_sink");

		lua_pop(L, 2);
	}

	LuaProgressSink::~LuaProgressSink()
	{
		// remove progress reporting stuff
		lua_getglobal(L, "aegisub");
		set_field_to_nil(L, -2, "progress");
		set_field_to_nil(L, -2, "debug");
		lua_pop(L, 1);

		set_field_to_nil(L, LUA_REGISTRYINDEX, "progress_sink");
	}

	ProgressSink* LuaProgressSink::GetObjPointer(lua_State *L, int idx)
	{
		assert(lua_type(L, idx) == LUA_TUSERDATA);
		return *((ProgressSink**)lua_touserdata(L, idx));
	}

	int LuaProgressSink::LuaSetProgress(lua_State *L)
	{
		GetObjPointer(L, lua_upvalueindex(1))->SetProgress(lua_tonumber(L, 1), 100);
		return 0;
	}

	int LuaProgressSink::LuaSetTask(lua_State *L)
	{
		GetObjPointer(L, lua_upvalueindex(1))->SetMessage(luaL_checkstring(L, 1));
		return 0;
	}

	int LuaProgressSink::LuaSetTitle(lua_State *L)
	{
		GetObjPointer(L, lua_upvalueindex(1))->SetTitle(luaL_checkstring(L, 1));
		return 0;
	}

	int LuaProgressSink::LuaGetCancelled(lua_State *L)
	{
		lua_pushboolean(L, GetObjPointer(L, lua_upvalueindex(1))->IsCancelled());
		return 1;
	}

	int LuaProgressSink::LuaDebugOut(lua_State *L)
	{
		ProgressSink *ps = GetObjPointer(L, lua_upvalueindex(1));

		// Check trace level
		if (lua_type(L, 1) == LUA_TNUMBER) {
			if (lua_tointeger(L, 1) > ps->GetTraceLevel())
				return 0;
			// remove trace level
			lua_remove(L, 1);
		}

		// Only do format-string handling if there's more than one argument left
		// (If there's more than one argument left, assume first is a format string and rest are format arguments)
		if (lua_gettop(L) > 1) {
			// Format the string
			lua_getglobal(L, "string");
			lua_getfield(L, -1, "format");
			// Here stack contains format string, format arguments, 'string' table, format function
			// remove 'string' table
			lua_remove(L, -2);
			// put the format function into place
			lua_insert(L, 1);
			// call format function
			if (lua_pcall(L, lua_gettop(L) - 1, 1, 0)) {
				// format failed so top of the stack now has an error message
				// which we want to add position information to
				luaL_where(L, 1);
				lua_insert(L, 1);
				lua_concat(L, 2);
				lua_error(L);
			}
		}

		// Top of stack is now a string to output
		ps->Log(luaL_checkstring(L, 1));
		return 0;
	}

	int LuaProgressSink::LuaDisplayDialog(lua_State *L)
	{
		ProgressSink *ps = GetObjPointer(L, lua_upvalueindex(1));

		LuaDialog dlg(L, true); // magically creates the config dialog structure etc
		ps->ShowDialog(&dlg);

		// more magic: puts two values on stack: button pushed and table with control results
		return dlg.LuaReadBack(L);
	}

	int LuaProgressSink::LuaDisplayOpenDialog(lua_State *L)
	{
		ProgressSink *ps = GetObjPointer(L, lua_upvalueindex(1));
		wxString message(check_wxstring(L, 1));
		wxString dir(check_wxstring(L, 2));
		wxString file(check_wxstring(L, 3));
		wxString wildcard(check_wxstring(L, 4));
		bool multiple = !!lua_toboolean(L, 5);
		bool must_exist = lua_toboolean(L, 6) || lua_isnil(L, 6);

		int flags = wxFD_OPEN;
		if (multiple)
			flags |= wxFD_MULTIPLE;
		if (must_exist)
			flags |= wxFD_FILE_MUST_EXIST;

		wxFileDialog diag(nullptr, message, dir, file, wildcard, flags);
		if (ps->ShowDialog(&diag) == wxID_CANCEL) {
			lua_pushnil(L);
			return 1;
		}

		if (multiple) {
			wxArrayString files;
			diag.GetPaths(files);

			lua_newtable(L);
			for (size_t i = 0; i < files.size(); ++i) {
				lua_pushstring(L, files[i].utf8_str());
				lua_rawseti(L, -2, i + 1);
			}

			return 1;
		}

		lua_pushstring(L, diag.GetPath().utf8_str());
		return 1;
	}

	int LuaProgressSink::LuaDisplaySaveDialog(lua_State *L)
	{
		ProgressSink *ps = GetObjPointer(L, lua_upvalueindex(1));
		wxString message(check_wxstring(L, 1));
		wxString dir(check_wxstring(L, 2));
		wxString file(check_wxstring(L, 3));
		wxString wildcard(check_wxstring(L, 4));
		bool prompt_overwrite = !lua_toboolean(L, 5);

		int flags = wxFD_SAVE;
		if (prompt_overwrite)
			flags |= wxFD_OVERWRITE_PROMPT;

		wxFileDialog diag(ps->GetParentWindow(), message, dir, file, wildcard, flags);
		if (ps->ShowDialog(&diag) == wxID_CANCEL) {
			lua_pushnil(L);
			return 1;
		}

		lua_pushstring(L, diag.GetPath().utf8_str());
		return 1;
	}
}
