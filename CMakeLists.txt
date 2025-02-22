#если есть ошибки с закрытием потока ввода - 
#   git config --global http.postBuffer 157286400


cmake_minimum_required (VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(custom-viewer LANGUAGES CXX VERSION 1.0.0)
add_executable(custom-viewer src/main.cpp)
set_target_properties( custom-viewer
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Получаем список всех заголовочных файлов
# file(GLOB_RECURSE headers "src/Headers/*.h")

# Получаем список всех исходных файлов
# file(GLOB_RECURSE sources "src/Sources/*.cpp")


file(GLOB headers "src/Headers/*.h")
foreach(file ${headers})
  target_sources(custom-viewer PUBLIC ${file})
endforeach()

file(GLOB sources "src/Sources/*.cpp")
foreach(file ${sources})
  target_sources(custom-viewer PRIVATE ${file})
endforeach()

target_include_directories(custom-viewer
PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src/Headers
PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src/Sources
)



include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
  GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)
 

#cmake_minimum_required (VERSION 3.30)

#project(libarchiveCustomTest LANGUAGES CXX VERSION 1.0.0)

#set(ENABLE_TEST OFF)

#include(FetchContent)

FetchContent_Declare(
    libarchive
    GIT_REPOSITORY https://github.com/libarchive/libarchive.git
    GIT_TAG master
)

FetchContent_MakeAvailable(libarchive)

#add_executable(libarchiveCustomTest src/main.cpp)

#target_link_libraries(libarchiveCustomTest PUBLIC archive)




# cmake_minimum_required(VERSION 3.30)
# project(p)

# include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  EXCLUDE_FROM_ALL
)
set(BOOST_INCLUDE_LIBRARIES filesystem program_options property_tree math)

FetchContent_MakeAvailable(Boost)

# add_executable(boost_test serialize.cpp)

# Убедитесь, что вы добавили все необходимые библиотеки
# target_link_libraries(boost_test PRIVATE Boost::filesystem
#                                          Boost::program_options
#                                          Boost::property_tree
#                                          Boost::math)





# # Создаем библиотеку из исходных файлов
# add_library(custom-viewer-lib ${sources})

# # Добавляем каталоги с заголовочными и исходными файлами
# target_include_directories(custom-viewer-lib
#   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Headers
#   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Sources
# )


target_link_libraries( custom-viewer
  # PUBLIC custom-viewer-lib
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component 
  PRIVATE archive
  PRIVATE Boost::filesystem
  PRIVATE Boost::program_options
  PRIVATE Boost::property_tree
  PRIVATE Boost::math
)


