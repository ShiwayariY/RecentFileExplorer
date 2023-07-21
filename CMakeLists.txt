cmake_minimum_required(VERSION 3.17)
project(RecentFileExplorer)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0100 NEW)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(
	include
)

add_executable(RecentFileExplorer WIN32
	src/main.cc
	include/RecentFileModel.hh
	src/RecentFileModel.cc
	include/KeyPressHandler.hh
	src/KeyPressHandler.cc
	src/util.cc
	resource.rc
	resource.qrc
	BreezeStyleSheets/breeze.qrc
)

target_link_libraries(RecentFileExplorer
	Qt::Widgets
)

install(TARGETS
	RecentFileExplorer
DESTINATION "${PROJECT_SOURCE_DIR}/bin/${BUILD_SFX}")
