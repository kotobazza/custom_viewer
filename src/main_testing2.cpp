

#include <iostream>
// #include <array>       // for array
// #include <chrono>      // for milliseconds
// #include <functional>  // for function
// #include <memory>      // for __shared_ptr_access, shared_ptr, allocator
// #include <string>      // for string, char_traits, operator+, basic_string
// #include <vector>      // for vector

#include "ArchiveWorker.h"
#include "DiskInfo.h"
#include "FileManipulations.h"
#include "Headers/ArchiveWorker.h"
#include "Headers/FileManipulations.h"
#include "Headers/JsonWorker.h"
#include "Headers/Serializable.h"
#include "Headers/XmlWorker.h"
#include "JsonWorker.h"
#include "Serializable.h"
#include "TextWorker.h"
#include "XmlWorker.h"

#include "Application.h"
auto screen = ftxui::ScreenInteractive::Fullscreen();

void exitor()
{
    screen.Exit();
}

int main()
{
    using namespace std::string_literals;
    std::cout << "Hello World!" << std::endl;

    Application a(exitor);

    screen.Loop(a.render());
}
