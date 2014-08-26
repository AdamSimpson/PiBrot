# PiBrot
PiBrot is a parallel Mandelbrot set race. The screen is divided in half vertically with one MPI task drawing the left side image and an identical image being drawn on the right side with N-2 MPI tasks, where N is the total number of MPI tasks.

## Screenshot
Initially a single button appears along with an oak leaf cursor. When ready to start the race place the cursor over the start button and press `a`.
![alt text](https://raw.githubusercontent.com/AdamSimpson/PiBrot/master/images/start_screenshot.png "Start Screenshot")

It is easy to see that the right side image will be generated considerably faster than the left. The speedup should approximately be N-2 times.
![alt text](https://raw.githubusercontent.com/AdamSimpson/PiBrot/master/images/mid_screenshot.png "Mid Screenshot")

The completed image on the right as the left attempts to catch up
![alt text](https://raw.githubusercontent.com/AdamSimpson/PiBrot/master/images/final_screenshot.png "Final Screenshot")

## Install

Several prerequisites are required before compiling the code. In a Linux environment, such as Raspian, these may be obtained using your distros package management system. On Macintosh it is recomended that [Homebrew](http://brew.sh) be used.

### Macintosh

It is assumed that the XCode toolchain has been installed, this is freely available from [Apple](https://developer.apple.com/xcode/downloads/) . Once Homebrew has been installed The following commands may be run in Terminal.app to setup the prerequisties.

    $ brew install mpich
    $ brew install homebrew/versions/glfw3
    $ brew install glew

### Raspberry Pi

On the RaspberryPi the following packages must be installed, all of which are availble through apt-get. If you used the TinySetup scripts these should already be installed

    $ sudo apt-get install mpich
    $ sudo apt-get install libglew-dev

## Compile and run
Once the prerequisites have been installed PiBrot can be compiled and run.

### Macintosh

To compile

    $ make -f makefile_macos

To run on a 4 core single socket computer:

    $ mpirun -n 4 ./bin/pibrot

### Raspbery Pi
To Compile

    $ make

To run on TinyTitan

    $ make run

