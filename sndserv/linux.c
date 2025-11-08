// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: linux.c,v 1.3 1997/01/26 07:45:01 b1 Exp $
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
//
// $Log: linux.c,v $
// Revision 1.3  1997/01/26 07:45:01  b1
// 2nd formatting run, fixed a few warnings as well.
//
// Revision 1.2  1997/01/21 19:00:01  b1
// First formatting run:
//  using Emacs cc-mode.el indentation for C++ now.
//
// Revision 1.1  1997/01/19 17:22:45  b1
// Initial check in DOOM sources as of Jan. 10th, 1997
//
//
// DESCRIPTION:
//	UNIX, soundserver for Linux i386.
//
//-----------------------------------------------------------------------------

//static const char rcsid[] = "$Id: linux.c,v 1.3 1997/01/26 07:45:01 b1 Exp $";


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#ifdef USE_OSS
#include <linux/soundcard.h>
#else
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#endif

#include "soundsrv.h"

int            audio_fd;
pa_simple*     audio_s;
pa_sample_spec audio_ss;
void
myioctl
( int	fd,
  int	command,
  int*	arg )
{   
    int		rc;
    extern int	errno;
    
    rc = ioctl(fd, command, arg);  
    if (rc < 0)
    {
	fprintf(stderr, "ioctl(dsp,%d,arg) failed\n", command);
	fprintf(stderr, "errno=%d\n", errno);
	exit(-1);
    }
}

void I_InitMusic(void)
{
}

void
I_InitSound
( int	samplerate,
  int	samplesize )
{

    int i;


#ifdef USE_OSS
    audio_fd = open("/dev/dsp", O_WRONLY);
    if (audio_fd < 0)
        fprintf(stderr, "Could not open /dev/dsp\n");
         
                     
    i = (2 << 16) | 11; // max 2 fragments,  11 = fragment size of 2048byte
    myioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &i);
                    
    myioctl(audio_fd, SNDCTL_DSP_RESET, 0);

    i = samplerate; // meant to be 11025 doesn't matter as it only really passes in 11025 anyway
    myioctl(audio_fd, SNDCTL_DSP_SPEED, &i);

#ifdef LEGACY_OSS
    i = 1;
    myioctl(audio_fd, SNDCTL_DSP_STEREO, &i);
#else
    i = 2;
    myioctl(audio_fd, SNDCTL_DSP_CHANNELS, &i);
#endif
    myioctl(audio_fd, SNDCTL_DSP_GETFMTS, &i);
    if (i &= AFMT_S16_LE)    
        myioctl(audio_fd, SNDCTL_DSP_SETFMT, &i);
    else
        fprintf(stderr, "Cannot play signed 16 data!\n");
#else

    audio_ss.format = PA_SAMPLE_S16LE;
    audio_ss.channels = 2;
    audio_ss.rate = samplerate;

    audio_s = pa_simple_new(NULL,         // Use the default server.
                      "ROOM",             // Our application's name.
                      PA_STREAM_PLAYBACK,
                      NULL,               // Use the default device.
                      "Music",            // Description of our stream.
                      &audio_ss,          // Our sample format.
                      NULL,               // Use default channel map
                      NULL,               // Use default buffering attributes.
                      NULL                // Ignore error code.
    );
#endif
}

void
I_SubmitOutputBuffer
( void*	samples,
  int	samplecount )
{
#ifdef USE_OSS
    write(audio_fd, samples, samplecount*4);
#else
    pa_simple_write(audio_s, samples, samplecount * 4, NULL);
#endif
}

void I_ShutdownSound(void)
{
#ifdef USE_OSS
    close(audio_fd);
#else
    pa_simple_free(audio_s);
#endif
}

void I_ShutdownMusic(void)
{
}
