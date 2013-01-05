message(STATUS "now process <log> module ...")
add_library(log moduleLog.c)
include_directories(${PROJECT_SOURCE_DIR}/moduleTools)
