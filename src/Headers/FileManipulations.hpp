#pragma once 
#include <filesystem>
#include <string>
#include <string_view>



namespace cv{

enum FileType
{
    File,
    Directory,
    Socket,
    Pipe,
    Device
};

class FileInfo
{
    std::string path;
    std::string perms;
    std::string owner;
    std::string includingDirectory;
    FileType type;
};

enum ActionRejected{
    IsDirectory,
    IsFile,
    NotEmpty,
    Empty,
    NoPermissions,

    
};


class FileManipulation
{


private:
    static std::string clearNonRelativePath(std::string path);//

public:
    static bool createNewFile(std::string path);//
    static bool createNewDirectory(std::string path);//
    
    static std::string readFile(std::string path);
    static bool writeIntoFile(std::string path, std::string val);

    static bool deletePath(std::string path);//

    static bool isExistingPath(std::string path);//
    static bool isDirectory(std::string path);//
    static bool isFile(std::string path);//
};


}
