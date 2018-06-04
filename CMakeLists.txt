################################################################
# cmake file for building DDAMA (Damic DAta Monitoring Analysis)
# @author Nuria Castello-Mor
CMAKE_MINIMUM_REQUIRED(VERSION 3.5 FATAL_ERROR)
################################################################

# project name
PROJECT(DDAMA)

### PACKAGE VERSIONS ########################################################
SET(${PROJECT_NAME}_VERSION_MAJOR 0 )
SET(${PROJECT_NAME}_VERSION_MINOR 1 )
SET(${PROJECT_NAME}_VERSION_PATCH 0 )

SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

###############################
# Require C++11
INCLUDE(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
    MESSAGE( SEND_ERROR "DDAMA requires C++11 support. Please upgrade your compiler !" )
ENDIF()

# Where to look of cmake config files
LIST(APPEND CMAKE_MODULE_PATH /damic/dqm4hep/cmake )

# DEPENDENCIES
# --- XXX [1] find a way (USE_FILE??) to include automaticaly dirs and libre
FIND_PACKAGE(DQMCore REQUIRED)
#FIND_PACKAGE(DQMViz REQUIRED)
FIND_PACKAGE(ROOT REQUIRED)

# Includes (XXX [1])
INCLUDE_DIRECTORIES("${DQMCore_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${ROOT_INCLUDE_DIRS}")
# Project
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

# Libraries dir XXX [1]

# DDAMA Plugin 
ADD_LIBRARY(ddama SHARED src/ddamaSModule.cc)

TARGET_LINK_LIBRARIES(ddama ${DQMCore_LIBRARIES} ${ROOT_LIBRARIES})

INSTALL(
    TARGETS ddama
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    COMPONENT library)
