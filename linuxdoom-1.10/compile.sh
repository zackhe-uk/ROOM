#!/bin/bash

if [ "$1" = "--help"  ] || [ "$1" = "-help" ] || [ "$1" = "-h" ]
then
    printf "%s\n" "Usage: $0 [-help/-lib/-dnet/clean]"
    printf "%s\n%s\n%s\n%s\n%s\n" \
	"  -help: shows this message" \
	"  -debug: compiles a debug build" \
	"  -8/-4: forces what arch to compile for" \
	"  -em: build for emscripten " \
    "   clean: clean up the build dir" \
    exit 0
fi

if [ "$1" = "clean" ]
then
    rm -rf bin
    exit 0
fi

# at this point we can safely make the binary out dir assuming a compilation is taking place
mkdir -p bin

echo "${CC:=gcc}" > /dev/null
echo "${EXOUT:=bin/doom}" > /dev/null

# brand new things
for var in "$@"
do
    if [[ "$var" == "-debug" ]]
    then
        PREXTRAOPT="-g $PREXTRAOPT"
    fi

    if [[ "$var" == "-8" ]]
    then
        PREXTRAOPT="$PREXTRAOPT -m64"

    elif [[ "$var" == "-4" ]]
    then
        PREXTRAOPT="$PREXTRAOPT -m32"
    fi

    if [[ "$var" == "-em" ]]
    then
        if [ ! -f ./doom1.wad ]; then ../getdoom.sh; fi
        if [[ "$PREXTRAOPT" == *"-m64"* ]]
        then
            PREXTRAOPT="$PREXTRAOPT -sMEMORY64 -Wl,-mwasm64"
        fi
        EXTRAOPT="$EXTRAOPT --embed-file ./doom1.wad@doom1.wad --shell-file ./shell.html"
        CC=emcc
    fi

    if [[ "$var" == "-mcem" ]]
    then
        if [[ "$PREXTRAOPT" == *"-m64"* ]]
        then
            EXTRAOPT="$EXTRAOPT -L../../SDL/build"
            EXOUT="bin/doom.html"
        else
            EXTRAOPT="$EXTRAOPT -L../../SDL32/build"
            EXOUT="bin/doom32.html"
        fi
        EXTRAOPT="$EXTRAOPT -I../../SDL/include"
    fi
done

$CC *.c $PREXTRAOPT -DNORMALUNIX -DLINUX -DUSE_SDL $EXTRAOPT -lSDL3 -lm -o "$EXOUT"