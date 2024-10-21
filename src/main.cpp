#include <iostream>
#include <array>       // for array
#include <chrono>      // for milliseconds
#include <functional>  // for function
#include <memory>      // for __shared_ptr_access, shared_ptr, allocator
#include <string>      // for string, char_traits, operator+, basic_string
#include <vector>      // for vector
#include <archive.h>
#include <archive_entry.h>
//#include <stdio.h>
#include <filesystem>
#include <sys/statvfs.h>
#include <mntent.h>
#include <cstdio>
#include <cstring>

#include <fstream>





#include "DiskInfo.hpp"
#include "FileManipulations.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>




int main(){
    std::cout << "Hello World!"<<std::endl;
    cv::DiskInfo diskInfo;
    //diskInfo.printFilesystemInfo();

    for(cv::DiskInfoRecord a: diskInfo.getMountedPoints()){
        std::cout << a.deviceName<<std::endl;
        std::cout << a.fileSystem <<std::endl;
        std::cout << a.mountPoint <<std::endl;
        std::cout << std::endl;
    }

    cv::FileManipulation fworker;
    fworker.createNewFile("let/a/test.txt");
    fworker.createNewDirectory("led/test");
    fworker.createNewDirectory("../../led/test2");


    fworker.deletePath("led");
    std::cout<<"path deleted" << std::endl;
    
}