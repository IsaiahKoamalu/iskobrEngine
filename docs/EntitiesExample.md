**Example of a Valid Entity File**
-

```json
[
  {
    "name": "Player",
    "player": true,
    "sprite": true,
    "health": true,
    "spritePath": {
      "idleLeft": {
        "name": "idleLeft",
        "filePath": "assets/idle_left.png",
        "frameCount": 10,
        "frameTime": 0.1
      }
    },
    "anim": {
      "start": "idleLeft",
      "frameWidth": 48,
      "frameHeight": 48
    },
    "Position": {
      "x": 150,
      "y": 0
    },
    "Velocity": {
      "dx": 0,
      "dy": 0
    }
  }
]

```
