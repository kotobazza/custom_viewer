#include "Application.h"

#include "DiskInfo.h"
#include "FileManipulations.h"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <iterator>
#include <string>

#include <filesystem>


ftxui::Component CText(std::string text) {
  return ftxui::Renderer([text] {  //
    return ftxui::text(text) | ftxui::underlinedDouble;
  });
}



void Application::createDiskUsageTable()
{
    using namespace ftxui;

    diskUsageContainer = Container::Vertical({});

    std::vector<std::vector<Element>> tableValues;

    tableValues.push_back(
        {
            text("Device Name"), 
            text("Device Mount Path"), 
            text("Device Filesystem"), 
            text("Total"), 
            text("Free"), 
            text("% Free<float>"), 
            text("% Used<float>"),
            text("% Free<gauge>"),
            text("% Used<gauge>")
        }
    );

    for (const cv::DiskInfoRecord& record : cv::DiskInfo::getMountedPoints())
    {
        // if(record.fileSystem=="tmpfs")
        //     continue;
        unsigned long used = record.totalMemory - record.freeMemory;
        float pFree = record.totalMemory ? static_cast<float>(record.freeMemory)/record.totalMemory*100 : 0.0f;
        float pUsed = record.totalMemory ? static_cast<float>(used)/record.totalMemory*100 : 0.f;

        tableValues.push_back(
            {
                text(record.deviceName) | center, 
                text(record.mountPoint) | center, 
                text(record.fileSystem) | center, 
                text(std::to_string(record.totalMemory) + " bytes") | center, 
                text(std::to_string(record.freeMemory) + " bytes") | center,

                text(std::to_string(pUsed)) | center,
                text(std::to_string(pFree)) | center,

                gaugeRight(pUsed/100),
                gaugeRight(pFree/100)
            }
        );
    }

    auto table = Table(tableValues);

    table.SelectAll().Border(LIGHT);
    table.SelectColumn(7).Border();
    table.SelectColumn(8).Border();
    table.SelectColumn(0).Border(LIGHT);
 
    // Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);

    mainTable = table.Render();

    diskUsageContainer = Renderer(diskUsageContainer, [=]{
        return vbox({
            mainTable | flex
        });
    });

}

void Application::createFilesystemContainer(std::string path)
{

    using namespace ftxui;
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);

    filesystemMenu = Menu(&dirEntries, &filesystemMenuSelected);
    
    filesystemLeftPannel = Container::Vertical({
        filesystemMenu
    });


    filesystemRightPannel = Container::Vertical({});

    filesystemLeftPannel = ResizableSplitLeft(filesystemLeftPannel, filesystemRightPannel, &leftPannelSizeModifier);

    filesystemUsageContainer = Container::Vertical({
        filesystemLeftPannel,
        filesystemDownPannel
    });
}

void Application::createTextWorksContainer(std::string path)
{

    using namespace ftxui;
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    
    textworksLeftPannel = Container::Vertical({
        Menu(&dirEntries, &filesystemMenuSelected)
    });


    textworksRightPannel = Container::Vertical({});

    textworksLeftPannel = ResizableSplitLeft(textworksLeftPannel, textworksRightPannel, &leftPannelSizeModifier);

    textUsageContainer = Container::Vertical({
        textworksLeftPannel,
        textworksDownPannel
    });
}

void Application::createDictionaryContainer(std::string path)
{

    using namespace ftxui;
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    
    dictionaryLeftPannel = Container::Vertical({
        Menu(&dirEntries, &filesystemMenuSelected)
    });


    dictionaryRightPannel = Container::Vertical({});

    dictionaryLeftPannel = ResizableSplitLeft(dictionaryLeftPannel, dictionaryRightPannel, &leftPannelSizeModifier);

    dictionaryUsageContainer = Container::Vertical({
        dictionaryLeftPannel,
        dictionaryDownPannel
    });
}










void Application::createDownPannel()
{
    using namespace ftxui;
    mainPathInput = Input(&mainPathInputVal, "/") | borderRounded | flex;

    pathInputEnter = Button("Enter", [=]{});
    pathInputCreate = Button("Create", [=]{});
    pathInputDelete = Button("Delete", [=]{});

    filesystemDownPannel = Container::Horizontal({
        mainPathInput,
        pathInputEnter,
        pathInputCreate,
        pathInputDelete
    });
}



Application::Application()
{
    using namespace ftxui;
    


    tabToggle = Toggle(&tabValues, &tabSelected) | borderRounded;

    

    createDiskUsageTable();



    textworksDownPannel = Container::Vertical({});
    dictionaryDownPannel = Container::Vertical({});


    createDownPannel();

    createFilesystemContainer(std::filesystem::current_path().string());
    createTextWorksContainer(std::filesystem::current_path().string());
    createDictionaryContainer(std::filesystem::current_path().string());






    tabContainer = Container::Tab({
        diskUsageContainer,
        filesystemUsageContainer,
        textUsageContainer,
        dictionaryUsageContainer
    }, &tabSelected);


    mainContainer = Container::Vertical({
        tabToggle,
        tabContainer
    });
}





ftxui::Component Application::render()
{
    using namespace ftxui;
    return Renderer(mainContainer, [&]{
        return vbox({
            tabToggle->Render(),
            tabContainer->Render(),
        });
    });
}



