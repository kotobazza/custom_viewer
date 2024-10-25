#include <filesystem>
#include <ftxui/screen/terminal.hpp>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for string, basic_string
#include <vector> // for vector

#include "ftxui/component/captured_mouse.hpp" // for ftxui
#include "ftxui/component/component.hpp" // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp" // for Element, separator, operator|, vbox, border

#include <ftxui/component/component_options.hpp>

#include <ftxui/dom/table.hpp>
#include <ftxui/screen/terminal.hpp>



class Application{
private:
    std::vector<std::string> tabValues{"Disk Usage", "Filesystem", "Text Works", "Dictionary"};
    
    //* Modifiers and Selectors
    int tabSelected{0};
    int leftPannelSizeModifier{30};
    int mainMenuSelected{0};
    int filesystemMenuSelected{0};

    //* InputStrings
    std::string mainPathInputVal{};

    //* DirMenuEntries
    std::vector<std::string> dirEntries{};

    //* DiskUsageTable
    

    //* MainComponents
    ftxui::Component tabToggle;
    ftxui::Component tabContainer;
    ftxui::Component mainContainer;

    //*Tabs
    
    ftxui::Component filesystemMenu;
    
    

    
    


    //* DiskUsagePannels
    ftxui::Component diskUsageContainer;
    ftxui::Element mainTable;



    //* FilesystemPannels
    ftxui::Component filesystemUsageContainer;
    ftxui::Component filesystemLeftPannel;
    ftxui::Component filesystemRightPannel;
    ftxui::Component filesystemDownPannel;


    //* TextWorksPannels
    ftxui::Component textUsageContainer;
    ftxui::Component textworksLeftPannel;
    ftxui::Component textworksRightPannel;
    ftxui::Component textworksDownPannel;


    //* DictionaryPannels
    ftxui::Component dictionaryUsageContainer;
    ftxui::Component dictionaryLeftPannel;
    ftxui::Component dictionaryRightPannel;
    ftxui::Component dictionaryDownPannel;


    //* File_ Containers Down Pannel
    

    //* Down Pannel Objects
    ftxui::Component mainPathInput;
    ftxui::Component pathInputEnter;
    ftxui::Component pathInputCreate;
    ftxui::Component pathInputDelete;
    




    void createDiskUsageTable();
    void createDownPannel();

    void createFilesystemContainer(std::string path);
    void createTextWorksContainer(std::string path);
    void createDictionaryContainer(std::string path);



public:
    Application();


    ftxui::Component render();
};