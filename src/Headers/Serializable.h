#pragma once

#include "FileManipulations.h"

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>


#include <iostream>

class Dictionary
{
private:
    std::string dictionaryName{};
    int dictionaryId{0};
    std::string dictionaryText{};

public:
    Dictionary(){};
    Dictionary(std::string dictName, int id, std::string src):
        dictionaryName{dictName}, dictionaryId{id}, dictionaryText{src} {};

    std::string getDictionaryName();//
    void setDictionaryName(std::string_view newName);//
    
    int getDictionaryId();//
    void setDictionaryId(int x);

    std::string getDictionaryContent();  //
    void setDictionaryContent(std::string_view content);
    


    boost::property_tree::ptree serialize();//
};


namespace cv
{
struct DictionaryDeserializationResult
{
    ActionState state;
    Dictionary dictionary;
};

}

