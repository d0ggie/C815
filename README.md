C815
====

Preface
-------
This is small asset unpacker for a Finnish MS-DOS game called "Ryssän kauhu" from the late 1990s.  Details and quirks and commented in source code, so even if one does not read or understand C language code, it is always possible just to read those.  Naming of all files pays homage to this golden era and while the code has been cleaned up this is not a showcase how to make things.  Many checks are abstractions have been omitted.  Mostly due to that not being the driving force of this tool.

I have no ties to the game, so everything has been reverse engineered based on the original game executable;  Linear Executable (LE) image `RYSSA.EXE` (as a funny fact, executable of this tool will be much larger than the original game including all non-gameplay related 16-bit loader stubs) was at first relocated, all relocation data processed and then loaded on the one and only IDA.  The first stage must be done as IDA has long since dropped these essential relocation features of MS-DOS and OS/2 era.  Without relocation data processed, it is impossible to determine what functions are called and what data do they reference.

Building
--------
The code compiles on GNU GCC9, and should work with the later versions, too.  No efforts whatsover has been made to support Clang or other popular compilers.  Simply install a vanilla GCC and you are good to go.  Sources are compiled and then linked using a supplied GNU Make makefile.

The code should compile on any platform, but again, this has been only tested on MSYS2 running on a modern MS-Windows.  One must enable symlink support for MSYS, or otherwise the build will fail.

To build the tool, simply say
```
make
```
and there should be an executable called `C815.EXE` compiled.  Intermediate objects are placed inside a staging directory called `STAGE`.  You may delete that directory.  Also, if you change any of the compiler flags you must also manually clean this directory;  No, there's no `clean` target supplied.

Usage
-----
Execute the freshly built (or a prebuilt) executable
```
C815.EXE <GRAPH.DAT>
```
and if the given `GRAPH.DAT` file is found (an absolute or a relative path may be prefixed to the file)  all assets will be dumped inside a directory called `DUMP`.  Sounds are stored as `.WAV` files (8-bit PCM) and graphical assets as `.BMP` files (uncompressed, 256-colors) so they do open basically with any audio or image processing tool.
