#include "Serializable.h"

int Dictionary::getDictionaryId()
{
    return dictionaryId;
}

void Dictionary::setDictionaryId(int x)
{
    dictionaryId = x;
}

std::string Dictionary::getDictionaryName()
{
    return dictionaryName;
}

void Dictionary::setDictionaryName(std::string_view newName)
{
    dictionaryName = std::string(newName);
}


void Dictionary::addNewEntry(std::string name, std::string content)
{
    if(source.find(name) != source.end())
        return;

    source[name]=content;
}

bool Dictionary::setNewEntryContent(std::string_view entryName, std::string content)
{
    if(source.find(std::string(entryName)) == source.end())
        return false;

    source[std::string(entryName)] = content;
    return true;

}


bool Dictionary::removeEntry(std::string_view entryName)
{
    if(source.erase(std::string(entryName))>0)
    {
        return true;
    }
    return false;
}


std::vector<std::string> Dictionary::listEntries()
{
    std::vector<std::string> res{};
    for(auto const& ent: source)
        res.push_back(ent.first);

    return res;
}

std::string Dictionary::getDictionaryEntryContent(std::string name)
{
    if(source.find(name) == source.end())
        return {};
    return source[name];
}


boost::property_tree::ptree Dictionary::serialize()
    {
        boost::property_tree::ptree pt{};
        pt.put("dictionary.name", dictionaryName);
        pt.put("dictionary.dictionary_id", dictionaryId);


        for(auto const&[name, content] : source)
        {
            pt.put("dictionary.source."+name, content);
        }

        return pt;
    }