cmake_minimum_required(VERSION 3.28)
project(iskobrEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch SFML
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

add_library(engine
        # src dir
        src/Engine/Engine.cpp
        src/Engine/InputManager.cpp
        src/Engine/EntityManager.cpp

        # include dir
        include/Engine/Engine.h
        include/Engine/InputManager.h
        include/Engine/EntityManager.h
        include/Engine/ComponentManager.h
        include/Engine/System.h
        include/Engine/SystemManager.h
        include/Engine/MovementSystem.h
        include/Engine/SpriteComponent.h
        include/Engine/RenderSystem.h
)

target_include_directories(engine
        PUBLIC
            include
)

target_link_libraries(engine
        PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

add_executable(${PROJECT_NAME} main.cpp
        include/Engine/InputManager.h
        src/Engine/InputManager.cpp
        include/Engine/EntityManager.h
        src/Engine/EntityManager.cpp
        include/Engine/ComponentManager.h
        include/Engine/System.h
        include/Engine/SystemManager.h
        include/Engine/MovementSystem.h
        include/Engine/SpriteComponent.h
        include/Engine/RenderSystem.h)

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Assets copy
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-audio>
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()