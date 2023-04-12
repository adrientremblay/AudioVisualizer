# Audio Visualizer

This application is an Audio Visualizer. In broad terms that means an application that streams data from an audio file 
and uses this data to drive some sort of visualization. There are infinitely many types of visualization but the most
common and straightforward one is a visualization as bars.

These visualizations involve analyzing the audio data to extract the frequency amplitudes then displaying 
these amplitudes in bars (more complete explanation in my video here <INSERT LINK HERE>).

My application features bars which can be rendered in 2D or 3D and arranged in a line or a circle. Additionally,
the bars can be spun around the x-axis, and the camera can spin around the origin around the y-axis. I also implemented
(though poorly) a mesh visualization. This visualization renders a wireframe "Suzanne" head model (the one from Blender)
as a wireframe and moves vertices along their normals depending on the frequency data.

## Implementation

The rendering is done directly using OpenGL. The SFML library was used to create a window context and also for some 
audio processing functionality. The FFTW library was used for the fourier transform stuff to determine the frequency
amplitudes. Additionally, the Assimp library was used for model loading, and the TGUI library was used for the GUI.

## Running

1. Clone the repo
2. Install SFML. On Debian based distros do:
    ```sudo apt install libsfml-dev```
3. Install FFTW. On Debian based distros do:
    ```sudo apt install libfftw3-dev```
4. Install Assimp. On Debian based distros do:
    ```sudo apt install libassimp-dev```
4. Install SDL2. On Debian based distros do:
   ```sudo apt install libsdl2-dev```
5. Install TGUI. On Debian based distros do:
    ```
   sudo add-apt-repository ppa:texus/tgui
   sudo apt update
   sudo apt install libtgui-1.0-dev
    ```
6. Install SDL2_TTF. On Debian based distros do:
    ```sudo apt-get install libsdl2-ttf-dev```
7. Copy the /glad folder to /usr/include on Debian based distros or install Glad on whatever operating system you're on