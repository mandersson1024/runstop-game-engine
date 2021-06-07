## RUNSTOP Game Engine 

![runstop_demo_scene](https://user-images.githubusercontent.com/30025913/119792509-091b6080-bed6-11eb-99ca-4fe389259724.png)

This project is for my own enjoyment and education. Anyone who sees this is welcome to have a look, though. If you want to build it you will have to figure out how to link the assimp lib, which is not included because of file size restrictions on GitHub.

The code runs on Windows using DX11. I'm deliberately ignoring portability to be able to spend more time on the core concents of the engine.

### Overview
* Language and Platform
  * C++20
  * Win32
  * DirectX 11
* Project Structure
  * Engine (static lib)
  * Editor (application)
* Features
  * Renderer
    * Physically-Based Rendering
    * Skybox/Radiance/Irradiance
    * Shadowmap (one direct light source, hard shadows)
  * Resource System
    * Multi-threaded asset loading
    * Asset hot reload in editor
  * Other
    * Raycasting
