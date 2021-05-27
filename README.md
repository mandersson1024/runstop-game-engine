## RUNSTOP Game Engine 
This project is for my own enjoyment and education. Anyone who sees this is welcome to have a look, though. If you want to build it you will have to figure out how to link the assimp lib, which is not included because of file size restrictions on GitHub.

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
    * Shadowmap (one direct light source, hard shadows)
    * Skybox
    * Implemented using DirectX 11
  * Resource System
    * Multi-threaded asset loading
    * Asset hot reload in editor
