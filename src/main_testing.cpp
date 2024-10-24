

#include <iostream>
// #include <array>       // for array
// #include <chrono>      // for milliseconds
// #include <functional>  // for function
// #include <memory>      // for __shared_ptr_access, shared_ptr, allocator
// #include <string>      // for string, char_traits, operator+, basic_string
// #include <vector>      // for vector


#include "DiskInfo.h"
#include "FileManipulations.h"
#include "Headers/ArchiveWorker.h"
#include "Headers/FileManipulations.h"
#include "Headers/JsonWorker.h"
#include "Headers/Serializable.h"
#include "Headers/XmlWorker.h"
#include "TextWorker.h"
#include "JsonWorker.h"
#include "XmlWorker.h"
#include "ArchiveWorker.h"
#include "Serializable.h"



int main(){
    using namespace std::string_literals;
    
    std::cout << "Hello World!"<<std::endl;
    //diskInfo.printFilesystemInfo();

    // for(cv::DiskInfoRecord a: cv::DiskInfo::getMountedPoints())
    // {
    //     std::cout << "Filesystem Device: " << a.deviceName << "\n";
    //     std::cout << a.fileSystem << "\n";
    //     std::cout << a.mountPoint << "\n";
    //     std::cout << "\n";
    // }


    // // * Создание директории
    // cv::FileManipulation::createNewDirectory("let/a");

    // //* Создание файла
    // cv::FileManipulation::createNewFile("let/a/test.txt");

    // //* Создание файла V2
    // cv::FileManipulation::createNewFile("let/b/test.txt");
    
    // //* Использование ненормированного пути
    // cv::FileManipulation::createNewDirectory("../../../let/test");
    
    // //* Удаление директории
    // cv::FileManipulation::createNewDirectory("let/c");
    // cv::FileManipulation::deletePath("let/c");

    // //* Удаление файла
    // cv::FileManipulation::deletePath("let/b/test.txt");


    // //* Тестирование TextWorker

    // //* Заполнение файла
    // cv::FileManipulation::createNewFile("let/c/test.txt");
    // cv::TextWorker::writeIntoFile("let/c/test.txt", "Hello World! Test 1\n");

    // //* Чтение из файла
    // cv::ReadFileOutput out {cv::TextWorker::readFromFile("let/c/test.txt")};
    // std::cout << "Test 1: Text from file: \n" << out.content<< "\n";

    // //* Тест добавления текста
    // cv::TextWorker::appendIntoFile("let/c/test.txt", "Test 2\n");
    // out = cv::TextWorker::readFromFile("let/c/test.txt");
    // std::cout << "Test 2: Text from file: \n" << out.content<< "\n";

    //* Работа с сериализуемой сущностью
    Dictionary t{};
    t.addNewEntry("Hello", "I'm here!\n");
    t.addNewEntry("Hello1", "I'm here\"\"\"\'\'\'\'\'\'!\n");
    t.addNewEntry("Hello2", "I'm here!\n");


    std::cout<<t.getDictionaryEntryContent("Hello");

    //* Сериализация сущности в XML и JSON
    cv::FileManipulation::createNewFile("let/d/dictionary.json");
    cv::FileManipulation::createNewFile("let/d/dictionary.xml");
    cv::TextWorker::writeIntoFile("let/d/dictionary.json", cv::JsonWorker::serializeDictionary(t));
    cv::TextWorker::writeIntoFile("let/d/dictionintary.xml", cv::XmlWorker::serializeDictionary(t));

    // //* Десериализация сущности из XML и JSON
    // cv::DictionaryDeserializationResult res1 = cv::JsonWorker::deserializeDictionary("let/d/dictionary.json");
    // if(res1.state == cv::ActionState::Done)
    //     std::cout<<"YAY\n";

    // cv::DictionaryDeserializationResult res2 = cv::XmlWorker::deserializeDictionary("let/d/dictionary.xml");
    // if(res2.state == cv::ActionState::Done)
    //     std::cout<<"YAY\n";

    //* Ошибка десериализации
    cv::DictionaryDeserializationResult res3 = cv::JsonWorker::deserializeDictionary("any/dictionary_failed.txt");
    if(res3.state == cv::ActionState::JsonFailed){
        std::cout<<"YAY\n";
    }

    cv::DictionaryDeserializationResult res4 = cv::XmlWorker::deserializeDictionary("any/dictionary_failed.xml");
    if(res4.state == cv::ActionState::XmlFailed){
        std::cout<<"YAY\n";
    }


    // //* Работа с архивами

    // cv::ArchiveWorker::createNewZipArchive("file.zip", {"file.txt"s, "file2.txt"s});
    // cv::ArchiveReadingResults res = cv::ArchiveWorker::readArchiveEntries("file.zip");
    
    // for(const auto& file : res.entries)
    // {
    //     std::cout<<file<<"\n";
    // }

    // cv::ArchiveWorker::unzipArchive("file.zip");









    
}