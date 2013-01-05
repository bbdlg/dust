message(STATUS "now complie <log> module")
add_library(log logModule.c)
include_directories(${PROJECT_SOURCE_DIR}/moduleTools)
