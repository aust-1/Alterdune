cmake_minimum_required(VERSION 3.16)

project(alterdune LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/utf-8)
endif()

add_executable(alterdune
  src/main.cpp
  src/acts/ActAction.cpp
  src/acts/ActCatalog.cpp
  src/bestiary/Bestiary.cpp
  src/bestiary/BestiaryEntry.cpp
  src/entities/BossMonster.cpp
  src/entities/Entity.cpp
  src/entities/MiniBossMonster.cpp
  src/entities/Monster.cpp
  src/entities/NormalMonster.cpp
  src/entities/Player.cpp
  src/io/CsvLoader.cpp
  src/items/Inventory.cpp
  src/items/Item.cpp
  src/systems/Combat.cpp
  src/systems/Game.cpp
  src/ui/TerminalUI.cpp
  src/util/RandomService.cpp
)

target_include_directories(alterdune PRIVATE include)
