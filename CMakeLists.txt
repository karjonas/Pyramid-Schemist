cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Pyramid_Schemist)

set(DATA_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "Directory for game data")
add_definitions(-DDATA_DIR="${DATA_DIR}")

set(SOURCES_CPP
  src/main.cpp
  src/pyramid.cpp
  src/scene.cpp
  src/level.cpp
  src/paths.cpp
)

add_executable(Pyramid_Schemist ${SOURCES_CPP})

set(LIBS
  allegro
  allegro_primitives
  allegro_font
  allegro_ttf
  allegro_audio
  allegro_acodec
  allegro_image
)

target_link_libraries(Pyramid_Schemist ${LIBS})

install(TARGETS Pyramid_Schemist DESTINATION bin)
install(DIRECTORY audio DESTINATION share/Pyramid_Schemist)
install(DIRECTORY fonts DESTINATION share/Pyramid_Schemist)
install(DIRECTORY img DESTINATION share/Pyramid_Schemist)
