#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

/*
 * audioio.h
 *
 *  Created on: Nov 5, 2013
 *      Author: plamot
 */

#ifndef AUDIOIO_H_
#define AUDIOIO_H_


//structAudioContext>
typedef struct AudioContext
{
	/////structures d'opensl
	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;

	// output mix interfaces
	SLObjectItf outputMixObject;

	// buffer queue player interfaces
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

	///configuration pour opensl
	int samplerate;
	int buffersize;

	////buffer contenant nos samples de sortie
	int16_t* audioBuffer;

	///paramètre pour notre oscillateur
	int note;
	uint16_t oscpos;


} AudioContext;
//structAudioContext<

void audiocontext_release(AudioContext* ctx);

AudioContext* audiocontext_create(int samplerate, int buffersize);


#endif /* AUDIOIO_H_ */
