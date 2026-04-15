cmake_minimum_required(VERSION 3.16)
project(PrograVillaGameV1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(game_v1_cli
    src/main_cli.cpp
)

target_include_directories(game_v1_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(SFML 2.5 COMPONENTS graphics window system QUIET)

if(SFML_FOUND)
    add_executable(game_v1_sfml
        src/main_sfml.cpp
    )
    target_include_directories(game_v1_sfml PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(game_v1_sfml PRIVATE sfml-graphics sfml-window sfml-system)
    message(STATUS "SFML encontrado: se construira game_v1_sfml")
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SFML3 QUIET sfml-graphics)
    endif()

    if(SFML3_FOUND)
        add_executable(game_v1_sfml
            src/main_sfml.cpp
        )
        target_include_directories(game_v1_sfml PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../src ${SFML3_INCLUDE_DIRS})
        target_link_directories(game_v1_sfml PRIVATE ${SFML3_LIBRARY_DIRS})
        target_compile_options(game_v1_sfml PRIVATE ${SFML3_CFLAGS_OTHER})
        target_link_libraries(game_v1_sfml PRIVATE ${SFML3_LIBRARIES})
        message(STATUS "SFML 3 encontrado por pkg-config: se construira game_v1_sfml")
    else()
        message(STATUS "SFML no encontrado: se construira solo game_v1_cli")
    endif()
endif()
