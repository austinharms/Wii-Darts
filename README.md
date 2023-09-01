# Wii Darts
A proof of concept darts game for the Wii  
This project is simply some code I wrote to play with the Wii and see how it works  
Contains basic gameplay mechanics and placeholder models  
This is by no means a complete or playable game 

## Building
### Tools & Dependencies  
 - Visual Studio 202
 - devkitpro
 - Dolphin emulator (or a Wii)

### Running the projects
1. Clone the repo
2. Clone the submodules  
***Note:** This project uses Dear ImGui as it's GUI library and is include it as a submodule*
2. Open the `Wii-Darts.sln`
3. Depending on the target device (Wii or Dolphin) Select the correct project configuration  
***Note**: if using wiiload ensure the proper system environment variables are set*  
***Note**: if using Dolphin ensure the dolphin executable is in your system path*
4. Build the `Darts` project
5. Start the project