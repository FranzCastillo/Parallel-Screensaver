# Parallel-Screensaver

# Getting started
## Using SFML as Graph Lib

**Note:**
> You can see wich compiler CLion is using by going to file>settings>Toolchains

1. Download `GCC 13.1.0 MinGW (SEH) - 64-bit` if you are using MinGW, or the Visual C++ options if you are using VisualStudio compiler [here](https://www.sfml-dev.org/download/sfml/2.6.1/)
3. Unzip the file in any accesible folder
4. In [CMakeLists.txt](CMakeLists.txt) change the line `set(SFML_DIR "C:/SFML/SFML-2.6.1/lib/cmake/SFML")` to you own SFML file directory, as `"<SFML_Directory>/lib/cmake/SFML"`
5. Go to file>settings>CMake and in the field of CMake options, add this: `-DCMAKE_INCLUDE_PATH=<SFML_Directory>\include -DCMAKE_LIBRARY_PATH=<SFML_Directory>\lib`
6. Add the `<SFML_Directory>\bin` route to your PATH Environment variable
7. Enjoy :smile:
