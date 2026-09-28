#include "pathcompat.h"

#include <algorithm>

#ifndef _WIN32
#include <filesystem>
#include <system_error>

#include <strings.h>

namespace fs = std::filesystem;
#endif

std::string ResolvePath(const char* path)
{
    std::string p(path);
    std::replace(p.begin(), p.end(), '\\', '/');

#ifndef _WIN32
    if (fs::exists(p))
    {
        return p;
    }

    // Walk the components, matching each one case-insensitively
    fs::path resolved;
    for (const fs::path& part : fs::path(p))
    {
        fs::path candidate = resolved / part;
        if (!fs::exists(candidate))
        {
            std::error_code ec;
            for (const fs::directory_entry& e : fs::directory_iterator(resolved.empty() ? "." : resolved, ec))
            {
                if (strcasecmp(e.path().filename().c_str(), part.c_str()) == 0)
                {
                    candidate = resolved / e.path().filename();
                    break;
                }
            }
        }
        resolved = candidate;
    }
    return resolved.string();
#else
    return p;
#endif
}
