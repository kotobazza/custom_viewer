#pragma once
#include <sys/statvfs.h>
#include <mntent.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

namespace cv{


class DiskInfoRecord{
public:
    std::string deviceName{};
    std::string mountPoint{};
    std::string fileSystem{};
    uint totalMemory{0};
    uint freeMemory{0};

    DiskInfoRecord(std::string name, std::string m_point, std::string fs, uint total, uint free) :
        deviceName{name}, mountPoint{m_point}, fileSystem{fs}, totalMemory(total), freeMemory(free) 
    {}
};




class DiskInfo{
private:

public:
    static void printFilesystemInfo();
    static std::vector<DiskInfoRecord> getMountedPoints();

};

}