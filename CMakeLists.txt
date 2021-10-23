cmake_minimum_required(VERSION 3.14)
project(RecentFileExplorer)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 20)

set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")

include_directories(
	include
#	"${Dependency_ROOT}/include"
)

#find_library(Dependency_LIB lib_name "${Dependency_ROOT}/lib/${BUILD_SFX}")

add_executable(RecentFileExplorer
	src/main.cc
)

#target_link_libraries(RecentFileExplorer
#	"${Dependency_LIB}"
#)

install(TARGETS
	RecentFileExplorer
DESTINATION "${PROJECT_SOURCE_DIR}/bin/${BUILD_SFX}")
