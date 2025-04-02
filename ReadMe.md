# Starship

![Static Badge](https://img.shields.io/badge/C%2B%2B-deepskyblue?style=flat)
![Static Badge](https://img.shields.io/badge/Custom%20Engine-dodgerblue?style=flat)

A 2D space shooter developed in my custom C++ game engine.

## How to Use

> This project was developed using Visual Studio, and it is recommended that Visual Studio and the MSVC toolchain be used for compiling from source. For best results, use VS2022.

The project requires my [custom C++ game engine](https://github.com/shreyasnisal/GameEngine) and a specific directory structure:

```
Engine
└── Code
Starship
├──Code
└──Run
```
### Attract Screen

| Key           | Action                            |
|---------------|-----------------------------------|
| N             | Start game                        |
| Space         | Start game                        |
| Xbox A        | Start game                        |
| Xbox Start    | Start game                        |
| X             | Show Xbox controller visual test  |
| Escape        | Exit game                         |

### Xbox Controller Visual Test

| Key           | Action                   |
|---------------|--------------------------|
| Escape        | Return to attract screen |

### Game

| Key                   | Action                                                                                                |
|-----------------------|---------------------------------------------------------------------------------------                |
| E                     | Accelerate PlayerShip in current forward direction                                                    |
| S                     | Rotate PlayerShip counter-clockwise                                                                   |
| F                     | Rotate PlayerShip clockwise                                                                           |
| D                     | Deaccelerate PlayerShip                                                                               |
| Space                 | Fire bullet in ship's current forward direction                                                       |
| Escape                | Exit to attract screen                                                                                |
| N                     | Respawn playership when dead (No longer valid since Playership does not have lives)                   |
| H                     | Enter hyper-speed mode to get PlayerShip back to Earth (only works when in the "exploration" state    |
| Q                     | Start a Quest when prompted (when PlayerShip is interacting with a planet)                            |
| Xbox Left Joystick    | Accelerate and rotate ship                                                                            |
| Xbox A                | Fire bullet in ship's current forward direction                                                       |
| Xbox Start            | Respawn playership when dead (no longer valid since PlayerShip does not have lives)                   |
| Xbox Back             | Exit to attract screen                                                                                |

In addition to the above controls available to the player, the game also offers a number of *developer cheats* that can be used for effective in-game information and debugging. The below table lists the available developer cheats. These cheats are only available in-game (not available on the attract screen).

| Key           | Action                                                                                    |
|---------------|-------------------------------------------------------------------------------------------|
| P             | Pause the game                                                                            |
| T             | Slows the game down to 1/10th of the normal speed                                         |
| O             | Runs a single frame of the game and then pauses the game                                  |
| I             | Spawns a new asteroid just off screen                                                     |
| K             | Destroys all enemies                                                                      |
| F1            | Toggles the *Debug Draw* mode which shows additional debug information for all entities   |
| F8            | Completely resets the game (Destroys the instance of Game class and initializes it again) |

