

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

#include "Application.h"



int main(){
    using namespace std::string_literals;
    std::cout << "Hello World!"<<std::endl;
    Application a;
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    screen.Loop(a.render());
    
    


    
}