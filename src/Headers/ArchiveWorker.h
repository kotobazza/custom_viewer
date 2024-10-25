#pragma once

#include "FileManipulations.h"
#include <archive.h>
#include <archive_entry.h>
#include <filesystem>


#include <vector>
#include <string>
#include <string_view>

namespace cv
{


struct ArchiveReadingResults{
    ActionState state;
    std::vector<std::string> entries;
};

class ArchiveWorker{
    static archive_entry* createNewArchiveEntry(const std::string entry);
public:
    // //! Функции работают с "плоскими" архивами!
    // //TODO: эти методы нужно переписывать с нуля, но технически, считается рабочим вариантом
    // //TODO: можно ли их сделать рекурсивными? 
    static ArchiveReadingResults readArchiveEntries(std::string_view path);
    static ActionState createNewZipArchive(std::string_view path, std::vector<std::string> entities);
    static ActionState unzipArchive(std::string_view path, std::string pathToExtract=std::filesystem::current_path().string());
};

}

