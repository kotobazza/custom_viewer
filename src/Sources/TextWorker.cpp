#include "TextWorker.h"
#include <fstream>

namespace cv{


//TODO: дорогое использование std::string(path), нужно сделать дешевле
ReadFileOutput TextWorker::readFromFile(std::string_view path)
{
    std::ifstream f{std::string(path)};

    if(!f.is_open())
        return {ActionState::Unavailable, {}};


    std::string contents;
    f.seekg(0, std::ios::end);
    contents.resize(f.tellg());
    f.seekg(0, std::ios::beg);
    f.read(&contents[0], contents.size());
    f.close();

    return {ActionState::Done, contents};
}

//TODO: дорогое использование std::string(path), нужно сделать дешевле
ActionState TextWorker::appendIntoFile(std::string_view path, std::string_view text)
{
    std::ofstream f(std::string(path), std::ios::app);

    if(!f.is_open())
        return ActionState::Unavailable;

    f << text;

    f.close();
    if(f.fail())
        return ActionState::Failed;



    return ActionState::Done;
}


//TODO: дорогое использование std::string(path), нужно сделать дешевле
ActionState TextWorker::writeIntoFile(std::string_view path, std::string_view text)
{
    std::ofstream f{std::string(path), std::ios::trunc};

    if(!f.is_open())
        return ActionState::Unavailable;

    f << text;
    f.close();

    if(f.fail())
        return ActionState::Failed;

    return ActionState::Done;
}



}