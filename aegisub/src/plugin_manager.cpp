// Copyright (c) 2008, Rodrigo Braz Monteiro
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

/// @file plugin_manager.cpp
/// @brief Keep track of and set up variable parts of the application
/// @ingroup main
///

#include "config.h"

#include "include/aegisub/audio_player.h"
#include "include/aegisub/audio_provider.h"
#include "include/aegisub/spellchecker.h"
#include "include/aegisub/subtitles_provider.h"
#include "plugin_manager.h"
#include "video_provider_manager.h"

#ifdef WITH_AUTO4_LUA
#include "auto4_lua_factory.h"
#endif

PluginManager::PluginManager() {
	init = false;

#ifdef WITH_AUTO4_LUA
	lua = NULL;
#endif

}

PluginManager::~PluginManager() {
	VideoProviderFactory::Clear();
	AudioProviderFactory::Clear();
	AudioPlayerFactory::Clear();
	SubtitlesProviderFactory::Clear();
	SpellCheckerFactory::Clear();

#ifdef WITH_AUTO4_LUA
	Automation4::ScriptFactory::Unregister(lua);
#endif
}

/// @brief Registers all built-in plugins
void PluginManager::RegisterBuiltInPlugins() {
	if (!init) {
		// Managers
		VideoProviderFactory::RegisterProviders();
		AudioProviderFactory::RegisterProviders();
		AudioPlayerFactory::RegisterProviders();
		SubtitlesProviderFactory::RegisterProviders();
		SpellCheckerFactory::RegisterProviders();

		// Automation languages
#ifdef WITH_AUTO4_LUA
		lua = new Automation4::LuaScriptFactory();
#endif
	}

	// Done
	init = true;
}
