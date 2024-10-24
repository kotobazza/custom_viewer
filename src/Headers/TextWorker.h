#pragma once
#include "FileManipulations.h"
#include <string>
#include <string_view>


namespace cv
{


struct ReadFileOutput{
    ActionState state;
    std::string content;
};

class TextWorker
{
public:
    static ReadFileOutput readFromFile(std::string_view path);
    static ActionState writeIntoFile(std::string_view path, std::string_view text);
    static ActionState appendIntoFile(std::string_view path, std::string_view text);

};

}