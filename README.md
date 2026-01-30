# Rex Framework

Rex Framework is a lightweight 2D game engine built on top of [SDL2 2.32.10](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.10).

Designed for simplicity and modularity, Rex provides an easy-to-use API for building games ranging from classic arcade titles to simulation and sandbox-style projects.

---

## Game Project Tree

```
mygame/
├── main.lua
└── assets/
    ├── player.png
    ├── jump.wav
    └── fonts/
        └── myfont.ttf
```

---

## Game Run 

```bash
cd mygame
rex .
```

---

## Example main.lua

```lua
rex.log("Hello Rex!")

player = rex.entity.spawn_player(100, 100)

function update(dt)
    local vx = 0
    if rex.input.held(rex.key.A) then vx = -200 end
    if rex.input.held(rex.key.D) then vx =  200 end
    rex.entity.set_velocity(player, vx, 0)
end

function draw()
    rex.gfx.clear(20, 20, 30)
    rex.entity.render_all()
end
```

---

## Build

### Python Build

1. Build

```bash
python3 x.py
```

2. Install

```bash
sudo python3 x.py install
```

---

### CMake Build

1. Build

```bash
cmake -S . -B build
cmake --build build -j
```

2. Install

```bash
sudo cmake --install build
```

---

## Example Projects

All sample projects can be found under the [`Examples/`](./Examples) directory:

- `entity.cpp` — Basic entity movement and rendering  
- `physics.cpp` — Gravity and collision test  
- `world.cpp` — Simple tile world rendering  
- `audio.cpp` — Music and sound effect playback  
- `text.cpp` — Text rendering demo

---

## License

Rex Framework uses [SDL2 2.32.10](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.10) as its underlying multimedia backend.

Rex Framework licenses apply its own licenses derived from MIT licenses.

---

*Rex Framework — Build your own 2D world.*
