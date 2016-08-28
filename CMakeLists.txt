cmake_minimum_required(VERSION 2.8)

project(Pyramid_Schemist)

set(SOURCES_CPP
  src/main.cpp
  src/pyramid.cpp
  src/scene.cpp
  src/level.cpp
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
