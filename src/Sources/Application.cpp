#include "Application.h"

#include "DiskInfo.h"
#include "FileManipulations.h"
#include "TextWorker.h"
#include "ArchiveWorker.h"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/screen/color.hpp"
#include <iterator>
#include <string>
#include <iostream>

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

      // Select row from the second to the last.
    auto content = table.SelectRows(1, -1);
    // Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    mainTable = table.Render();

    diskUsageContainer = Renderer(diskUsageContainer, [=]{
        return vbox({
            mainTable | flex
        });
    });

}


void Application::openPath(std::string path)
{
    if(cv::FileManipulation::isDirectory(path))
    {
        dirEntries.clear();
        dirEntries = cv::FileManipulation::getDirectoryEntries(path);
        usingPath = path;   
        clearPlainText();
        commandExecutionString = "Opened directory: "+std::filesystem::path(path).filename().string();
        return;
    }

    else if(cv::FileManipulation::isArchive(path))
    {
        cv::ArchiveReadingResults output = cv::ArchiveWorker::readArchiveEntries(path);
        if(output.state != cv::ActionState::Done)
        {
            commandExecutionString = "ERROR: can't read provided archive: "+path;
            return;
        }

        std::string content = "Reading archive includes...\nArchive [" + path + "]:\n";

        for(auto& ent: output.entries)
        {
            content += ent + "\n";
        }
        
        commandExecutionString = "Opened archive: "+std::filesystem::path(path).filename().string();
        
        

        if(output.entries.size()==0)
        {
            commandExecutionString = "Opened empty archive: "+std::filesystem::path(path).filename().string();
        }

        plainTextString = content;
        openedFilePath = path;
        isPathArchive=true;

    }

    else if(cv::FileManipulation::isFile(path))
    {        
        cv::ReadFileOutput output = cv::TextWorker::readFromFile(path);
        if(output.state != cv::ActionState::Done)
        {
            commandExecutionString = "ERROR: can't read provided file: "+path;
            return;
        }

        plainTextString = output.content;
        commandExecutionString = "Opened file:"+std::filesystem::path(path).filename().string();;
        if(output.content=="")
            commandExecutionString = "Opened empty file: " + std::filesystem::path(path).filename().string();
        isPathFile=true;
        openedFilePath = path;


    }
    else:
        commandExecutionString = "ERROR: wrong path/no typeof {Directiry, Archive, Text file}: "+path;
}


void Application::saveFileText()
{
    if(cv::FileManipulation::isExistingPath(openedFilePath) && cv::FileManipulation::isFile(openedFilePath))
    {
        cv::TextWorker::writeIntoFile(openedFilePath, plainTextString);
    }
    else
    {
        commandExecutionString = "File wasn't saved properly...";
        modalFileCanNotBeOverwritten=true;
    }
}


void Application::moveFileMenuToPath(std::string path)
{
    dirEntries.clear();
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    usingPath = path;
    openedFilePath = "";
    clearPlainText();
}


void Application::clearPlainText()
{
    plainTextString = "";
    isPathFile = false;
    isPathArchive = false;
}

void Application::openParentPath(std::string path)
{
    moveFileMenuToPath(path);
    clearPlainText();
    commandExecutionString="Opened folder: "+ std::filesystem::path(path).parent_path().string();
}

void Application::unzipArchive()
{
    cv::ActionState output = cv::ArchiveWorker::unzipArchive(openedFilePath);
    if(output == cv::ActionState::Done)
        commandExecutionString = "Unzipped archive: "+std::filesystem::path(openedFilePath).filename().string();
    else
        commandExecutionString = "ERROR: can't unzip provided archive: "+openedFilePath;

    moveFileMenuToPath(usingPath);
}



