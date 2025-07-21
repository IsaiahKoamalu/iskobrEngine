---
layout: default
title: Main
---

<style>
  .button-container {
    display: flex;
    justify-content: center;
    margin-top: 20px;
  }
  .button {
    display: inline-block;
    padding: 0.5em 1em;
    background-color: #0066cc;
    color: #000;
    text-decoration: none;
    border-radius: 4px;
    font-family: sans-serif;
  }
  .button:hover {
    background-color: #0056b3;
</style>

<div class="button-container">
  <a href="{{ '/about/' | relative_url }}" class="button">
    About
  </a>
</div>

# **DOCUMENTATION**

**ENGINE CLASS**
-

The Engine class is the heart of this program. It is responsible for the following:
- Defines the run function which houses the main loop. ``void run(bool debugMode);``
- Defines all manager objects as unique pointers.
- Defines all system objects as shared pointers.
- Defines the render window.
- Defines an overall event processing function. ``void processEvents();``
- Defines the main update function in which all separate system updates are called. ``void update(float dt, sf::Time tDt);``
- Defines a function that loads and configures all entities from the entities.json file. ``bool loadEntities(std::string &filepath);``

Within the run function you will see the logic required for setting up the engine, the managers are first assigned followed by the registration of all systems
to the system manager with the following format: ``thisSystem = systemManager->registerSystem<ThisSystem>();``
Due to the fact that the particle system is responsible for resolving its own collisions, an instance of the collision system
is passed to the particle system via the following line below system registration: ``particleSystem->setCollisionSystem(collisionSystem.get());``

After the system logic you will find the logic responsible for loading the entity data from entities.json using the `loadEntities()` function which is called
with a shared pointer to the entity file. The load function utilizes the nlohmann json.hpp file which can be found within the json directory that resides in the external directory.
The function begins by open the file and creating an entity via this line: ``Entity entity = entityManager->createEntity()``. It then parses the entity file and assigns the relevant systems and components accordingly.

The next step in the run function involves the addition of tile sets to the tileset manager and the loading of the tile map through the tile map system.

The final logic is a while loop which is the main engine loop. Within this loop delta time is constantly calculated. Followed by a call to the `processEvents();` function which as of now is really only responsible for closing the window correctly.
The event function is proceeded by the `update();` function which takes the float delta time as well as the delta time as an object of `sf::Time` (for the particle system). Within the update function you will see all systems calling their own respective update functions (save for the render system which calls its update function in the actual render function).
You will also notice the line `window.setView(cameraSystem->view)` which hooks the camera system up to the window. The last line calls the `void render()` function which is responsible for clearing the window and implementing the render systems update function.

**EDITING THE LEVEL INSTANCE**
-
The current level can be accessed and edited by running the tile map editor. While the editor is running the current tileset will be displayed on the left hand side of the window, you can select a tile by left clicking on it and place it in the level my left clicking on the desired location. You can cycle through the different tilesets by pressing E (right) and Q (left), however only the grass tileset has collision setup right now. The editor also currently supports undo functionality with 'ctrl z'.  As of now the size of the map is pretty restricted and fixed, I plan on expanding the features of the editor soon. Once you are satisfied with the level changes you can press 's' and the level data will be written to the level.json file.

**ADDING AND REMOVING ENTITIES** 
-
Entities can be added by adding the required and desired entity information to the entities.json file that is read and parsed automatically within the load function of the Engine class.
The load function will automatically register entities to the proper systems and add the needed components based off the data that has been provided in the entities.json file.
It is important that no more than one entity is given a Player data entry that is equal to true.
Entities can be removed the same way by removing all of its corresponding information from the entities.json file.

For an example of what a valid entitiy file should look like please view [this page](EntitiesExample.md).

**PLAYER CONTROLS**
-
Player input is handled within the PlayerInput system, which is a system reserved only for the **one** entity with a Player flag that is equal to true.

**Keyboard & Mouse:**

  - A,D -> left and right movement.
  - Space -> jump, you can jump on to walls to utilize the WallCling component which allows for the player to jump back and forth between walls to reach higher spots. (currently still buggy)
  - C -> crouch, allows for movement under spaces that you would not be able to get through while standing.(as of now you cannot attack while crouching)
  - Shift -> roll, allows for the player to roll through select entities collision boxes, also allows for passage under low spaces (like crouch).
  - LMB -> attack, the attack system is directional, so if the player is facing right they will attack right and vice versa.
  - R -> reset player position.

**Controller:**

  - Left Stick -> left and right movement.
  - A bttn -> jump, you can jump on to walls to utilize the WallCling component which allows for the player to jump back and forth between walls to reach higher spots. (currently still buggy)
  - R3 -> crouch, allows for movement under spaces that you would not be able to get through while standing.(as of now you cannot attack while crouching)
  - B bttn -> roll, allows for the player to roll through select entities collision boxes, also allows for passage under low spaces (like crouch).
  - Rignt Bumper -> attack, the attack system is directional, so if the player is facing right they will attack right and vice versa.

**MANAGERS**
-
The managers of this engine are responsible for handling the creation, deletion and storage of different parts of the engine.
Such as entities, systems and components.

**Entity Manager**: The entity manager manages the creation, storage and deletion of entities which
are essentially id's that represent objects.

**Component Manager**: The component manager manages the creation, deletion and storage of components, which are
data containers that can be associated with entities in order to provide attributes and behaviors to that entity.

**System Manager**: The system manager manages the engines systems which are modules of logic that preform operations upon components.

**COMPONENTS**
-
All components are separate data containers that define and hold different attributes. When a component is assigned to an entity that entity now possesses the properties and attributes defined in the component.
The components are separate from the systems. The systems will operate on entities based on the components they possess.
*Ex*: If you want an entity to be able to take damage you must assign it a health component so that the damage system may operate on it accordingly.

**Actor Component**: The actor component currently does not define much logic, but it is a component meant to define the time of actor the entity is (hostile, interactable, player controlled, etc.).

**Animation Component**: The animation component defines two separate structs, AnimationData and the AnimationComponent itself. The animation data struct includes the frame count, current frame, frame width, frame height, frame time, and a shared pointer of a texture.
The animation component struct itself contains an unordered map of type string and AnimationData, a current state string, previous state string, current frame, and elapsed time.

**Attack Collider Component**: The attack collider component defines the data responsible for attack collision. This includes two separate right and left `sf::FloatRect` and there corresponding active booleans, a static boolean, a trigger identifier boolean, and a string tag.
The attack collider component is used in the collision system to determine collision while attacking as well as in the player input system where the active booleans are set according to player input.

**Collider Component**: The collider component is another component that defines properties of collision. It holds a `sf::FloatRect` for bounds, a static boolean, a trigger identifier boolean, an active boolean, and a string tag.

**Direction Component**: The direction component is a component that holds both an enumerated class 'Direction' and the DirectionComponent struct itself. The enum class holds four values, 'Up', 'Down', 'Left', 'Right'.
The struct defines an object of Direction which holds the current direction value.

```c++
enum class Direction{
    Up,
    Down,
    Left,
    Right
};

struct DirectionComponent {
    Direction current = Directon::Right; // Default value
}

```

**Health Component**: As mentioned earlier, the health component defines attributes related to health. Entities given a health component are subsequently able to be affected by the damage system and 'killed'.
The component defines a health amount, a low health boolean, a full health boolean, and an is-dead boolean.

**Player Component**: The player component is pretty much what it sounds like. It defines all the data that is associated with the player entity. 
However, this will more than likely change as I begin to move properties into the actor component since many actors also require data defined in the player component.

**Position Component**: The position component is just a struct with two float variables x and y that determine an entities current position the screen.
Nearly all entities will require a position component.

**Tile Component**: The tile component defines properties of the level tiles. It holds a 

**Sprite Component**: The sprite component defines properties related to... you guessed it, a sprite! The struct defines a sf::Sprite as well as a flipX boolean
that is really only utilized if you don't want to have to make the opposite side of a sprite. An entity needs a sprite component in order to be registered to the animation system effectively.

**Tile Component**: The tile component holds the data that makes up all the tiles of a map. It defines a `sf::Sprite`, a `TileType`, an is-solid boolean, an is-animated boolean, and a tile id.
When level files are loaded the tiles are automatically given tile components with the necessary data.

**Velocity Component**: The velocity component defines velocity using two floats, dx and dy. Any entity that you wish to be affected by a movement based system needs to
possess a velocity component. **EX:** If you create an entity, and you want it to be influenced by gravity, then you must give it a velocity component since the physics system (where gravity resides)
calls upon the velocity component of all entities registered to it in a manner similar to the following.
```c++
auto& velocity = components.getComponent<Velocity>(entity);
velocity.dy += gravity * dt; // where dt is just delta time
```

**Wall-Cling Component**: The wall cling component is a component that defines the properties that allow an entity to preform a 'wall cling' which opens up wall jumping and sliding. 
As of right now, it is the only component that utilizes the glm library, it defines a vec2 variable that acts as a unit vector pointing out of a wall. The struct is composed of the following...
```c++
struct WallClingComponent {
    bool active {false};
    bool touchedThisFrame {false};
    float timer {0.0f};
    glm::vec2 wallNormal {0.0f, 0.0f};
    
    float maxTime {1.5f};
    float slideSpeed {0.0f};
    float jumpImpulse {350.f};
}
```

**SYSTEMS**
-
All systems are children that derive publicly from System and define their own component filters and behaviors. Entities registered to certain systems must possess the required components in order for the system to run correctly, 
there are multiple fail safes in place that attempt maintain component to system consistency. However, if a system expects a component from an entity that does not possess it will result
in a breach of the following assertion statement ``assert(components.find(entity) != components.end() && "Component not found.");`` this statement can be found in the get function of the ComponentManager.


**Animation System**: The animation system is responsible for grabbing all entities that possess an animation component. The animation system then updates the entities animation states and sets the correct textures.

**Camera System**: The camera system is responsible for maintaining and updating the correct frame of reference relative to the player.

**Collision System**: The collision system is responsible for identifying and resolving all collisions (except for particle collisions). It takes into account all entities that posses a collider component. Some entities may possess multiple different collider components. Ex: As of now the player possess both a base collider component and an attack collider component.

**Damage System**: The damage system is responsible for grabbing all entities that possess a health component and updating their health accordingly. The damage system will also destroy entities as well as remove them from the systems they have been registered to once their healthComponent.isDead = true. On top of this the damage system is also currently responsible for setting particle emitters and spawning particles when applicable.

**Ground Reset System**: The ground reset system is a small simple system whose only responsibility is to set the player component's isGrounded member variable to false in order to prevent bugs within certain movement functionalities.

**Movement System**: The movement system is responsible for grabbing all entities that possess both a position and velocity component. 
The movement system takes into account the entities current velocity and position in order to update its new velocity and position. 
The movement system is also responsible for preventing entities from leaving the set level boundaries.

**Particle System**: The particle system holds all the logic and data concerning particles and the way they behave within the engine. 
The particles are rendered within the render system just like all other entities. However, the collision logic is handled within itself in its update function. 
This is done through the declaration of a variable (m_collisionSystem) of type CollisionSystem which is assigned to a const collision system pointer. 
This allows for the access of the isSolidAt function which checks if a tile meets the requirements to collide with a particle.

**Physics System**: The physics system defines and implements physical forces on all entities that possess a velocity component, as of now the only physical force that is implemented through the physics system is gravity.

**Player Input System**: The player input system is responsible for receiving input through either keyboard & mouse or controller. Based on the input the system updates the player velocity as well as sets the current animation state for the animation system to process.

**Render System**: The render system is a pretty simple system that is responsible for rendering all entities as well as particles.

**Tilemap System**: The tilemap system is responsible for handling the parsing and loading of tilemaps. After the map file is loaded, the tile set and individual tile IDs are determined. Each tile is created is an object of Entity and is given a sf::Sprite with a texture.
The texture rect is then set according to the tile width and tile height data. It is then given a tile component and, if it is solid, registered to the collision system with a collider component that has bounds equal to its texture rect.

**Trigger System**: The trigger system is currently not in use, but it is meant to act as a type of collision system that triggers events instead of physically resolving collision instances.
Its logic is very similar to that of the collision system.

