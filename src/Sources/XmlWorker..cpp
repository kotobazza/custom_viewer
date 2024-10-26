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
            ActionState::StructFailed,
            Dictionary{}
        };
    }

    int id = 0;
    if(pt.get_optional<int>("dictionary.dictionary_id"))
        id = pt.get<int>("dictionary.dictionary_id");
    else
        return DictionaryDeserializationResult{
            ActionState::ParseFailed,
            Dictionary{}
        };

    std::string name;
    if(pt.get_optional<std::string>("dictionary.name"))
        name = pt.get<std::string>("dictionary.name");
    else
        return DictionaryDeserializationResult{
            ActionState::ParseFailed,
            Dictionary{}
        };

    std::string source;
    if(pt.get_optional<std::string>("dictionary.source"))
        source = pt.get<std::string>("dictionary.source");
    else
        return DictionaryDeserializationResult{
            ActionState::ParseFailed,
            Dictionary{}
        };


    return DictionaryDeserializationResult{
        ActionState::Done,
        Dictionary{
            name,
            id,
            source
        }
    };


}

bool XmlWorker::isValidXml(std::string_view path)
{
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_xml(std::string(path), pt);
        return true;
    } catch (const boost::property_tree::xml_parser_error&) {
        return false;
    }

}

}
