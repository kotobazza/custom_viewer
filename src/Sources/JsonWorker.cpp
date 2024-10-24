#include "JsonWorker.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace cv{

std::string JsonWorker::serializeDictionary(Dictionary dictionary)
{
    boost::property_tree::ptree pt = dictionary.serialize();
    std::stringstream ss;
    boost::property_tree::write_json(ss, pt);
    std::string jsonString = ss.str();
    return jsonString;
}


DictionaryDeserializationResult JsonWorker::deserializeDictionary(std::string_view path)
{
    boost::property_tree::ptree pt;

    try{
        boost::property_tree::read_json(std::string(path), pt);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return DictionaryDeserializationResult{
            ActionState::JsonFailed,
            Dictionary{}
        };
    }


    int id = 0;
    if(pt.get_optional<int>("dictionary.entries_count"))
        id = pt.get<int>("dictionary.dictionary_id");
    else
        return DictionaryDeserializationResult{
            ActionState::JsonFailed,
            Dictionary{}
        };


    if(pt.count("dictionary.source")==0)
        return DictionaryDeserializationResult{
            ActionState::JsonFailed,
            Dictionary{}
        };

    std::map<std::string, std::string> source{};

    for (const auto& pair : pt.get_child("dictionary.source")) {
        source[pair.first] = pair.second.get_value<std::string>();
    }


    return DictionaryDeserializationResult{
        ActionState::Done,
        Dictionary{
            pt.get<std::string>("dictionary.name"),
            id,
            source
        }
    };


}

}





