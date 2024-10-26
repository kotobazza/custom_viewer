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

bool JsonWorker::isValidJson(std::string_view path)
{

    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(std::string(path), pt);
        return true;
    } catch (const boost::property_tree::json_parser_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }

}


std::string JsonWorker::escapeJsonString(std::string input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '/':
                output += "\\/";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Escape control characters
                    char buf[7];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<int>(c));
                    output += buf;
                } else {
                    output += c;
                }
                break;
        }
    }
    return output;
}



}





