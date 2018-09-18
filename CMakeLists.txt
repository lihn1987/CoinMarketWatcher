cmake_minimum_required(VERSION 3.5.1)
project(CoinMarketWatcher)
add_subdirectory(./submodule/AsioHttps)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CUR_DIR .)
aux_source_directory(${CUR_DIR}/src DIR_SRC)

SET(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
INCLUDE_DIRECTORIES(${CUR_DIR}/submodule/AsioHttps/src)

#对qt的处理
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED Widgets)
qt5_wrap_cpp( MOC ${CUR_DIR}/src/main_widget.h)
qt5_wrap_ui( UIC ${CUR_DIR}/src/main_widget.ui)
QT5_ADD_RESOURCES(RCC ${CUR_DIR}/src/resouce.qrc)

set(QT_SRC
  ${CUR_DIR}/src/main_widget.cc
  ${CUR_DIR}/src/main_widget.h
  ${CUR_DIR}/src/main_widget.ui)

add_executable(CoinMarketWatcher
  ${DIR_SRC}
  ${QT_SRC}
  ${MOC}
  ${RCC}
)

target_link_libraries(CoinMarketWatcher AsioHttpsLib)
target_link_libraries(CoinMarketWatcher Qt5::Widgets)
target_link_libraries(CoinMarketWatcher boost_thread)
target_link_libraries(CoinMarketWatcher boost_iostreams)
