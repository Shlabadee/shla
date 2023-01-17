#ifndef __SHLA_PATH_INCLUDED
#define __SHLA_PATH_INCLUDED

#include <vector>
#include <string>

#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace shla
{
    enum class PATH_TYPE
    {
        FILE,
        DIRECTORY,
        OTHER,
        NONEXISTENT
    };

    inline PATH_TYPE GetPathType(const char* path)
    {
        struct stat sb;

        if (stat(path, &sb) == -1)
        {
            return PATH_TYPE::NONEXISTENT;
        }

        switch (sb.st_mode & S_IFMT)
        {
            case S_IFREG:
                return PATH_TYPE::FILE;
            case S_IFDIR:
                return PATH_TYPE::DIRECTORY;
            default:
                return PATH_TYPE::OTHER;
        }
    }

    inline int GetFilesInDirectory(std::string path, std::vector<std::string> *listing)
    {
        if (GetPathType(path.c_str()) != PATH_TYPE::DIRECTORY)
        {
            return 1;
        }
        
        path += "*";

        WIN32_FIND_DATA data;
        HANDLE hFind = FindFirstFile(path.c_str(), &data);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                listing->push_back(std::string(data.cFileName));
            }
            while (FindNextFile(hFind, &data));
            FindClose(hFind);
            return 0;
        }

        return 2;
    }
    
    inline int CreateDirectory(const char* path)
    {
        if (::CreateDirectory(path, NULL))
        {
            return 0;
        }
        else if (ERROR_ALREADY_EXISTS == GetLastError())
        {
            return 1;
        }
        
        return 2;
    }
}

#endif
