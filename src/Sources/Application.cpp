#include "Application.h"

#include "DiskInfo.h"
#include "FileManipulations.h"
#include "TextWorker.h"
#include "JsonWorker.h"
#include "XmlWorker.h"
#include "ArchiveWorker.h"
#include "Serializable.h"
#include <archive.h>
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/screen/color.hpp"
#include <iterator>
#include <string>
#include <iostream>

#include <filesystem>
#include <cstdint>
#include <sstream>

using namespace ftxui;



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

void Application::openDirectory(std::string path)
{
    dirEntries.clear();
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    usingPath = path;   
    clearPlainText();
    commandExecutionString = "Opened directory: "+std::filesystem::path(path).filename().string();
    dictionaryCommandExecutionString = "Opened directory: "+std::filesystem::path(path).filename().string();
}

void Application::showArchive(std::string path)
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



void Application::openPath(std::string path)
{
    if(cv::FileManipulation::isDirectory(path))
    {
        openDirectory(path);
    }

    else if(cv::FileManipulation::isArchive(path))
    {
        showArchive(path);

    }
    else if(cv::FileManipulation::isFile(path))
    {        
        

        cv::ReadFileOutput output = cv::TextWorker::readFromFile(path);
        if(output.state != cv::ActionState::Done)
        {
            commandExecutionString = "ERROR: can't read provided file: "+path;
            dictionaryCommandExecutionString = "ERROR: can't read provided file: "+path;
            return;
        }


    

        if(cv::JsonWorker::isValidJson(path) || cv::XmlWorker::isValidXml(path))
        {
            cv::DictionaryDeserializationResult serializable;
            bool isJson=false;
            if(cv::JsonWorker::isValidJson(path))
            {
                serializable = cv::JsonWorker::deserializeDictionary(path);
                isJson=true;
            }
            else 
                serializable = cv::XmlWorker::deserializeDictionary(path);
            
            dictionaryCommandExecutionString = "Checking is " + std::string(isJson?"JSON":"XML") + " file accessible for Dictionary format. Deserialization " + std::string(serializable.state== cv::ActionState::Done ? "Done" : "Undone");
 
            if(serializable.state == cv::ActionState::Done)
            {
                
                Dictionary dict = serializable.dictionary;

                dictionaryCommandExecutionString = std::string("Opened ")+ (isJson ? "JSON":"XML") + " dictionary: " +std::filesystem::path(path).filename().string();
                commandExecutionString = std::string("Found Dictionary. Opened ")+(isJson ? "JSON":"XML") + " in Dictionary tab: " + std::filesystem::path(path).filename().string();
            
                dictionaryIdInputVal = std::to_string(dict.getDictionaryId());
                dictionaryNameInputVal = dict.getDictionaryName();
                dictionaryContentInputVal = dict.getDictionaryContent();
                isPathDictionary=true;
                openedFilePath = path;
                return;
                
            }
            //dictionaryCommandExecutionString = std::string(isJson?"JSON":"XML") + " file has wrong structure for a dictionary. Opened in Filesystem";
            
        }
        
        commandExecutionString = "Opened file:"+std::filesystem::path(path).filename().string();
        plainTextString = output.content;
        if(output.content=="")
            commandExecutionString = "Opened empty file: " + std::filesystem::path(path).filename().string();
        isPathFile=true;
        openedFilePath = path;

    }
    else
    {
        commandExecutionString = "ERROR: wrong path/no typeof {Directiry, Archive, Text file}: "+path;
        dictionaryCommandExecutionString = "ERROR: wrong path/no typeof {Directiry, Archive, Text file}: "+path;
    }
        
}


void Application::saveFileText()
{
    if(cv::FileManipulation::isExistingPath(openedFilePath) && cv::FileManipulation::isFile(openedFilePath))
    {
        cv::TextWorker::writeIntoFile(openedFilePath, plainTextString);
        commandExecutionString = "File saved into: "+openedFilePath; 
    }
    else
    {
        commandExecutionString = "File wasn't saved properly...";
        dictionaryCommandExecutionString = "File wasn't saved properly...";
        modalFileCanNotBeOverwritten=true;
    }
}


