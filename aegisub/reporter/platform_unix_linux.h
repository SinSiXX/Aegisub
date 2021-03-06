// Copyright (c) 2009, Grigori Goronzy <greg@geekmind.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$

/// @file platform_unix_linux.h
/// @see platform_unix_linux.cpp
/// @ingroup unix

class Platform;

/// @brief Linux values.
class PlatformUnixLinux : public PlatformUnix {
public:
	PlatformUnixLinux() {};
	virtual ~PlatformUnixLinux() {};

	// Hardware
	virtual std::string CPUId();
	virtual std::string CPUSpeed();
	virtual int CPUCores();
	virtual int CPUCount();
	virtual std::string CPUFeatures();
	virtual std::string CPUFeatures2();
	virtual uint64_t Memory();

	// Unix Specific
	virtual std::string UnixLibraries();
private:
    virtual std::string getProcValue(const wxString path, const wxString key);
};
