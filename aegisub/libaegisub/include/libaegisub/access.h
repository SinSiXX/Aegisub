// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
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

/// @file access.h
/// @brief Public interface for access methods.
/// @ingroup libaegisub

#include <libaegisub/exception.h>

namespace agi {
	namespace acs {

DEFINE_SIMPLE_EXCEPTION_NOINNER(Fatal, FileSystemError, "filesystem/fatal");
DEFINE_SIMPLE_EXCEPTION_NOINNER(NotAFile, FileSystemError, "filesystem/not_a_file")
DEFINE_SIMPLE_EXCEPTION_NOINNER(NotADirectory, FileSystemError, "filesystem/not_a_directory")

DEFINE_SIMPLE_EXCEPTION_NOINNER(Read, FileNotAccessibleError, "filesystem/not_accessible/read_permission")
DEFINE_SIMPLE_EXCEPTION_NOINNER(Write, FileNotAccessibleError, "filesystem/not_accessible/write_permission")

enum Type {
	FileRead,
	DirRead,
	FileWrite,
	DirWrite
};

void Check(const std::string &file, acs::Type);

void CheckFileRead(const std::string &file);
void CheckDirRead(const std::string &dir);

void CheckFileWrite(const std::string &file);
void CheckDirWrite(const std::string &dir);

	} // namespace axs
} // namespace agi