void Application::moveFileMenuToPath(std::string path)
{
    dirEntries.clear();
    dirEntries = std::vector<std::string>{};
    auto dirContainig = cv::FileManipulation::getDirectoryEntries(path);
    std::copy(dirContainig.begin(), dirContainig.end(), std::back_inserter(dirEntries));

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
    dictionaryCommandExecutionString="Opened folder: "+ std::filesystem::path(path).parent_path().string();
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


void Application::zipArchive()
{
    std::string newPath = openedFilePath+".zip";
    if(cv::ArchiveWorker::createNewZipArchive(newPath ,openedFilePath))
        archiveCommandExecutionString = "Zipped file: " +std::filesystem::path(newPath).filename().string();
    else
       commandExecutionString = "ERROR: can't zip provided folder: "+openedFilePath;

    moveFileMenuToPath(usingPath);
    
    
}


void Application::createFilesystemContainer(std::string path)
{

    using namespace ftxui;
    dirEntries = cv::FileManipulation::getDirectoryEntries(path);



    commandExecutionString="Open some file...";



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
                if(usingPath!=startCurrentPath)
                    openParentPath(std::filesystem::path(usingPath).parent_path().string());
                else{
                    dictionaryCommandExecutionString="Can't open higher hierarchy folder...";
                    commandExecutionString="Can't open higher hierarchy folder...";
                }
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
                    openedFilePath = usingPath+'/'+dirEntries[filesystemMenuSelected];
                    modalSureToDelete=true;
                    mainPathInputVal="";
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

    filesystemRightPannel = Container::Vertical({
        Input(&plainTextString)|borderEmpty,
        Renderer([]{return filler();}),
        Renderer([&]{
            return text(commandExecutionString) | underlined | bgcolor(Color::GrayLight);
        }),
        filesystemDownPannel
    });



    filesystemUsageContainer = Container::Vertical({
        filesystemRightPannel
    });

    
}


void Application::saveIntoJson()
{
    Dictionary t{dictionaryNameInputVal, std::stoi(dictionaryIdInputVal), dictionaryContentInputVal};
    if(isPathDictionary)
    {
        cv::TextWorker::writeIntoFile(openedFilePath, cv::JsonWorker::serializeDictionary(t));
    }
    else
    {
        if(!cv::FileManipulation::createNewFile(openedFilePath)) return;
        cv::TextWorker::writeIntoFile(openedFilePath, cv::JsonWorker::serializeDictionary(t));
        
    }
    moveFileMenuToPath(usingPath);
}

void Application::saveIntoXml()
{
    Dictionary t{dictionaryNameInputVal, std::stoi(dictionaryIdInputVal), dictionaryContentInputVal};
    if(isPathDictionary)
    {
        cv::TextWorker::writeIntoFile(openedFilePath, cv::XmlWorker::serializeDictionary(t));
    }
    else
    {
        if(!cv::FileManipulation::createNewFile(openedFilePath)) return;
        cv::TextWorker::writeIntoFile(openedFilePath, cv::XmlWorker::serializeDictionary(t));
    }
    moveFileMenuToPath(usingPath);
    
}


void Application::createDictionaryContainer(std::string path)
{
    dictionaryCommandExecutionString = "Open some XML or JSON file...";
    dictionaryDownPannel = Container::Horizontal({
        Container::Horizontal({
            Renderer([&]{
                return text("Filename:")|vcenter;
            }),
            Input(&dictionaryPathInputVal)|borderRounded
        })|flex,
        Container::Vertical({
                Button("Enter", [&]{
                    if(dictionaryPathInputVal==""){
                        dictionaryCommandExecutionString="Enter some path into input string...";
                        return;
                    }
                    std::string selectedPath = usingPath + "/" + dictionaryPathInputVal;
                    openPath(selectedPath);

                    dictionaryPathInputVal = "";
                }),
                Button("<-", [&]{
                    if(usingPath!=startCurrentPath)
                        openParentPath(std::filesystem::path(usingPath).parent_path().string());
                    else{
                        dictionaryCommandExecutionString="Can't open higher hierarchy folder...";
                        commandExecutionString="Can't open higher hierarchy folder...";
                    }
                })
            }),
        Container::Vertical({
            Button("Save in JSON", [&]{
                if(dictionaryPathInputVal=="")
                {
                    dictionaryCommandExecutionString="Enter some path into input string...";
                    return;
                }
                else if(dictionaryCommandExecutionString.length() >= 5 && dictionaryCommandExecutionString.substr(dictionaryCommandExecutionString.length() - 5) == ".json ")
                {
                    openedFilePath = usingPath + "/" + dictionaryPathInputVal;
                    modalSureToSerializeIntoJson=true;
                    
                }
                else{
                    dictionaryCommandExecutionString="Unappropriate file extension...";
                    openedFilePath = usingPath + "/" + dictionaryPathInputVal;
                    modalSureToSerializeIntoJson=true;
                }

            }),
            Button("Save in XML", [&]{
                if(dictionaryPathInputVal=="")
                {
                    dictionaryCommandExecutionString="Enter some path into input string...";
                }
                else if(dictionaryCommandExecutionString.length() >= 4 && dictionaryCommandExecutionString.substr(dictionaryCommandExecutionString.length() - 4) == ".xml ")
                {
                    openedFilePath = usingPath + "/" + dictionaryPathInputVal;
                    modalSureToSerializeIntoXml=true;

                }
                else{
                    openedFilePath = usingPath + "/" + dictionaryPathInputVal;
                    dictionaryCommandExecutionString="Unappropriate file extension...";
                    openedFilePath = usingPath + "/" + dictionaryPathInputVal;
                    modalSureToSerializeIntoXml=true;
                }
            })
        })
    });


    dictionaryRightPannel = Container::Vertical({
        Container::Vertical({
            Renderer([&]{
                return text("Dictionary")|bold|color(Color::BlueLight)|center;
            }),
            Container::Horizontal({
                Renderer([&]{
                    return text("ID#")|bold|center;
                }),
                Input(&dictionaryIdInputVal)|border|vcenter|flex
                | CatchEvent([&](Event event) {
                    return event.is_character() && !std::isdigit(event.character()[0]);
                })
                | CatchEvent([&](Event event) {
                    return event.is_character() && dictionaryIdInputVal.size() > 8;
                })
            }),
            Container::Horizontal({
                Renderer([&]{
                    return text("Name: ")|bold|center;
                }),
                Input(&dictionaryNameInputVal)|border|vcenter|flex
            }),
            Renderer([&]{
                return vbox({
                    separator(),
                    filler()
                });
                
            }),
            Renderer([&]{
                return text("Content")|bold|color(Color::BlueLight)|center;
            }),
            Input(&dictionaryContentInputVal)|border|vcenter|flex
            
        }),
        Renderer([&]{
            return filler();
        }),
        Renderer([&]{
            return text(dictionaryCommandExecutionString) | underlined | bgcolor(Color::GrayLight);
        }),
        dictionaryDownPannel
    });

    dictionaryUsageContainer = Container::Vertical({
        dictionaryRightPannel
        
    });
}





void Application::createArchiveContainer(std::string path)
{

    dirEntries = cv::FileManipulation::getDirectoryEntries(path);
    archiveCommandExecutionString = "<Ctrl+A> to create a zip archive from selected folder...";

    archiveLeftPannel = Container::Vertical({
        Renderer([&]{
            return text("#>"+startCurrentPath)|underlined|bold|bgcolor(Color::YellowLight);
        }),
        Renderer([&]{
            return text("/>"+usingPath)|underlined|bold|bgcolor(Color::GreenLight);
        }),
        Renderer([&]{return separator();}),
    });


    archiveDownPannel=Container::Horizontal({});


    archiveRightPannel = Container::Vertical({
        Container::Vertical({
            Renderer([&]{
                return text(archiveCommandExecutionString) |underlined|bold|bgcolor(Color::GrayLight);
            })
        }),
        archiveDownPannel
    });


    archiveContainer = Container::Vertical({
        archiveRightPannel
        
    });


}


void Application::echoByAll(std::string text)
{
    dictionaryCommandExecutionString = text;
    archiveCommandExecutionString = text;
    commandExecutionString = text;
}




void Application::createFilesystemMenu()
{
    dirEntries = cv::FileManipulation::getDirectoryEntries(usingPath);

    filesystemMenu = Container::Vertical({
        Renderer([&]{
            return text("#>"+startCurrentPath)|underlined|bold|bgcolor(Color::YellowLight)|color(Color::GrayDark);
        }),

        Renderer([&]{
            return text("/>"+usingPath)|underlined|bold|bgcolor(Color::GreenLight)|color(Color::GrayDark);
        }),

        Renderer([&]{return separator();}),

        Menu({&dirEntries, &filesystemMenuSelected})
            | CatchEvent([&](Event event){
                  bool ret = (Event::Character("\n")==event | Event::ArrowRight==event);
                  if(ret)
                  {
                      std::string selectedPath = usingPath+'/'+dirEntries[filesystemMenuSelected];
                      openPath(selectedPath);

                  }
                  return ret;
              })
            | CatchEvent([&](Event event){
                  bool ret = (Event::ArrowLeft==event);
                  if(ret)
                  {
                      if(usingPath!=startCurrentPath)
                          openParentPath(std::filesystem::path(usingPath).parent_path().string());
                      else{
                          echoByAll("Can't open higher hierarchy folder...");
                      }
                  }
                  return ret;
              })
            | CatchEvent([&](Event event){
                  bool ret = (Event::Delete == event);

                  if(ret)
                  {
                      openedFilePath = usingPath+'/'+dirEntries[filesystemMenuSelected];
                      modalSureToDelete=true;

                  }
                  return ret;
              })
            | CatchEvent([&](Event event){
                  bool ret = (Event::CtrlA==event);
                  if(ret)
                  {
                      if(cv::FileManipulation::isDirectory(usingPath+"/"+dirEntries[filesystemMenuSelected]))
                      {
                          archiveCreatingPath = usingPath+"/"+dirEntries[filesystemMenuSelected];
                          modalSureToSaveArchive=true;
                      }
                      else
                      {
                          echoByAll("<Ctrl+A> pressed. Select a folder before pressing <Ctrl+A>");
                      }
                  }
                  return ret;
              })
    }) | border;


}




Application::Application(std::function<void()>exitor)
{
    using namespace ftxui;

    exitorClosure=exitor;

    tabToggle = Toggle(&tabValues, &tabSelected) | borderRounded;

    createFilesystemMenu();


    createDiskUsageTable();
    std::cout<< "diskusagetable_create\n";

    createFilesystemContainer(std::filesystem::current_path().string());
    std::cout<< "filesystemcontainer_create\n";

    createDictionaryContainer(std::filesystem::current_path().string());
    std::cout<<"dictionarycontainer_create\n";

    createArchiveContainer(std::filesystem::current_path().string());
    std::cout<<"arhicvecontainer_create\n";




    tabContainer = Container::Tab({
        diskUsageContainer,
        filesystemUsageContainer,
        dictionaryUsageContainer,
        archiveContainer
    }, &tabSelected);




    mainContainer = Container::Horizontal({
        filesystemMenu,
        Container::Vertical({
            Container::Horizontal({
                tabToggle|flex,
                Button("X", [&]{
                    exitorClosure();
                    }) | bold
                }),
            tabContainer
        })
    })
    | Modal(Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Undeniable action")|hcenter|bold,
                text("You sure you want\nto delete this path?"),
                text(openedFilePath)|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Anyway",[&]{
                cv::FileManipulation::deletePath(openedFilePath);
                commandExecutionString="Deleted path: "+ openedFilePath;
                moveFileMenuToPath(usingPath);
                modalSureToDelete=false;
                
                
            }),
            Button("Cancel",[&]{
                modalSureToDelete=false;
                
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
                text("Sure to serialize into JSON?")|hcenter|bold,
                text("The file  will be used."),
                text(cv::FileManipulation::clearNonRelativePath(openedFilePath))|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Accept",[&]{
                modalSureToSerializeIntoJson=false;
                saveIntoJson();
                
            }),
            Button("Cancel",[&]{
                modalSureToSerializeIntoJson=false;
            })
        })
    }), &modalSureToSerializeIntoJson)
    |Modal(Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Sure to serialize into XML?")|hcenter|bold,
                text("The file  will be used."),
                text(cv::FileManipulation::clearNonRelativePath(openedFilePath))|color(Color::Blue)
            });
        }),
        Container::Horizontal({
            Button("Accept",[&]{
                modalSureToSerializeIntoXml=false;
                saveIntoXml();
                
            }),
            Button("Cancel",[&]{
                modalSureToSerializeIntoXml=false;
            })
        })
    }), &modalSureToSerializeIntoXml)
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
    }), &modalSureToUnzip)
    | Modal(Container::Vertical({
        Renderer([&]{
            openedFilePath = archiveCreatingPath;
            
            return vbox({
                text("Undeniable action")|hcenter|bold,
                text("You sure you want\nto create archive from this folder?"),
                text(openedFilePath)|color(Color::Blue),
            });
        }),
        Container::Horizontal({
            Button("Anyway",[&]{
                zipArchive();
                archiveCommandExecutionString = "Creating archive from folder" + openedFilePath;
                modalSureToSaveArchive=false;
            }),
            Button("Cancel",[&]{
                modalSureToSaveArchive=false;
            })
        })
    }), &modalSureToSaveArchive);
    
}





ftxui::Component Application::render()
{
    using namespace ftxui;
    return mainContainer;
    
}

