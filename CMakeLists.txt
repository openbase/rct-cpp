cmake_minimum_required(VERSION 3.5)

include(FindPkgConfig)
include(UsePkgConfig)

# --- general project setup ---
set(PROJECT_NAME                  openbase-rct)
set(OPENBASE_RCT_VERSION_MAJOR    "2")
set(OPENBASE_RCT_VERSION_MINOR    "0")
set(OPENBASE_RCT_VERSION_REVISION "0")
set(OPENBASE_RCT_VERSION_SHORT    "${OPENBASE_RCT_VERSION_MAJOR}.${OPENBASE_RCT_VERSION_MINOR}")
#set(PROJECT_NAMESPACE 	          "org-openbase")

project(${PROJECT_NAME} VERSION ${OPENBASE_RCT_VERSION} LANGUAGES CXX)

set(CONFIG_PACKAGE_LOCATION "lib/cmake/${PROJECT_NAME}")
set(CMAKE_CONFIG_FILE_NAME "RCTConfig.cmake")
set(CMAKE_CONFIG_VERSION_FILE_NAME "RCTConfigVersion.cmake")
set(CMAKE_CONFIG_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_FILE_NAME}")
set(CMAKE_CONFIG_VERSION_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_VERSION_FILE_NAME}")

set(OPENBASE_RCT_NAME_RSB         rctrsb)
set(OPENBASE_RCT_NAME_ROS         rctros)
set(OPENBASE_RCT_NAME_PROTO       rctproto)
set(OPENBASE_RCT_API_VERSION      "${OPENBASE_RCT_VERSION_MAJOR}.${OPENBASE_RCT_VERSION_MINOR}")
set(OPENBASE_RCT_VERSION          "${OPENBASE_RCT_VERSION_MAJOR}.${OPENBASE_RCT_VERSION_MINOR}.${OPENBASE_RCT_VERSION_REVISION}")
set(OPENBASE_RCT_PACKAGE          "${PROJECT_NAME}-${OPENBASE_RCT_VERSION_MAJOR}.${OPENBASE_RCT_VERSION_MINOR}")
set(OPENBASE_RCT-ROS_PACKAGE      "${PROJECT_NAME}-ros-${OPENBASE_RCT_VERSION_MAJOR}.${OPENBASE_RCT_VERSION_MINOR}")

math(EXPR OPENBASE_RCT_VERSION_MAJOR_CONT     "${OPENBASE_RCT_VERSION_MAJOR}*1000000")
math(EXPR OPENBASE_RCT_VERSION_MINOR_CONT     "${OPENBASE_RCT_VERSION_MINOR}*1000")
math(EXPR OPENBASE_RCT_VERSION_REVISION_CONT  "${OPENBASE_RCT_VERSION_REVISION}*1")
math(EXPR OPENBASE_RCT_VERSION_CONT           "${OPENBASE_RCT_VERSION_MAJOR_CONT}+${OPENBASE_RCT_VERSION_MINOR_CONT}+${OPENBASE_RCT_VERSION_REVISION_CONT}")

set(PREFIX     ${CMAKE_INSTALL_PREFIX})
set(BINDIR     "${PREFIX}/bin")
set(INCLUDEDIR "${PREFIX}/include/${OPENBASE_RCT_PACKAGE}")
set(MANDIR     "${PREFIX}/man")
set(LIBDIR     "${PREFIX}/lib/")
set(DATADIR    "${PREFIX}/share/${OPENBASE_RCT_PACKAGE}")
set(DATADIR_ROS    "${PREFIX}/share/${OPENBASE_RCT-ROS_PACKAGE}")

option(BUILD_EXAMPLES "build examples?" ON)
option(BUILD_ROS_SUPPORT "build ros middleware support?" ON)
option(BUILD_RSB_SUPPORT "build rsb middleware support?" ON)

