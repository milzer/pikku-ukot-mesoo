// MSVC <io.h> directory search (_findfirst/_findnext/_findclose) for POSIX systems.
// Matches like Windows does: case-insensitive, results in name order.

#ifndef __POSIX_IO_H__
#define __POSIX_IO_H__

#include <dirent.h>
#include <fnmatch.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include <algorithm>
#include <string>
#include <vector>

struct _finddata_t
{
    char name[260];
};

struct _FindHandle
{
    std::vector<std::string> names;
    size_t next;
};

inline int _findfill(_FindHandle* h, _finddata_t* data)
{
    if (h->next >= h->names.size())
    {
        return -1;
    }
    strncpy(data->name, h->names[h->next++].c_str(), sizeof(data->name) - 1);
    data->name[sizeof(data->name) - 1] = 0;
    return 0;
}

inline intptr_t _findfirst(const char* filespec, _finddata_t* data)
{
    std::string spec(filespec);
    std::replace(spec.begin(), spec.end(), '\\', '/');

    size_t slash = spec.rfind('/');
    std::string dir = slash == std::string::npos ? "." : spec.substr(0, slash);
    std::string pattern = slash == std::string::npos ? spec : spec.substr(slash + 1);

    DIR* d = opendir(dir.c_str());
    if (!d)
    {
        return -1;
    }

    _FindHandle* h = new _FindHandle();
    h->next = 0;
    while (dirent* e = readdir(d))
    {
        if (fnmatch(pattern.c_str(), e->d_name, FNM_CASEFOLD) == 0)
        {
            h->names.push_back(e->d_name);
        }
    }
    closedir(d);

    std::sort(h->names.begin(), h->names.end(), [](const std::string & a, const std::string & b)
    {
        return strcasecmp(a.c_str(), b.c_str()) < 0;
    });

    if (_findfill(h, data) != 0)
    {
        delete h;
        return -1;
    }
    return (intptr_t)h;
}

inline int _findnext(intptr_t handle, _finddata_t* data)
{
    return _findfill((_FindHandle*)handle, data);
}

inline int _findclose(intptr_t handle)
{
    delete (_FindHandle*)handle;
    return 0;
}

#endif
