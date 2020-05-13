# Adafruit GFX Library ![Build Status](https://github.com/adafruit/Adafruit-GFX-Library/workflows/Arduino%20Library%20CI/badge.svg)

## Description

The Adafruit_GFX library for Arduino provides a common syntax and set of graphics primitives (points, lines, circles, etc.). for all of our LCD and OLED displays. This allows Arduino sketches to easily be adapted between display types with minimal fuss…and any new features, performance improvements and bug fixes will immediately apply across our complete offering of color
displays.

## Installation

### First Method

![image](https://user-images.githubusercontent.com/36513474/68398431-89fcf500-0196-11ea-9157-9c95cecc1586.png)

1. In the Arduino IDE, navigate to Sketch > Include Library > Manage Libraries
1. Then the Library Manager will open and you will find a list of libraries that are already installed or ready for installation.
1. Then search for gfx graphics using the search bar.
1. Click on the text area and then select the specific version and install it.

### Second Method

1. Navigate to the [Releases page](https://github.com/adafruit/Adafruit-GFX-Library/releases).
1. Download the latest release.
1. Extract the zip file
1. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library

## Features

- ### Lower level funtions

    The GFX library handles the lower-level functions of the display devices by pairing with a hardware-specific library for each display device.

- ### Range of colors

    The GFX library also handles a range of color TFT displays connected via SPI.

- ### Give back

    The library is free, you don’t have to pay for anything. However, if you want to support the development, or just thank the author of the library by purchasing products from Adafruit!

    Not only you’ll encourage the development of the library, but you’ll also learn how to best use the library and probably some C++ too

- ### BSD License

    Adafruit_GFX library is open-source and uses one of the most permissive licenses so you can use it on any project.

  - Minimal restrictions
  - Proprietary use
  - Distribution

## Useful Resources

- Image2Code: This is a handy Java GUI utility to convert a BMP file into the array code necessary to display the image with the drawBitmap function. Check out the code at [ehubin's GitHub repository](https://github.com/ehubin/Adafruit-GFX-Library/tree/master/Img2Code)

- drawXBitmap function: You can use the GIMP photo editor to save a .xbm file and use the array saved in the file to draw a bitmap with the drawXBitmap function. See the [pull request](https://github.com/adafruit/Adafruit-GFX-Library/pull/31) here for more details.

- 'Fonts' folder contains bitmap fonts for use with recent (1.1 and later) Adafruit_GFX. To use a font in your Arduino sketch, #include the corresponding .h file and pass the address of GFXfont struct to setFont(). Pass NULL to revert to 'classic' fixed-space bitmap font.

- 'fontconvert' folder contains a command-line tool for converting TTF fonts to Adafruit_GFX header format.

---

## Roadmap

The PRIME DIRECTIVE is to maintain backward compatibility with existing Arduino sketches -- many are hosted elsewhere and don't track changes here, some are in print and can never be changed! This "little" library has grown organically over time and sometimes we paint ourselves into a design corner and just have to live with it or add ungainly workarounds.

Highly unlikely to merge any changes for additional or incompatible font formats (see Prime Directive above). There are already two formats and the code is quite bloaty there as it is (this also creates liabilities for tools and documentation). If you *must* have a more sophisticated font format, consider creating a fork with the features required for your project. For similar reasons, also unlikely to add any more bitmap formats, it's getting messy.

Please don't reformat code for the sake of reformatting code. The resulting large "visual diff" makes it impossible to untangle actual bug fixes from merely rearranged lines.

## Contributing

If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell others about this library
- Contribute new protocols

Please read [CONTRIBUTING.md](https://github.com/adafruit/Adafruit-GFX-Library/blob/master/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Credits

The library is written and maintained by Limor Fried/Ladyada for Adafruit Industries <info@adafruit.com>.

Other contributors are listed [here](https://github.com/adafruit/Adafruit-GFX-Library/graphs/contributors)

## License

This library is licensed under [BSD license](https://github.com/adafruit/Adafruit-GFX-Library/blob/master/license.txt).
