# ROOM
### _Ready DOOM_

> [!CAUTION]
> The main DOOM codebase, in it's current state, is partially untested.
>
> Due to macOS limitations and me just generally
> being scared of C memory problems, I currently
> only test it under Emscripten.

ROOM is a Linuxdoom fork that aims to fix bugs,
while mainly keeping the structure of the code
the same, helping experienced modders be familliar.




**TODOs**:

- sndserver on emscripten

**Changes**:

- SDL3 support (ONLY bleeding edge builds, unfortunately some functions I use are not yet in stable)
- supressed/fixed most warnings
- PulseAudio sndserver with optional OSS (`-DUSE_OSS` in `CFLAGS`)
- misc sndserver improvements
- add working macOS support for soundserver
