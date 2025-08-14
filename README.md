# Snake Rewind 🎮🐍

Snake Rewind is a retro modern remake of the classic Snake game, built from scratch in **C** using **Raylib**. But this isn't just Snake - it's Snake with a twist: clones of your past moves become your greatest threat, all wrapped in a glowing CRT-inspired visual package.

https://github.com/user-attachments/assets/f498b315-90d5-489f-8a1e-162d00eeebd8

## 🎥 Watch the YouTube Series

[Snake Rewind playlist](https://www.youtube.com/playlist?list=PLI7p1zrAYQeUb67kPtsyduLk1aJChzIQZ)

## 🚀 What This Project Includes

- 🧱 **Grid-based game loop**: A pixel-perfect framebuffer with crisp resolution scaling
- 🎮 **Snake movement**: Step-based updates, buffered input and clean direction logic
- 🍎 **Food mechanics**: Growth and path evolution tied to food collisions
- 🧠 **Clone system**: Time delayed enemies that replay your exact path
- 💀 **Collision + game over**: Self and clone collision with freeze + retry UI
- 💥 **Score feedback**: Animated UI using scale + rotation for juicy feedback
- ✨ **Shader-based post-processing**:
  - Bloom / glow effect
  - CRT scanlines
  - Flicker and distortion
  - Chromatic aberration

## 🧠 What You Can Learn

This codebase is packed with examples of:

- Writing a 2D game loop in C without an engine
- Using Raylib's `RenderTexture2D` and shaders
- Creating pixel perfect layouts and scaling
- Animating UI elements (text) with transformation matrices
- Managing time based movement and update loops
- Building gameplay state machines (play, game over, retry)
- Designing postprocessing shader pipelines

It's approachable for intermediate devs and rewarding for those who want to push Raylib to the edge.

## 🧪 Building and Running

To build and run Snake Rewind on your system, you'll need:

### ✅ Prerequisites

- **C compiler** supporting **C23**
- **CMake** 3.16 or higher
- [**Raylib**](https://www.raylib.com/) installed on your system

---

### 🛠 Build Instructions

From the root of the project:

```bash
cmake -B build
cmake --build build
```

This will produce the `snake_rewind` executable in the `build/` directory.

### ▶️ Run the Game

```bash
./build/snake-rewind
```

## 🗃️ External Resources

These were helpful while building Snake Rewind:

- [Raylib examples](https://www.raylib.com/examples.html) - a collection of examples for a quick start using Raylib
- [LearnOpenGL.com: Bloom](https://learnopengl.com/Advanced-Lighting/Bloom) - our main source for implemetation of bloom/glow shader
- [Creating a scanline effect in Shadertoy](https://agatedragon.blog/2024/01/26/shadertoy-scanline/) - initial source for scanline shader, but we built our own thing
- [Mini: Chromatic Aberration](https://mini.gmshaders.com/p/gm-shaders-mini-chromatic-aberration)

## 🙏 Acknowledgements

- [Snake Snake Snake by Sheepolution](https://sheepolution.itch.io/) Huge thanks to the original creator of the Snake clone that inspired this project. I discovered it online and decided to fully rebuild and expand on the concept from scratch - exploring deeper gameplay, polish and shaders. This wouldn't exist without that spark of inspiration.
