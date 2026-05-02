# COMP2113_minigame_project : THE LEGEND OF HKU

**The game is inspried by The Legend of Zelda**

The purpose of the project is to recreate a source code base for the game from scracth.
The main game will be based on text and ASCII art.

## How to Play
Run: 
`make && ./build/game` or `make`


## Basic game settings

| Symbol | Meaning |
| ------ | ------- |
| ` ` | Walkable tile |
| `#` | Obstacle (unwalkable) |
| `\|` | Vertical wall border |
| `-` | Horizontal wall border |
| `+` | Wall corner |
| `^` `v` `<` `>` `P` | Player (facing up/down/left/right) |
| `E` | Enemy (Slime) |
| `B` | Boss |
| `♥` | Health heart (drops from slimes, heals player for 10 HP) |
| `*` | Attack particle |
| `o` | Enemy projectile |

## Usage and Controls

| Key | Action |
| -------- | -------- |
| W | Up |
| A | Left |
| S | Down |
| D | Right |
| Space bar | Attack |
| C | Activate shield (reflects projectiles) |
| P | Pause | 
| Q | Quit | 

## Difficulty

| Difficulty | Enemies per room | Enemy Speed |
| ---------- | ---------------- | ----------- |
| Easy | 1 Slime | Slow |
| Normal | 2 Slimes | Medium |
| Hard | 3 Slimes | Fast |

## Features

### Save & Load
- When quitting with `Q`, the game asks if you want to save progress
- On next launch, the game detects a save file and asks to continue or start fresh
- Saves current room, player position, difficulty, and enemy states

### Combat
- Press `Space` to attack in the direction you are facing
- Player has invincibility frames after taking damage (~2 seconds)
- Player blinks while invincible
- Press `C` to activate shield - reflects incoming projectiles
- Enemies shoot projectiles (`o`) when within close range
- Enemies wander slowly when the player is far, and chase when nearby
- **Cannot walk into slimes or boss** - collision detection prevents overlapping with enemies

### Health & Healing
- Slimes drop health hearts (`♥`) when defeated
- Walk over a heart to collect it and heal **10 HP** (capped at max HP)
- Hearts disappear when collected or when transitioning to a new room
- Use combat strategically to defeat slimes and collect healing items

### Room Transitions
- Rooms are connected via north/east/south/west exits
- Walk to the edge of a room to automatically transition to the next room
- Enemies respawn when entering a new room

### Enemy Behavior
Enemies have different behaviors depending on the situation. 
Stay alert — they are smarter than they look!

### HUD
- Displays current HP as hearts (`<3`) with color indicator:
  - Green → healthy
  - Yellow → moderate damage
  - Red → low HP
- Shows available controls at the bottom of the screen

## Team
- The Legend of HKU Team
  - Apinat Ngampanpaisan as **arm35177**
  - Nattapat Chantarojsiri as **pK69420**
  - Nuththawat Kitchongcharoenying as **Nu7h**
  - Patanin Ngaensupalak as **Patanin-ng**
  - Pattarapon Thepsuthin as **Santaclaus007x**
  - Supakorn Surarat as **Amp-ss**
