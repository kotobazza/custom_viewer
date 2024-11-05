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
#include <ftxui/dom/flexbox_config.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/screen/color.hpp"
#include <ftxui/screen/terminal.hpp>
#include <iterator>
#include <string>

#include <cstdint>
#include <sstream>

using namespace ftxui;

namespace Colors
{
ftxui::Color BACKGROUND_COLOR = ftxui::Color::RGB(40, 40, 40);
ftxui::Color FOREGROUND_COLOR = ftxui::Color::RGB(240, 240, 240);
ftxui::Color ALT_BACKGROUND_COLOR = ftxui::Color::RGB(240, 240, 240);
ftxui::Color ALT_FOREGROUND_COLOR = ftxui::Color::RGB(60, 60, 60);

ftxui::Color THEME_COLOR_1 = ftxui::Color::GreenLight;
ftxui::Color THEME_COLOR_2 = ftxui::Color::BlueLight;
ftxui::Color THEME_COLOR_3 = ftxui::Color::Cyan;
ftxui::Color THEME_COLOR_4 = ftxui::Color::Orange1;

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
    content.DecorateCellsAlternateRow(color(Colors::THEME_COLOR_2), 3, 0);
    content.DecorateCellsAlternateRow(color(Colors::THEME_COLOR_3), 3, 1);
    content.DecorateCellsAlternateRow(color(Colors::FOREGROUND_COLOR), 3, 2);

    mainTable = table.Render();

    diskUsageContainer = Renderer(diskUsageContainer, [&]{
        return vbox({
            mainTable | flex, //EXPANDING
            text(commandExecutionString) |underlined|bold|bgcolor(Colors::ALT_BACKGROUND_COLOR)|bold|color(Colors::ALT_FOREGROUND_COLOR)
        });
    });

}

void Application::openDirectory(std::string path)
{
    moveFileMenuToPath(path);
    clearPlainText();
    echoCommand("Opened directory: "+std::filesystem::path(path).filename().string());

}

void Application::showArchive(std::string path)
{
    cv::ArchiveReadingResults output = cv::ArchiveWorker::readArchiveEntries(path);
    if(output.state != cv::ActionState::Done)
    {
        echoCommand("ERROR: can't read provided archive: "+path);
        return;
    }

    std::string content = "Reading archive includes...\nArchive [" + path + "]:\n";

    for(auto& ent: output.entries)
    {
        content += ent + "\n";
    }
    
    echoCommand("Opened archive: "+std::filesystem::path(path).filename().string());
    
    

    if(output.entries.size()==0)
    {
        echoCommand("Opened empty archive: "+std::filesystem::path(path).filename().string());
    }

    archivePlainText = content;
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
            echoCommand("ERROR: can't read provided file: "+path);
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
            
            echoCommand("Checking is " + std::string(isJson?"JSON":"XML") + " file accessible for Dictionary format. Deserialization " + std::string(serializable.state== cv::ActionState::Done ? "Done" : "Undone"));

            if(serializable.state == cv::ActionState::Done)
            {
                
                Dictionary dict = serializable.dictionary;

                echoCommand(std::string("Found Dictionary. Opened ")+(isJson ? "JSON":"XML") + " in Dictionary tab: " + std::filesystem::path(path).filename().string());

                dictionaryIdInputVal = std::to_string(dict.getDictionaryId());
                dictionaryNameInputVal = dict.getDictionaryName();
                dictionaryContentInputVal = dict.getDictionaryContent();
                isPathDictionary=true;
                openedFilePath = path;
                return;
                
            }

            
        }

        echoCommand("Opened file:"+std::filesystem::path(path).filename().string());
        

        plainTextString = output.content;
        if(output.content=="")
            echoCommand("Opened empty file:"+std::filesystem::path(path).filename().string());
        isPathFile=true;
        openedFilePath = path;

    }
    else
    {
        echoCommand("ERROR: wrong path/type of {Directiry, Archive, Text file}: "+path);

    }

}


