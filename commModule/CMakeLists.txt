message(STATUS "now complie <comm> module")
add_library(comm commModule.c)
include_directories(${PROJECT_SOURCE_DIR}/toolsModule)
