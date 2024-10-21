#include "FileManipulations.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <string_view>



namespace cv{

bool FileManipulation::createNewFile(std::string path)
{
    //относительные пути?
    path = FileManipulation::clearNonRelativePath(path);
    std::filesystem::path filePath {path};
    std::filesystem::create_directories(std::filesystem::current_path() / filePath.parent_path());  
    std::ofstream fout {std::filesystem::current_path() / filePath};
    fout.close();

    return true;
} 


bool FileManipulation::createNewDirectory(std::string path)
{
    path = FileManipulation::clearNonRelativePath(path);
    std::filesystem::create_directories(std::filesystem::current_path() / path);
    return true;
}


std::string FileManipulation::clearNonRelativePath(std::string path)
{
    size_t pos = 0;
    while((pos = path.find("../", pos)) != std::string::npos)
    {
        path.erase(pos, 3);
    }
    std::cout << path<<std::endl;
    return path;
}



bool FileManipulation::isExistingPath(std::string path)
{
    return std::filesystem::exists(clearNonRelativePath(path));
}


bool FileManipulation::isFile(std::string path)
{
    return !std::filesystem::is_directory(clearNonRelativePath(path));
}


bool FileManipulation::isDirectory(std::string path)
{
    return std::filesystem::is_directory(clearNonRelativePath(path));
}


bool FileManipulation::deletePath(std::string path)
{
    return std::filesystem::remove_all(clearNonRelativePath(path));
}

}