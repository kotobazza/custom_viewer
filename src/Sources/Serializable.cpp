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

std::string Dictionary::getDictionaryContent()
{
    return dictionaryText;
}

void Dictionary::setDictionaryContent(std::string_view content)
{
    dictionaryText = std::string(content);
}



boost::property_tree::ptree Dictionary::serialize()
{
    boost::property_tree::ptree pt{};
    pt.put("dictionary.name", dictionaryName);
    pt.put("dictionary.dictionary_id", dictionaryId);
    pt.put("dictionary.source", dictionaryText);

    return pt;
}