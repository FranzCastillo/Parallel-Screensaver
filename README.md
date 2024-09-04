# Parallel-Screensaver
In teams of three members, we were tasked with designing and implementing a sequential algorithm to solve a problem that has potential for parallelization. The chosen problem was to simulate a spiral galaxy screensaver, where stars rotate and move toward the center in a spiral pattern. Once the sequential version of the program was completed, the goal was to modify and improve the algorithm by parallelizing the computations to achieve better performance. The performance improvements are analyzed based on the concepts of speedup and efficiency studied in the course.

## Features
* Simulates a spiral galaxy with up to 30,000 stars.
* Each star's position and rotation are updated in a smooth and continuous manner.
* Stars are colored based on their distance from the center, creating a gradient effect from yellow (at the center) to blue and violet (at the edges).
* Randomly appearing and disappearing stars that fade over time.
* Real-time FPS counter displayed for performance monitoring.

## Parameters
The user has the option to either use default parameters or provide custom values:
* Number of Points: (5000 - 30000) Number of stars in the galaxy.
* Maximum Radius: (300.0 - 450.0) Maximum size of the galaxy.
* Speed: (0.035 - 0.099) Speed of the stars as they move towards the center.
* Base Rotation Speed: (0.0007 - 0.007) Speed at which the stars rotate around the center.

## Program versions

### Sequential Version ```main.cpp```

#### Description
The sequential version of the Spiral Galaxy screensaver generates a visualization of a rotating galaxy with spiral arms. Points (representing stars) are animated to rotate and move towards the center of the galaxy while following a spiral pattern. Additionally, extra stars appear and fade randomly across the galaxy, enhancing the visual effect.

### Parallel Version ```main_omp.cpp```

#### Description
The parallel version of the Spiral Galaxy screensaver takes advantage of multi-core processors by using OpenMP to parallelize the computation of the star positions and animations. This version offers the same visual output as the sequential version but performs better on systems with multiple CPU cores.


## Getting started
### Using SFML as Graph Lib

**Note:**
> You can see wich compiler CLion is using by going to file>settings>Toolchains

1. Download `GCC 13.1.0 MinGW (SEH) - 64-bit` if you are using MinGW, or the Visual C++ options if you are using VisualStudio compiler [here](https://www.sfml-dev.org/download/sfml/2.6.1/)
3. Unzip the file in any accesible folder
4. In [CMakeLists.txt](CMakeLists.txt) change the line `set(SFML_DIR "C:/SFML/SFML-2.6.1/lib/cmake/SFML")` to you own SFML file directory, as `"<SFML_Directory>/lib/cmake/SFML"`
5. Go to file>settings>CMake and in the field of CMake options, add this: `-DCMAKE_INCLUDE_PATH=<SFML_Directory>\include -DCMAKE_LIBRARY_PATH=<SFML_Directory>\lib`
6. Add the `<SFML_Directory>\bin` route to your PATH Environment variable
7. Enjoy :smile:



