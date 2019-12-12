# LightRider

### Getting Started

#### Installing LightRider

Check out the [releases](https://github.com/MackinnonBuck/light-rider/releases) page to download the latest installer. After unzipping the download, run `setup.exe`. "LightRider" should now show up in your computer's list of programs.

#### Building from source on Windows

To build LightRider from source, make sure you have Visual Studio 2019 installed with the "Desktop development with C++" component installed. In addition, you'll need a folder named "OpenGL" installed to C:\ with GLFW and GLM dependencies inside. If you have GLFW and GLM installed to a different location on your machine, adjust the Visual Studio Project configuration to reflect those differences. Ensure you have the build configuration set to "x86", then the project should build successfully.

#### Running the game

Running LightRider without any command-line arguments will run it in normal mode. In normal mode, auto-accelerate is enabled, so if you have no friends and want to play the game by yourself, I recommend using the `-demo` command-line argument to disable auto-accelerate (that way, the other bike won't immediately crash into your trail).

#### Keyboad controls

| Action                | Player 1 keyboard binding | Player 2 keyboard binding |
| --------------------- | ------------------------- | ------------------------- |
| Accelerate (`-demo`)  | W                         | Up arrow                  |
| Turn left             | A                         | Left arrow                |
| Turn right            | D                         | Right arrow               |
| Jump                  | Space                     | Enter                     |
| Look at opponent      | Left shift                | Right shift               |

#### Xbox controller controls

| Action                | Controller binding   |
| --------------------- | -------------------- |
| Accelerate (`-demo`)  | Right trigger        |
| Turn                  | Left stick X axis    |
| Jump                  | A                    |
| Look at opponent      | B                    |