void Application::saveFileText()
{
    if(cv::FileManipulation::isExistingPath(openedFilePath) && cv::FileManipulation::isFile(openedFilePath))
    {
        cv::TextWorker::writeIntoFile(openedFilePath, plainTextString);
        echoCommand("File saved into: "+openedFilePath);
    }
    else
    {
        echoCommand("File wasn't saved properly...");

        modalFileCanNotBeOverwritten=true;
    }
}


void Application::moveFileMenuToPath(std::string path)
{
    dirEntries.clear();
    dirEntries = std::vector<std::string>{".."};
    auto dirContainig = cv::FileManipulation::getDirectoryEntries(path);
    std::copy(dirContainig.begin(), dirContainig.end(), std::back_inserter(dirEntries));

    usingPath = path;
    openedFilePath = "";
    clearPlainText();
}


void Application::clearPlainText()
{
    plainTextString = "";
    archivePlainText = "";
    dictionaryContentInputVal="";
    dictionaryIdInputVal="";
    dictionaryNameInputVal="";
    isPathFile = false;
    isPathDictionary = false;
    isPathArchive = false;
}

void Application::openParentPath(std::string path)
{
    moveFileMenuToPath(path);
    clearPlainText();
    echoCommand("Opened folder: "+ std::filesystem::path(path).parent_path().string());
}

void Application::unzipArchive()
{
    cv::ActionState output = cv::ArchiveWorker::unzipArchive(openedFilePath);
    if(output == cv::ActionState::Done)
        echoCommand("Unzipped archive: "+std::filesystem::path(openedFilePath).filename().string());
    else
        echoCommand("ERROR: can't unzip provided archive: "+openedFilePath);

    moveFileMenuToPath(usingPath);
}


void Application::zipArchive()
{
    std::string newPath = openedFilePath+".zip";
    if(cv::ArchiveWorker::createNewZipArchive(newPath ,openedFilePath))
        echoCommand("Zipped file: " +std::filesystem::path(newPath).filename().string());
    else
        echoCommand("ERROR: can't zip provided folder: "+openedFilePath);

    moveFileMenuToPath(usingPath);
    
    
}


