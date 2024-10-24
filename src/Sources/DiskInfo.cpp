#include "DiskInfo.h"
#include <mntent.h>
#include <iostream>

namespace cv{

void DiskInfo::printFilesystemInfo() 
{
    FILE *mounts = setmntent("/proc/mounts", "r");
    if (mounts == nullptr) {
        perror("setmntent");
        return;
    }

    struct mntent *mnt;
    while ((mnt = getmntent(mounts)) != nullptr) {
        struct statvfs stat;
        if (statvfs(mnt->mnt_dir, &stat) == 0) {
            unsigned long total = stat.f_blocks * stat.f_frsize;
            unsigned long free = stat.f_bfree * stat.f_frsize;
            unsigned long used = total - free;

            std::cout << "Устройство: " << mnt->mnt_fsname << std::endl;
            std::cout << "Точка монтирования: " << mnt->mnt_dir << std::endl;
            std::cout << "Файловая система: " << mnt->mnt_type << std::endl;
            std::cout << "Полная память: " << total / (1024 * 1024) << " MB" << std::endl;
            std::cout << "Свободная память: " << free / (1024 * 1024) << " MB" << std::endl;
            std::cout << "Используемая память: " << used / (1024 * 1024) << " MB" << std::endl;
            std::cout << "-----------------------------------" << std::endl;
        } else {
            perror("statvfs");
        }
    }

    endmntent(mounts);
}



std::vector<DiskInfoRecord> DiskInfo::getMountedPoints()
{
    FILE *mounts = setmntent("/proc/mounts", "r");
    if (mounts == nullptr) {
        perror("setmntent");
        return {};
    }

    std::vector<DiskInfoRecord> records{};

    struct mntent *mnt;
    while ((mnt = getmntent(mounts)) != nullptr) {
        struct statvfs stat;
        if (statvfs(mnt->mnt_dir, &stat) == 0) {
            unsigned long total = stat.f_blocks * stat.f_frsize;
            unsigned long free = stat.f_bfree * stat.f_frsize;

            DiskInfoRecord record = DiskInfoRecord{
                mnt->mnt_fsname, 
                mnt->mnt_dir,
                mnt->mnt_type,
                total,
                free
            };


            records.push_back(record);
        } else {
            perror("statvfs");
        }
    }

    return records;
}


}