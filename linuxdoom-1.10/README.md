## Compiling
Run
```
gcc *.c -Wall -Wextra -Wpedantic -Wno-unused -Wno-format-truncation -Wno-implicit-fallthrough -DUSE_SDL -lSDL3 -m32 -DNORMALUNIX -DLINUX -lm -o doom
```

or targetting Emscripten
```
emcc *.c -m32 -g -DNORMALUNIX -DLINUX -DUSE_SDL --embed-file path/to/doom1.wad@doom1.wad -L/path/to/SDL/lib -I/path/to/SDL/include -lSDL3 -lm -o doom.html
```