void Application::createFilesystemContainer()
{
    filesystemDownPannel = Container::Horizontal({
        Button("Enter", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some path into input string...(/)");
                return;
            }
            std::string selectedPath = usingPath + "/" + pathInputVal;
            openPath(selectedPath);

            pathInputVal = "";
        }),
        Button("<-", [&]{
            if(usingPath!=starterPath)
                openParentPath(std::filesystem::path(usingPath).parent_path().string());
            else{
                echoCommand("Can't open higher hierarchy folder!");

            }
        }),
        Button("Create Directory", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some path into input string...(/)");
                return;
            }
            if(!cv::FileManipulation::isExistingPath(usingPath + "/"+pathInputVal)){
                cv::FileManipulation::createNewDirectory(usingPath + "/"+pathInputVal);
                moveFileMenuToPath(usingPath);
                echoCommand("Created new directory: "+usingPath + "/"+pathInputVal);
            }
            else{
                echoCommand("This entry already exists!");
            }
        }),
        Button("Create File", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some path into input string...(/)");
                return;
            }
            if(!cv::FileManipulation::isExistingPath(usingPath + "/" + pathInputVal)){
                cv::FileManipulation::createNewFile(usingPath + "/" + pathInputVal);
                moveFileMenuToPath(usingPath);
                echoCommand("Created new file: "+usingPath + "/"+pathInputVal);
            }
            else{
                echoCommand("This entry already exists!");
            }
        }),
        Button("Delete", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some path into input string... (/)");
                return;
            }
            if(cv::FileManipulation::isExistingPath(usingPath + "/" + pathInputVal)){
                openedFilePath = usingPath+'/'+pathInputVal;
                modalSureToDelete=true;
                pathInputVal="";
            }
            else{
                echoCommand("Path in input (/) doesn't exist!");
            }
        }),
        Button("Create archive", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some directory name into input string...(/)");
                return;
            }
            modalSureToSaveArchive=true;

        }),
        Maybe(
            Button("Save", [&]{
                modalSureToWrite=true;
            }), 
        &isPathFile),
        Maybe(
            Button("Unzip archive", [&]{
                modalSureToUnzip=true;
            }), 
        &isPathArchive),

    });




    filesystemUsageContainer = Container::Vertical({
        Container::Vertical({
            Input(&plainTextString)|borderEmpty|flex, //EXPANDABLE
            Renderer([&]{
                return text(commandExecutionString) | underlined | bgcolor(Colors::ALT_BACKGROUND_COLOR)|bold|color(Colors::ALT_FOREGROUND_COLOR);
            }),
        })|flex,
        filesystemDownPannel
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


void Application::createDictionaryContainer()
{

    dictionaryDownPannel = Container::Horizontal({
        Button("Enter", [&]{
            if(pathInputVal==""){
                echoCommand("Enter some path into input string...(/)");
                return;
            }
            std::string selectedPath = usingPath + "/" + pathInputVal;
            openPath(selectedPath);

            pathInputVal = "";
        }),
        Button("<-", [&]{
            if(usingPath!=starterPath)
                openParentPath(std::filesystem::path(usingPath).parent_path().string());
            else{

                echoCommand("Can't open higher hierarchy folder!");
            }
        }),
        Button("Save in JSON", [&]{
            if(pathInputVal=="")
            {
                echoCommand("Enter some path into input string...(/)");
                return;
            }

            if(pathInputVal.length() >= 5 && pathInputVal.substr(pathInputVal.length() - 5) == ".json ")
            {
                openedFilePath = usingPath + "/" + pathInputVal;
                modalSureToSerializeIntoJson=true;
                
            }
            else{
                echoCommand("Unappropriate file extension!");
                openedFilePath = usingPath + "/" + pathInputVal;
                modalSureToSerializeIntoJson=true;
            }

        }),
        Button("Save in XML", [&]{
            if(pathInputVal=="")
            {
                echoCommand("Enter some path into input string...");

            }
            else if(pathInputVal.length() >= 4 && pathInputVal.substr(pathInputVal.length() - 4) == ".xml ")
            {
                openedFilePath = usingPath + "/" + pathInputVal;
                modalSureToSerializeIntoXml=true;

            }
            else{
                echoCommand("Unappropriate file extension!");
                openedFilePath = usingPath + "/" + pathInputVal;
                modalSureToSerializeIntoXml=true;
            }
        }),
    });


    dictionaryUsageContainer = Container::Vertical({
        Renderer([&]{
                return text("Dictionary")|bold|color(Colors::THEME_COLOR_2)|hcenter;
        }),
        Container::Horizontal({
            Renderer([&]{
                return text("ID# ")|bold;
            }),
            Input(&dictionaryIdInputVal)|underlined
                | CatchEvent([&](Event event) {
                        return event.is_character() && !std::isdigit(event.character()[0]);
                    })
                | CatchEvent([&](Event event) {
                        return event.is_character() && dictionaryIdInputVal.size() > 8;
                    })
        }),
        Container::Horizontal({
            Renderer([&]{
                return text("Name: ")|bold;
            }),
            Input(&dictionaryNameInputVal)|underlined
        }),

        Container::Vertical({
            Renderer([&]{
                return text("Content")|bold|color(Colors::THEME_COLOR_2)|hcenter; 
            }),
            Input(&dictionaryContentInputVal)|flex,
        })|border|flex,
        Renderer([&]{
            return text(commandExecutionString) | underlined | bgcolor(Colors::ALT_BACKGROUND_COLOR)|bold|color(Colors::ALT_FOREGROUND_COLOR);
        }),
        dictionaryDownPannel,

        
    });

    
}





void Application::createArchiveContainer()
{
    
    archiveDownPannel=Container::Horizontal({});


    archiveContainer = Container::Vertical({
        Container::Vertical({
        Input(&archivePlainText)|borderEmpty|flex,
        Renderer([&]{
                return text(commandExecutionString) |underlined|bold|bgcolor(Colors::ALT_BACKGROUND_COLOR)|bold|color(Colors::ALT_FOREGROUND_COLOR);
            })
        })|flex,
        archiveDownPannel
    });


}


void Application::echoCommand(std::string text)
{
    commandExecutionString = " "+ text;
}




void Application::createFilesystemMenu()
{
    moveFileMenuToPath(usingPath);

    auto flexOption = FlexboxConfig().Set(FlexboxConfig::AlignItems::Stretch);

    filesystemMenu = Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Filesystem menu")|bold|center,
                separator()
            });
        }),

        Menu({&dirEntries, &filesystemMenuSelected})
            | CatchEvent([&](Event event){
                  bool ret = (Event::Character("\n")==event);
                  if(ret)
                  {
                      if(dirEntries[filesystemMenuSelected] == "..")
                      {
                          if(usingPath!=starterPath)
                              openPath(cv::FileManipulation::getParentPath(usingPath));
                          else
                          {
                              echoCommand("Can't open higher hierarchy folder!");
                          }
                      }
                      else
                      {
                          std::string selectedPath = usingPath+'/'+dirEntries[filesystemMenuSelected];
                          openPath(selectedPath);
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
                          echoCommand("<Ctrl+A> pressed. Select a folder in Filesystem Menu before pressing <Ctrl+A>");
                      }
                  }
                  return ret;
              }),
    });


}




