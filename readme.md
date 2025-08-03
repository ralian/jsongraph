# jsongraph

Node/Graph data editor I designed for editing generic json files. Think blueprints from UE, but not really - way more generic than that.

I am aware of [imgui-node-editor](https://github.com/thedmd/imgui-node-editor), it is very nice and while some of the ideas there inspired me, I have slightly different goals.

You can read about the goals behind this project [here]().

I also started work on this in Python but decided I couldn't deal with the weirdness of the bindings. Back to C++ again ¯\_(⊙︿⊙)_/¯

## Build Instructions

Dependencies (CMake will find and install these as necessary for you)
- OpenGL 3 (of course)
- GLFW 3.4
- Dear Imgui v1.92.1

### Windows

System requirements:
- Developer Powershell with MSVC
- CMake 3.24+
- Ninja

From a developer powershell, simply run
```
cmake --preset debug
cmake --build --preset debug
./build/debug/jsongraph.exe
```
The configuration call to cmake should pull the correct versions of all the dependencies.

### Linux

System requirements:
- GCC/Clang - (need to nail down the requried versions required)
- CMake 3.24+
- Ninja

Should be the exact same steps, except of course just run `./build/debug/jsongraph` to start the executable.

### Emscripten

Hell if I know. But, I want to get this build working. Actually, one possiblity could be clang with the new webassembly stuff instead?

