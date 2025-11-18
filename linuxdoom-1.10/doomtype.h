// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Simple basic typedefs, isolated here to make it easier
//	 separating modules.
//    
//-----------------------------------------------------------------------------


#ifndef __DOOMTYPE__
#define __DOOMTYPE__


#ifndef __BYTEBOOL__
#define __BYTEBOOL__
typedef enum {False, True} boolean;
typedef unsigned char byte;
#endif

#ifndef __PTRINT__
#define __PTRINT__

#if defined(__x86_64__) || defined(__ppc64__) || defined(__arm64__) || defined(__wasm64__)

#include <stdint.h>
typedef intptr_t ptrint;
typedef uintptr_t uptrint;

#else

// useful, tells it how to compile
#define ENV32
typedef int ptrint;
typedef unsigned int uptrint;

#endif

#endif


// Predefined with some OS.
#if defined(LINUX) && !defined(__APPLE__)
#include <values.h>
#else
#define MAXCHAR		((char)0x7f)
#define MAXSHORT	((short)0x7fff)

// Max pos 32-bit int.
#define MAXINT		((int)0x7fffffff)
#ifdef ENV32
#define MAXLONG		((long)0x7fffffff)
#else
#define MAXLONG		MAXINT
#endif

#define MINCHAR		((char)0x80)
#define MINSHORT	((short)0x8000)

// Max negative 32-bit integer.
#define MININT		((int)0x80000000)
#ifdef ENV32
#define MINLONG		((long)0x80000000)
#else
#define MINLONG		MININT
#endif

#endif




#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
