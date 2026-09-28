// Resolves a path the way Windows would: backslash separators and case-insensitive
// names. Used for paths that come from the game's data files (e.g. "sfx\fbfire.wav").

#ifndef __PATHCOMPAT_H__
#define __PATHCOMPAT_H__

#include <string>

std::string ResolvePath(const char* path);

#endif
