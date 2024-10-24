#include "XmlWorker.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>

namespace cv{

std::string XmlWorker::serializeDictionary(Dictionary dictionary)
{
    boost::property_tree::ptree pt = dictionary.serialize();
    std::stringstream ss;
    boost::property_tree::write_xml(ss, pt);
    std::string jsonString = ss.str();
    return jsonString;
}

DictionaryDeserializationResult XmlWorker::deserializeDictionary(std::string_view path)
{
    boost::property_tree::ptree pt;
    
    try{
        boost::property_tree::read_xml(std::string(path), pt);
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return DictionaryDeserializationResult{
            ActionState::XmlFailed,
            Dictionary{}
        };
    }

    int id = 0;
    if(pt.get_optional<int>("dictionary.dictionary_id"))
        id = pt.get<int>("dictionary.dictionary_id");
    else
        return DictionaryDeserializationResult{
            ActionState::XmlFailed,
            Dictionary{}
        };

    if(pt.count("dictionary.source")==0)
        return DictionaryDeserializationResult{
            ActionState::XmlFailed,
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
