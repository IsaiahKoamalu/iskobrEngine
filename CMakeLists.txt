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

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(glm)


add_library(engine
        # src dir
        src/Engine/Engine.cpp
        src/Engine/InputManager.cpp
        src/Engine/EntityManager.cpp
        src/Editor/main_editor.cpp

        # include dir
        include/Engine/Engine.h
        include/Engine/InputManager.h
        include/Engine/EntityManager.h
        include/Engine/ComponentManager.h
        include/Engine/System.h
        include/Engine/SystemManager.h
        include/Engine/TilesetManager.h
        include/Engine/Components/SpriteComponent.h
        include/Engine/Components/Position.h
        include/Engine/Components/Velocity.h
        include/Engine/Components/DirectionComponent.h
        include/Engine/Components/PlayerComponent.h
        include/Engine/Components/ColliderComponent.h
        include/Engine/Components/ActorComponent.h
        include/Engine/Components/TileComponent.h
        include/Engine/Components/HealthComponent.h
        include/Engine/Components/AttackColliderComponent.h
        include/Engine/Systems/CameraSystem.h
        include/Engine/Systems/TileMapSystem.h
        include/Engine/Systems/TriggerSystem.h
        include/Engine/Systems/RenderSystem.h
        include/Engine/Systems/ActorSystem.h
        include/Engine/Systems/MovementSystem.h
        include/Engine/Systems/PhysicsSystem.h
        include/Engine/Systems/GroundResetSystem.h
        include/Engine/Systems/ParticleSystem.h
        include/Engine/Systems/DamageSystem.h
        include/Engine/Systems/KnockBackSystem.h
        include/Engine/Core/TileType.h
        include/Engine/Components/WallClingComponent.h
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
        glm::glm
)

add_executable(tilemap_editor src/Editor/main_editor.cpp)
target_link_libraries(tilemap_editor PRIVATE sfml-graphics sfml-window sfml-system)
target_include_directories(tilemap_editor PRIVATE include)

add_executable(${PROJECT_NAME} main.cpp
        src/Engine/EntityManager.cpp
        src/Engine/InputManager.cpp
        include/Engine/InputManager.h
        include/Engine/EntityManager.h
        include/Engine/ComponentManager.h
        include/Engine/System.h
        include/Engine/SystemManager.h
        include/Engine/TilesetManager.h
        include/Engine/Components/PlayerComponent.h
        include/Engine/Components/SpriteComponent.h
        include/Engine/Components/Position.h
        include/Engine/Components/Velocity.h
        include/Engine/Components/AnimationComponent.h
        include/Engine/Components/TileComponent.h
        include/Engine/Components/DirectionComponent.h
        include/Engine/Components/ColliderComponent.h
        include/Engine/Components/WallClingComponent.h
        include/Engine/Components/HealthComponent.h
        include/Engine/Components/AttackColliderComponent.h
        include/Engine/Systems/TriggerSystem.h
        include/Engine/Systems/AnimationSystem.h
        include/Engine/Systems/CameraSystem.h
        include/Engine/Systems/TileMapSystem.h
        include/Engine/Systems/PlayerInputSystem.h
        include/Engine/Systems/RenderSystem.h
        include/Engine/Systems/MovementSystem.h
        include/Engine/Systems/PhysicsSystem.h
        include/Engine/Systems/ParticleSystem.h
        include/Engine/Systems/GroundResetSystem.h
        include/Engine/Systems/DamageSystem.h
        include/Engine/Core/TileType.h
        include/Engine/Systems/KnockBackSystem.h
        include/Engine/Components/KnockBackComponent.h
)

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