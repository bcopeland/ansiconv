cmake_minimum_required(VERSION 2.8)

project(ansiconv)

set(PROJECT_SOURCES
  src/outputlo.c
  src/output.c
  src/ansiconv.h
  src/sauce.c
  src/main.c
  src/ansiparse.c
  src/convert.c
  src/outputpng.h
  src/list.h
  src/outputpng.c
  src/readbin.c
  src/dosfont.h
  src/dosfont.c
  src/sauce.h
  src/list.c
  src/output.h
  src/readidf.c
)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

add_executable(ansiconv ${PROJECT_SOURCES})
set_target_properties(ansiconv PROPERTIES
  C_STANDARD 99
)
target_link_libraries(ansiconv ${PNG_LIBRARY})