void Application::createFilesystemContainer(std::string path)
{

    using namespace ftxui;
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);

    filesystemMenu = Menu(&dirEntries, &filesystemMenuSelected);
        
    filesystemMenu |= CatchEvent([&](Event event){
            bool ret = (Event::Character("\n")==event | Event::ArrowRight==event);
            if(ret)
            {
                std::string selectedPath = usingPath+'/'+dirEntries[filesystemMenuSelected];
                openPath(selectedPath);
                
            }

            return ret;
        });

    filesystemMenu |= CatchEvent([&](Event event){
            bool ret = (Event::ArrowLeft==event);
            if(ret)
            {
                openParentPath(std::filesystem::path(usingPath).parent_path().string());
            }
            return ret;
        }); 

    filesystemMenu|= CatchEvent([&](Event event){
            bool ret = (Event::Delete == event);
            if(ret) modalSureToDelete = true;
            return ret;
        });

    commandExecutionString="Open some file...";
    
    filesystemLeftPannel = Container::Vertical({
        Renderer([&]{
            return text("#>"+usingPath)|underlined|bold|bgcolor(Color::GreenLight);
        }),
        Renderer([&]{return separator();}),
        filesystemMenu
    });

    filesystemRightPannel = Container::Vertical({
        Input(&plainTextString)|borderEmpty,
        Renderer([]{return filler();}),
        Renderer([&]{
            return text(commandExecutionString) | underlined | bgcolor(Color::GrayLight);
        }),
        filesystemDownPannel
    });

    filesystemLeftPannel = ResizableSplitLeft(filesystemLeftPannel, filesystemRightPannel, &leftPannelSizeModifier);

    filesystemUsageContainer = Container::Vertical({
        filesystemLeftPannel
    });

    filesystemDownPannel = Container::Horizontal({
        Input(&mainPathInputVal, "/") | borderRounded | flex,
        Container::Vertical({
            Button("Enter", [&]{
                if(mainPathInputVal==""){
                    commandExecutionString="Enter some path into input string...";
                    return;
                }
                std::string selectedPath = usingPath + "/" + mainPathInputVal;
                openPath(selectedPath);

                mainPathInputVal = "";
            }),
            Button("<-", [&]{
                openParentPath(std::filesystem::path(usingPath).parent_path().string());
            })
        }),
        Container::Vertical({
            Button("Create Directory", [&]{
                if(!cv::FileManipulation::isExistingPath(usingPath + "/"+mainPathInputVal)){
                    cv::FileManipulation::createNewDirectory(usingPath + "/"+mainPathInputVal);
                    moveFileMenuToPath(usingPath);
                    commandExecutionString="Created new directory: "+usingPath + "/"+mainPathInputVal;
                }
            }),
            Button("Create File", [&]{
                if(!cv::FileManipulation::isExistingPath(usingPath + "/" + mainPathInputVal)){
                    cv::FileManipulation::createNewFile(usingPath + "/" + mainPathInputVal);
                    moveFileMenuToPath(usingPath);
                    commandExecutionString="Created new file: "+usingPath + "/"+mainPathInputVal;
                }
            })
        }),
        Container::Vertical({
                    Button("Delete", [&]{
                if(cv::FileManipulation::isExistingPath(usingPath + "/" + mainPathInputVal)){
                    modalSureToDelete=true;
                }
            }),
            Button("Create archive", [&]{})
            

        }),
        Container::Vertical({
            Maybe(Button("Save", [&]{
                modalSureToWrite=true;
            }), &isPathFile),
            Maybe(Button("Unzip archive", [&]{
                modalSureToUnzip=true;
            }), &isPathArchive)
        })
    });
}