if(WIN32)
    set(OPENBASE_RCT_BUILD_TYPE      STATIC)
	set(OPENBASE_RCT_BUILD_PREFIX    ${CMAKE_STATIC_LIBRARY_PREFIX})
	set(OPENBASE_RCT_BUILD_SUFFIX    ${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
    set(OPENBASE_RCT_BUILD_TYPE      SHARED)
	set(OPENBASE_RCT_BUILD_PREFIX    ${CMAKE_SHARED_LIBRARY_PREFIX})
	set(OPENBASE_RCT_BUILD_SUFFIX    ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

SET(CMAKE_INSTALL_RPATH "${LIBDIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# --- dependencies ---
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
# must be done before rsc, because rsc module for finding eigen is broken!
find_package(Eigen3 REQUIRED)
include_directories(BEFORE SYSTEM  ${Eigen3_INCLUDE_DIRS})
link_directories(${Eigen3_LIBRARY_DIRS})

#rsc
find_package(RSC 0.18 REQUIRED)
message(STATUS "RSC version: ${RSC_VERSION}")
include_directories(BEFORE SYSTEM ${RSC_INCLUDE_DIRS})
link_directories(${RSC_LIBRARY_DIRS})

find_package(RSC-CMake 0.18 REQUIRED)
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_INSTALL_PREFIX}/share/cmake/Modules" ${RSC_CMAKE_MODULE_PATH})

#boost
find_package(Boost REQUIRED QUIET COMPONENTS thread program_options date_time)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_LOG_DYN_LINK)

message(STATUS "Using CMake module path: ${CMAKE_MODULE_PATH}")

find_package(tf2-minimal REQUIRED)
include_directories(BEFORE SYSTEM  ${tf2-minimal_INCLUDE_DIRS})
link_directories(${tf2-minimal_LIBRARY_DIRS})
message(STATUS "tf2 version: ${tf2-minimal_VERSION}")
set(RCT_HAVE_TF2 TRUE)

find_package(openbase-type REQUIRED)

if(BUILD_ROS_SUPPORT)
	find_package(roscpp)
	include_directories(BEFORE SYSTEM  ${roscpp_INCLUDE_DIRS})
	link_directories(${roscpp_LIBRARY_DIRS})
	if(roscpp_FOUND)
		message(STATUS "roscpp version: ${roscpp_VERSION}")
		set(RCT_HAVE_ROS TRUE)
	else(roscpp_FOUND)
		message(FATAL_ERROR "\nroscpp not found\nIn order skip building ROS support add option -DBUILD_ROS_SUPPORT=OFF")
	endif(roscpp_FOUND)
endif(BUILD_ROS_SUPPORT)

if(BUILD_RSB_SUPPORT)
	find_package(ProtocolBuffers)
	if(NOT PROTOBUF_LIBRARY)
	    message(FATAL_ERROR "\nprotobuf library not found\nIn order skip building RSB support add option -DBUILD_RSB_SUPPORT=OFF")
	endif()
	if(NOT PROTOBUF_INCLUDE_DIR)
	    message(FATAL_ERROR "\nprotobuf include dir not found\nIn order skip building RSB support add option -DBUILD_RSB_SUPPORT=OFF")
	endif()
	if(NOT PROTOBUF_PROTOC_EXECUTABLE)
	    message(FATAL_ERROR "\nprotoc executable not found\nIn order skip building RSB support add option -DBUILD_RSB_SUPPORT=OFF")
	endif()
	
	#rsb
	find_package(RSB 0.18 QUIET)

	if(RSB_FOUND)
		message(STATUS "RSB version: ${RSB_VERSION}")
		set(RCT_HAVE_RSB TRUE)
		include_directories(BEFORE SYSTEM  ${RSB_INCLUDE_DIRS})
		link_directories(${RSB_LIBRARY_DIRS})
	else(RSB_FOUND)
		message(FATAL_ERROR "RSB not found!")
		message(STATUS "In order skip building RSB support add option -DBUILD_RSB_SUPPORT=OFF")
	endif(RSB_FOUND)
endif(BUILD_RSB_SUPPORT)

configure_file(${CMAKE_SOURCE_DIR}/core/src/rct/rctConfig.h.in ${CMAKE_BINARY_DIR}/rct/rctConfig.h)
install(FILES ${CMAKE_BINARY_DIR}/rct/rctConfig.h DESTINATION ${INCLUDEDIR}/rct)

include_directories(${CMAKE_BINARY_DIR})

# --- build instructions modules ---
add_subdirectory(core/src)
if(BUILD_RSB_SUPPORT)
	add_subdirectory(rsb/src)
endif(BUILD_RSB_SUPPORT)
if(BUILD_ROS_SUPPORT)
	add_subdirectory(ros/src)
endif(BUILD_ROS_SUPPORT)
if(BUILD_EXAMPLES)
	ADD_SUBDIRECTORY(examples/src)
endif(BUILD_EXAMPLES)

configure_file(RCTConfig.cmake.in "${PROJECT_BINARY_DIR}/RCTConfig.cmake" @ONLY)
configure_file(RCTConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/RCTConfigVersion.cmake" @ONLY)
configure_file(RCT-ROSConfig.cmake.in "${PROJECT_BINARY_DIR}/RCT-ROSConfig.cmake" @ONLY)
configure_file(RCT-ROSConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/RCT-ROSConfigVersion.cmake" @ONLY)

install(FILES
		${CMAKE_CONFIG_FILE_PATH}
		${CMAKE_CONFIG_VERSION_FILE_PATH}
		DESTINATION "${DATADIR}")
install(FILES
		${CMAKE_CONFIG_FILE_PATH}
		${CMAKE_CONFIG_VERSION_FILE_PATH}
		DESTINATION "${DATADIR_ROS}")
install(DIRECTORY
		"${PROJECT_SOURCE_DIR}/cmake"
		DESTINATION "${DATADIR}")
