# Spectral Engine (WIP)
![Platform](https://shields.io/badge/MacOS--9cf?logo=Apple&style=social)
![GitHub](https://img.shields.io/github/license/Nic-Urb/SpectralEngine?color=blue&style=flat-square)
![Size](https://img.shields.io/github/repo-size/Nic-Urb/SpectralEngine?style=flat-square)

## About
Spectral Engine is an in-development game engine written in C++.

<img width="1582" alt="Screenshot 2024-06-17 at 20 53 35" src="https://github.com/Nic-Urb/SpectralEngine/assets/79403684/3edf49f6-08b6-4ed0-8bea-7df67cf78aaa">

## Features

| Feature           | Description                                       | Progress  |
| -------           | -----------                                       | --------  |
| **Scripting**     | LUA scripting using Sol2                          | ✅ Done    |
| **Physics**       | 3D and 2D physics using JoltPhysics and Box2D     | ✅ Done    |
| **Scene system**  | scene management + scene serialization with YAML  | ✅ Done    |
| **Angle support** | backend graphics support for Metal                | ✅ Done    |
| **ECS support**   | Entity-System-Component through Entt              | ✅ Done    |


## TODO

| Feature                | Description                                      | Progress              |
| -------                | -----------                                      | --------              |
| **Native Scripting**   | option to use native c++ scripting               | 🛠️ WIP |
| **Lighting System**    | basic lighting system                            | 🛠️ WIP |
| **Materials**          | setting to choose material for meshes            | 🛠️ WIP |
| **3D Mouse Picking**   | 3D mouse picking for entities using ray casting  | 🛠️ WIP |
| **Shadow Mapping**     | with soft and hard shadows support               | 📋 Not Yet Started |
| **Audio**              | audio system using FMOD                          | 📋 Not Yet Started |
| **Generating Project** | generates project files                          | 📋 Not Yet Started    |
| **Animation System**   | 3D skeletal animation system                     | 📋 Not Yet Started    |
| **Precompiled Shaders**| precompile all the shaders and cache them for GPU| 📋 Not Yet Started    | 



## Premake

Generate project file for XCode:

cd to root folder, then `./premake5 xcode4`,  this should generate a Xcode workspace.

Now you need to set the correct working directory. Currently, we're using SpectralEditor as our working directory, but if you want to use Sandbox, then the Sandbox directory would be your working directory.


## Third Party Dependencies
- [**Raylib**](https://github.com/raysan5/raylib) graphics library
- [**Angle**](https://github.com/google/angle) adds support for Metal rendering backend
- [**Imgui**](https://github.com/ocornut/imgui) graphical UI library
- [**Spdlog**](https://github.com/gabime/spdlog) logging
- [**Yaml**](https://yaml.org/) serialization
- [**TinyFileDialogs**](https://github.com/native-toolkit/libtinyfiledialogs) save/load file dialog
- [**Entt**](https://github.com/skypjack/entt) ECS
- [**Sol2**](https://github.com/ThePhD/sol2) LUA scripting
- [**Box2D**](https://github.com/erincatto/box2d) 2D physics
- [**JoltPhysics**](https://github.com/jrouwe/JoltPhysics) 3D physics

## License
>See the [LICENSE](https://github.com/Nic-Urb/SpectralEngine/blob/main/LICENSE) file for details. This project is licensed under the terms of the MIT license.

## Special Mentions
Jason Gregory (2008), [Game Engine Architecture](https://www.gameenginebook.com)

The Cherno, Game Engine [video series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)

[Raylib Discord](https://discord.com/invite/raylib) community

