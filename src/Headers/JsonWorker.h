#pragma once

#include "Serializable.h"

#include <string>
#include <string_view>



namespace cv
{

class JsonWorker
{
public:
    static std::string serializeDictionary(Dictionary dictionary);
    static DictionaryDeserializationResult deserializeDictionary(std::string_view path);

};

}
