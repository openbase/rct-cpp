cmake_minimum_required(VERSION 2.6)

find_package(openbase-type REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# --- generate executable
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/core/src ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
ADD_LIBRARY(${OPENBASE_RCT_NAME_RSB} SHARED rct/impl/TransformCommRsb.cpp rct/impl/TransformConverter.cpp)
TARGET_LINK_LIBRARIES(${OPENBASE_RCT_NAME_RSB} ${RSB_LIBRARIES} org-openbase::openbase-type ${OPENBASE_RCT_NAME})
SET_TARGET_PROPERTIES(${OPENBASE_RCT_NAME_RSB} PROPERTIES
                                 VERSION ${OPENBASE_RCT_VERSION}
                                 SOVERSION ${OPENBASE_RCT_API_VERSION})

# --- install target
INSTALL(TARGETS ${OPENBASE_RCT_NAME_RSB}
        LIBRARY DESTINATION ${LIBDIR})
INSTALL(DIRECTORY . DESTINATION ${INCLUDEDIR}
          FILES_MATCHING
          PATTERN "./rct/impl/*.h"
)

