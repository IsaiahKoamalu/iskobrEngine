**2D Game Engine Written in c++ Utilizing ECS Architecture**
-

**EDITING THE LEVEL INSTANCE**
The current level can be accessed and edited through the tilemap editor. Once you are satisfied with the level changes press 's' and the level data will be written to the level.json file.
The editor also currently supports undo functionality with 'ctrl z'.

**ADDING AND REMOVING ENTITIES**
Entities can be added by adding the required and desired entity information to the entities.json file that is read and parsed automatically within the load function of the Engine class.
The load function will automatically register entities to the proper systems and add the needed components based off the data that has been provided in the entities.json file.
It is important that no more than one entity is given a Player data entry that is equal to true.
Entities can be removed the same way by removing all of its corresponding information from the entities.json file.

**PLAYER INPUT**
Player input is handled within the PlayerInput system, which is a system reserved only for the **one** entity with a Player flag that is equal to true.
Keyboard & Mouse:
  - A,D -> left and right movement.
  - Space -> jump, you can jump on to walls to utilize the WallCling component which allows for the player to jump back and forth between walls to reach higher spots. (currently still buggy)
  - C -> crouch, allows for movement under spaces that you would not be able to get through while standing.(as of now you cannot attack while crouching)
  - Shift -> roll, allows for the player to roll through select entities collision boxes, also allows for passage under low spaces (like crouch).
  - LMB -> attack, the attack system is directional, so if the player is facing right they will attack right and vice versa.
  - R -> reset player position.
Controller:
  - Left Stick -> left and right movement.
  - A bttn -> jump, you can jump on to walls to utilize the WallCling component which allows for the player to jump back and forth between walls to reach higher spots. (currently still buggy)
  - R3 -> crouch, allows for movement under spaces that you would not be able to get through while standing.(as of now you cannot attack while crouching)
  - B bttn -> roll, allows for the player to roll through select entities collision boxes, also allows for passage under low spaces (like crouch).
  - Rignt Bumper -> attack, the attack system is directional, so if the player is facing right they will attack right and vice versa.


**SYSTEMS**
All systems are children that derive publicly from System and define their own component filters and behaviors.

**Animation System**: The animation system is responsible for grabbing all entities that posess an animation component. The animation system then updates the entities animation states and sets the correct textures.

**Camera System**: The camera system is responsible for maintaining and updating the correct frame of reference relative to the player.

**Collision System**: The collision system is responsible for indentifying and resolving all collisions (except for particle collisions). It takes into account all entities that posses a collider component. Some entities may posess multiple different collider components. Ex: As of now the player possess both a base collider component and an attack collider component.

**Damage System**: The damage system is responsible for grabing all entities that posess a health component and updating their health accordingly. The damage system will also destroy entities as well as remove them from the systems they have been registered to once their healthComponent.isDead = true. On top of this the damage system is also currently responsible for setting particle emitters and spawning particles when applicable.

**Ground Reset System**: The ground reset system is a small simple system whose only responsibility is to set the playr component's isGrounded member variable to false in order to prevent bugs within certain movement functionalities.

**Movement System**: The movement system is responsible for grabbing all entities that posess both a position and velocity component. The movement system takes into account the entities current velocity and position in order to update its new velocity and position. The movement system is also responsible for preventing entities from leaving the set level boundaries.
