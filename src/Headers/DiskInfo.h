#pragma once
#include <sys/statvfs.h>
#include <mntent.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

namespace cv{


struct DiskInfoRecord{
    std::string deviceName{};
    std::string mountPoint{};
    std::string fileSystem{};
    unsigned long int totalMemory{0};
    unsigned long int freeMemory{0};
};



class DiskInfo{
public:
    static void printFilesystemInfo();
    static std::vector<DiskInfoRecord> getMountedPoints();

};

}