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
    std::vector<std::string> tabValues{"Disk Usage", "Filesystem", "Dictionary", "Archive"};
    std::string starterPath{std::filesystem::current_path().string()};
    std::string usingPath{std::filesystem::current_path().string()};
    std::string openedFilePath{};

    std::vector<std::string> tempMenuVals{".."};
    int a=0;





    bool modalSureToDelete{false};
    bool modalSureToWrite{false};
    bool modalSureToUnzip{false};
    bool modalWrongPath{false};
    bool modalFileCanNotBeOverwritten{false};
    //* Modifiers and Selectors

    int tabSelected{0};
    int leftPannelSizeModifier{60};
    int mainMenuSelected{0};
    int filesystemMenuSelected{0};

    std::string commandExecutionString{};
    

    bool isPathFile{false};
    bool isPathArchive{false};
    bool isPathDictionary{false};
    
    std::string plainTextPlaceholder{"Open some text file..."};
    std::string archivePlainText;

    
    


    //* InputStringsmainPathInputVal
    std::string pathInputVal{};

    //* DirMenuEntries
    std::vector<std::string> dirEntries{};

    //* DiskUsageTable
    

    //* MainComponents
    ftxui::Component tabToggle;
    ftxui::Component tabContainer;
    ftxui::Component filesystemMenu;
    ftxui::Component mainContainer;


    //*Tabs
    

    
    



    //* DiskUsagePannels
    ftxui::Component diskUsageContainer;
    ftxui::Element mainTable;



    //* FilesystemPannels
    ftxui::Component filesystemUsageContainer;
    ftxui::Component filesystemLeftPannel;
    ftxui::Component filesystemRightPannel;
    ftxui::Component filesystemDownPannel;
    std::string plainTextString{};
    










    //* DictionaryPannels
    ftxui::Component dictionaryUsageContainer;
    ftxui::Component dictionaryLeftPannel;
    ftxui::Component dictionaryRightPannel;
    ftxui::Component dictionaryDownPannel;

    ftxui::Component dictionaryIdInput;
    ftxui::Component dictionaryNameInput;
    ftxui::Component dictionaryContentInput;



    std::string dictionaryNameInputVal{};
    std::string dictionaryIdInputVal{""};
    std::string dictionaryContentInputVal{};
    bool modalSureToSerializeIntoJson{false};
    bool modalSureToSerializeIntoXml{false};










    ftxui::Component exitButton{};


    //* ArchiveCreationPannels
    ftxui::Component archiveContainer;
    ftxui::Component archiveLeftPannel;
    ftxui::Component archiveRightPannel;
    ftxui::Component archiveDownPannel;
    bool modalSureToSaveArchive{false};
    std::string archiveCreatingPath{};



    //* File_ Containers Down Pannel
    

    //* Down Pannel Objects
    ftxui::Component mainPathInput;





    void createDiskUsageTable();

    void createFilesystemContainer();
    void createDictionaryContainer();
    void createArchiveContainer();

    void openPath(std::string path);
    void openParentPath(std::string path);
    void moveFileMenuToPath(std::string path);
    void clearPlainText();
    void saveFileText();
    void unzipArchive();
    void zipArchive();
    void saveIntoJson();
    void saveIntoXml();

    void openDirectory(std::string path);
    void showArchive(std::string path);
    void showFile(std::string text, std::string path);
    void showStructuredFile(std::string structed_text, std::string path);



    void createFilesystemMenu();
    void echoCommand(std::string text);
    

    std::function<void()> exitorClosure;

public:
    Application(std::function<void()>exitor);


    ftxui::Component render();
};
