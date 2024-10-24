#pragma once

#include "Serializable.h"

#include <string>


namespace cv
{

class XmlWorker
{
public:
    static std::string serializeDictionary(Dictionary dictionary);
    static DictionaryDeserializationResult deserializeDictionary(std::string_view path);

};

}