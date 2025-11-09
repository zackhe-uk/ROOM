# ROOM
### _Ready DOOM_

> [!CAUTION]
> The codebase, in it's current state, is COMPLETELY untested.
>
> Due to macOS/Linux limitations and deprecations, it will not
> be tested until I finish moving the video handling Linuxdoom
> code to SDL2 or SDL3.
>
> I've already done that a while back [here](https://github.com/doom-em/sdl_doom_em/blob/master/i_sdl_video.c) based on [this](https://github.com/aserebryakov/sdl_doom)
> but that was pretty shitty and it's probably due for a rewrite.

ROOM is a Linuxdoom fork that aims to fix bugs,
while mainly keeping the structure of the code
the same, helping experienced modders be familliar.




**TODOs**:

- move from X11 to SDL2 for display


**Changes**:

- supressed/fixed most warnings
- PulseAudio sndserver with optional OSS (`-DUSE_OSS` in `CFLAGS`)
- misc sndserver improvements
