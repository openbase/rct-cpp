cmake_minimum_required(VERSION 2.6)
            
# --- generate executable
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/core/src ${CMAKE_SOURCE_DIR}/rsb/src ${CMAKE_SOURCE_DIR}/ros/src ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../../core/src)

ADD_EXECUTABLE(publisher-example rct/publisher.cpp)
TARGET_LINK_LIBRARIES(publisher-example ${PROJECT_NAME} ${OPENBASE_RCT_NAME_RSB} ${RSB_LIBRARIES} org-openbase::openbase-type ${Log4cxx_LIBRARIES})

ADD_EXECUTABLE(subscriber-example rct/subscriber.cpp)
TARGET_LINK_LIBRARIES(subscriber-example ${PROJECT_NAME} ${OPENBASE_RCT_NAME_RSB} ${RSB_LIBRARIES} org-openbase::openbase-type ${Log4cxx_LIBRARIES})
