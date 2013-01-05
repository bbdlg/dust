message(STATUS "now process <comm> module ...")
add_library(comm moduleComm.c)
include_directories(${PROJECT_SOURCE_DIR}/moduleTools)
