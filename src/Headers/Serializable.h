#pragma once

#include "FileManipulations.h"

#include <string>
#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>


#include <iostream>

class Dictionary
{
private:
    std::string dictionaryName{};
    int dictionaryId{0};
    std::map<std::string, std::string> source{};

public:
    Dictionary(){};
    Dictionary(std::string dictName, int id, std::map<std::string, std::string> src):
        dictionaryName{dictName}, dictionaryId{id}, source{src} {};

    std::string getDictionaryName();//
    int getDictionaryId();//
    void setDictionaryId(int x);
    void addNewEntry(std::string name, std::string content);//
    std::vector<std::string> listEntries();//
    std::string getDictionaryEntryContent(std::string entryName);  //
    void setDictionaryName(std::string_view newName);//
    bool setNewEntryContent(std::string_view entryName, std::string content);//

    bool removeEntry(std::string_view entryName);

    
    

    
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

