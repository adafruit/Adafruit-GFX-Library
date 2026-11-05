### Build fontconvert on Windows with MSYS2

#### Step 1: Install MSYS2

Download and run the installer from [msys2.org](https://www.msys2.org/).

Open the "MSYS2 UCRT64" shell from the Start menu.
The packages below only work in that shell.

#### Step 2: Install the compiler and FreeType

```
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-freetype mingw-w64-ucrt-x86_64-pkgconf make
```

Check that pkg-config finds FreeType:

```
pkg-config --modversion freetype2
```

It should print a version number. If it prints nothing, `make` will fail with "ft2build.h not found".

#### Step 3: Build fontconvert

Go to the fontconvert folder of the library, for example:

```
cd "/c/Users/$USERNAME/Documents/Arduino/libraries/Adafruit_GFX_Library/fontconvert"
make
```

The Makefile finds FreeType with pkg-config, so no edits are needed.

This creates `fontconvert.exe`.

#### Step 4: Make a font

```
./fontconvert yourfont.ttf 9 > yourfont9pt7b.h
```

Put the header in your sketch folder and `#include` it.

More on fonts: [Adafruit GFX guide](https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts).