Application::Application(std::function<void()>exitor)
{
    exitorClosure=exitor;

    tabToggle = Toggle(&tabValues, &tabSelected) | borderRounded;

    createFilesystemMenu();


    createDiskUsageTable();

    createFilesystemContainer();

    createDictionaryContainer();

    createArchiveContainer();




    tabContainer = Container::Tab({
                                      diskUsageContainer|border|flex,
                                      filesystemUsageContainer|border|flex,
                                      dictionaryUsageContainer|border|flex,
                                      archiveContainer|border|flex
                                  }, &tabSelected);


    filesystemMenu |= border;
    filesystemMenu |=flex;


    exitButton = Button(" X ", [&]{exitorClosure();});
    mainPathInput = Input(&pathInputVal, "/")|border|bold;

    auto mainLayouter = Container::Horizontal({
        filesystemMenu,
        Container::Vertical({
            Container::Horizontal({
                tabToggle,
                exitButton
            }),
            tabContainer,
            mainPathInput
        })
    });

    mainContainer = Renderer(mainLayouter, [&]{
        return hbox({
            vbox({
                filesystemMenu->Render(),
                vbox({
                    text("Help")|bold|center,
                    separator(),
                    hbox({
                        text("Enter")|color(Colors::THEME_COLOR_2)|border,
                        text("Open folder or file")|vcenter
                    }),
                    hbox({
                        text("Ctrl")|color(Colors::THEME_COLOR_2)|border,
                        text("A")|color(Colors::THEME_COLOR_1)|border,
                        text("Create an archive")|vcenter
                    })
                })|border
            }),

            vbox({
                hbox({
                    vbox({
                        text("  #>"+starterPath)|bold|color(Colors::THEME_COLOR_1),
                        text("  />"+usingPath)|bold|color(Colors::THEME_COLOR_2),
                        text("  !>"+openedFilePath)|bold|color(Colors::THEME_COLOR_4),
                    }),
                    filler(),
                    tabToggle->Render(),
                    exitButton->Render(),
                }),
                tabContainer->Render(),
                mainPathInput->Render(),
            })|flex,
        });
    })

                    | bgcolor(Colors::BACKGROUND_COLOR)
                    | color(Colors::FOREGROUND_COLOR)




                    | Modal(Container::Vertical({
                                Renderer([&]{
                                    return vbox({
                                        text("Undeniable action")|hcenter|bold,
                                        text("You sure you want to delete this path?"),
                                        text(openedFilePath)|color(Colors::THEME_COLOR_2)
                                    });
                                }),
                                Container::Horizontal({
                                    Button("Anyway",[&]{
                                        cv::FileManipulation::deletePath(openedFilePath);
                                        echoCommand("Deleted path: "+ openedFilePath);

                                        moveFileMenuToPath(usingPath);
                                        modalSureToDelete=false;
                                        


                                    }),
                                    Button("Cancel",[&]{
                                        modalSureToDelete=false;

                                    })
                                })
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToDelete)
                    | Modal(Container::Vertical({
                                Renderer([&]{
                                    return vbox({
                                        text("Can't overwrite a file")|hcenter|bold,
                                        text("This file cannot be overwritten. The file's content will be dropped out."),
                                        text(openedFilePath)|color(Colors::THEME_COLOR_2)
                                    });
                                }),
                                Container::Horizontal({
                                    Button("Okay",[&]{
                                        modalFileCanNotBeOverwritten=false;
                                        clearPlainText();
                                    })
                                })
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalFileCanNotBeOverwritten) 
                    | Modal(Container::Vertical({
                                Renderer([&]{
                                    return vbox({
                                        text("Sure to serialize into JSON?")|hcenter|bold,
                                        text("The file  will be used."),
                                        text(cv::FileManipulation::clearNonRelativePath(openedFilePath))|color(Colors::THEME_COLOR_2)
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
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToSerializeIntoJson)
                    |Modal(Container::Vertical({
                                Renderer([&]{
                                    return vbox({
                                        text("Sure to serialize into XML?")|hcenter|bold,
                                        text("The file  will be used."),
                                        text(cv::FileManipulation::clearNonRelativePath(openedFilePath))|color(Colors::THEME_COLOR_2)
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
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToSerializeIntoXml)
                    | Modal(Container::Vertical({
                                Renderer([&]{
                                    return vbox({
                                        text("Undeniable action")|hcenter|bold,
                                        text("You sure you want\nto rewrite this file?"),
                                        text(openedFilePath)|color(Colors::THEME_COLOR_2)
                                    });
                                }),
                                Container::Horizontal({
                                    Button("Anyway",[&]{
                                        saveFileText();

                                        modalSureToWrite=false;
                                        pathInputVal="";
                                    }),
                                    Button("Cancel",[&]{
                                        modalSureToWrite=false;
                                        pathInputVal="";
                                    })
                                })
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToWrite)
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
                                        text(openedFilePath)|color(Colors::THEME_COLOR_2),
                                        text("It will " + will) |color(Colors::THEME_COLOR_3),
                                        text("creating this new path: "+ newPath)
                                    });
                                }),
                                Container::Horizontal({
                                    Button("Anyway",[&]{
                                        unzipArchive();
                                        echoCommand("Unzipping archive to folder: " + openedFilePath);
                                        modalSureToUnzip=false;
                                    }),
                                    Button("Cancel",[&]{
                                        modalSureToUnzip=false;

                                    })
                                })
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToUnzip)
                    | Modal(Container::Vertical({
                                Renderer([&]{
                                    openedFilePath = archiveCreatingPath;

                                    return vbox({
                                        text("Undeniable action")|hcenter|bold,
                                        text("You sure you want\nto create archive from this folder?"),
                                        text(openedFilePath)|color(Colors::THEME_COLOR_2),
                                    });
                                }),
                                Container::Horizontal({
                                    Button("Anyway",[&]{
                                        zipArchive();
                                        echoCommand("Creating archive from folder" + openedFilePath);
                                        modalSureToSaveArchive=false;
                                    }),
                                    Button("Cancel",[&]{
                                        modalSureToSaveArchive=false;
                                    })
                                })
                            })|border|bgcolor(Colors::BACKGROUND_COLOR), &modalSureToSaveArchive);
    
}





ftxui::Component Application::render()
{
    using namespace ftxui;
    return mainContainer;
    
}