void Application::createDictionaryContainer(std::string path)
{

    using namespace ftxui;
    dictionaryDownPannel = Container::Horizontal({
        
    })
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    
    dictionaryLeftPannel = Container::Vertical({
        Renderer([&]{
            return text("#>"+usingPath)|underlined|bold|bgcolor(Color::GreenLight);
        }),
        Renderer([&]{return separator();}),
        Menu(&dirEntries, &filesystemMenuSelected)
    });

    dictionaryRightPannel = Container::Vertical({
        Renderer([&]{
            return filler();
        }),
        dictionaryDownPannel
    });

    dictionaryLeftPannel = ResizableSplitLeft(dictionaryLeftPannel, dictionaryRightPannel, &leftPannelSizeModifier);

    dictionaryUsageContainer = Container::Vertical({
        dictionaryLeftPannel
        
    });
}


Application::Application(std::function<void()>exitor)
{
    using namespace ftxui;

    exitorClosure=exitor;

    tabToggle = Toggle(&tabValues, &tabSelected) | borderRounded;


    createDiskUsageTable();
    std::cout<< "diskusagetable_create\n";

    createFilesystemContainer(std::filesystem::current_path().string());
    std::cout<< "filesystemcontainer_create\n";

    createDictionaryContainer(std::filesystem::current_path().string());
    std::cout<<"dictionarycontainer_create\n";


    tabContainer = Container::Tab({
        diskUsageContainer,
        filesystemUsageContainer,
        dictionaryUsageContainer
    }, &tabSelected);


    mainContainer = Container::Vertical({
        Container::Horizontal({
            tabToggle|flex,
            Button("X", [&]{
                exitorClosure();
            }) | bold
        }),
        tabContainer
    })
    | Modal(Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Undeniable action")|hcenter|bold,
                text("You sure you want\nto delete this path?"),
                text(usingPath + "/" + mainPathInputVal)|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Anyway",[&]{
                cv::FileManipulation::deletePath(usingPath + "/" + mainPathInputVal);
                commandExecutionString="Deleted path: "+ usingPath + "/" + mainPathInputVal;
                moveFileMenuToPath(usingPath);
                modalSureToDelete=false;
                mainPathInputVal="";
                
            }),
            Button("Cancel",[&]{
                modalSureToDelete=false;
                mainPathInputVal="";
            })
        })
    }), &modalSureToDelete)
    | Modal(Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Can't overwrite a file")|hcenter|bold,
                text("This file cannot be overwritten. The file's content will be dropped out."),
                text(openedFilePath)|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Okay",[&]{
                modalFileCanNotBeOverwritten=false;
                clearPlainText();
            })
        })
    }), &modalFileCanNotBeOverwritten)
    | Modal(Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Undeniable action")|hcenter|bold,
                text("You sure you want\nto rewrite this file?"),
                text(openedFilePath)|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Anyway",[&]{
                saveFileText();
                commandExecutionString = "Saving text to: " + openedFilePath;
                modalSureToWrite=false;
                mainPathInputVal="";
            }),
            Button("Cancel",[&]{
                modalSureToWrite=false;
                mainPathInputVal="";
            })
        })
    }), &modalSureToWrite)
    | Modal(Container::Vertical({
        Renderer([&]{
            std::string will{};
            std::string newPath = usingPath + "/" +  cv::FileManipulation::transformPathFromArchiveToFolder(openedFilePath);
            if(cv::FileManipulation::isExistingPath(newPath))
                will = "refill the existing directiry";
            else
                will = "create new directory";
            return vbox({
                text("Undeniable action")|hcenter|bold,
                text("You sure you want\nto unzip this archive?"),
                text(openedFilePath)|color(Color::Blue),
                text("It will " + will) |color(Color::Cyan),
                text("creating this new path: "+ newPath)
            });
        }),
        Container::Horizontal({
            Button("Anyway",[&]{
                unzipArchive();
                commandExecutionString = "Unzipping archive to folder: " + openedFilePath;
                modalSureToUnzip=false;
            }),
            Button("Cancel",[&]{
                modalSureToUnzip=false;
            })
        })
    }), &modalSureToUnzip);
    
}





ftxui::Component Application::render()
{
    using namespace ftxui;
    return mainContainer;
    
}

