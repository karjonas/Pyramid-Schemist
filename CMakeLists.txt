cmake_minimum_required(VERSION 2.8)

project(ld36)

set(SOURCES_CPP
  src/main.cpp
)

add_executable(ld36 ${SOURCES_CPP})

set(LIBS
  allegro
  allegro_primitives
  allegro_font
  allegro_ttf
  allegro_audio
  allegro_acodec
)

target_link_libraries(ld36 ${LIBS})

install(TARGETS ld36 DESTINATION bin)