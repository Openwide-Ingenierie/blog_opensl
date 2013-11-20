#include <stddef.h>
#include <stdlib.h>
#include <android/log.h>
#include <math.h>
#include "audioio.h"


#define APPTAG "blogsles"

//playerCallback>
//conversion de la note sur l'echelle MIDI vers sa frequence associée
double miditofreq(int note)
{
	return pow(2, (note-69)/12.) * 440.;
}

void playerCallback(SLAndroidSimpleBufferQueueItf bq, void* opaque)
{
	AudioContext* ctx = (AudioContext*)opaque;

	int i;
	//si il n'y plus de note, nous insérons du silence
	if (ctx->note == 0)
	{
		memset(ctx->audioBuffer, 0, ctx->buffersize * sizeof(int16_t));
	}
	//sinon nous générons une sinusoide de la fréquence de la note
	else
	{

		double freq = miditofreq(ctx->note);
		double scale =  (2 * M_PI * freq) / ctx->samplerate;

		int t = ctx->oscpos;
		for (i = 0; i < ctx->buffersize; i++)
		{

			ctx->audioBuffer[i] =  (sin(t++ * scale) * 5000);
		}
		//nous sauvegardons la position du générateur afin d'eviter d'avoir une
		//discontinuité entre les buffers
		ctx->oscpos = t;
	}
	//mise en queue du buffer audio
	(*bq)->Enqueue(bq, ctx->audioBuffer, ctx->buffersize * sizeof(int16_t));
}
//playerCallback<

//engineinit>
SLresult engineInit(AudioContext *ctx)
{
	SLresult result;
	// create engine
	result = slCreateEngine(&(ctx->engineObject), 0, NULL, 0, NULL, NULL);
	if(result != SL_RESULT_SUCCESS)
		return result;

	// realize the engine
	result = (*ctx->engineObject)->Realize(ctx->engineObject, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		return result;

	// get the engine interface, which is needed in order to create other objects
	result = (*ctx->engineObject)->GetInterface(
		ctx->engineObject,
		SL_IID_ENGINE,
		&(ctx->engineEngine));

	if(result != SL_RESULT_SUCCESS)
		return result;

	return result;
}
//engineinit<


//mixerinit>
SLresult mixerInit(AudioContext *ctx)
{
	SLresult result;

	//nombre d'interface à créer
	const SLuint32 nbInterface = 1;
	//tableau de interface à créer, ici une seule interface de type
	//volume
	const SLInterfaceID ids[] = {SL_IID_VOLUME};
	//tableau indique que nous n'avons par besoin de récupérer
	//l'interface de l'objet
	const SLboolean req[] = {SL_BOOLEAN_FALSE};
	result = (*ctx->engineEngine)->CreateOutputMix(
		ctx->engineEngine,
		&(ctx->outputMixObject),
		nbInterface,
		ids,
		req);
	if(result != SL_RESULT_SUCCESS)
		return result;

	result = (*ctx->outputMixObject)->Realize(ctx->outputMixObject, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		return result;

	return result;
}
//mixerinit<


//convertSamplerate>
SLuint32 convertSamplerate(int samplerate)
{
	switch(samplerate) {
	case 8000:
		return SL_SAMPLINGRATE_8;
		break;
	case 11025:
		return SL_SAMPLINGRATE_11_025;
		break;
	case 16000:
		return SL_SAMPLINGRATE_16;
		break;
	case 22050:
		return SL_SAMPLINGRATE_22_05;
		break;
	case 24000:
		return SL_SAMPLINGRATE_24;
		break;
	case 32000:
		return SL_SAMPLINGRATE_32;
		break;
	case 44100:
		return SL_SAMPLINGRATE_44_1;
		break;
	case 48000:
		return SL_SAMPLINGRATE_48;
		break;
	case 64000:
		return SL_SAMPLINGRATE_64;
		break;
	case 88200:
		return SL_SAMPLINGRATE_88_2;
		break;
	case 96000:
		return SL_SAMPLINGRATE_96;
		break;
	case 192000:
		return SL_SAMPLINGRATE_192;
		break;
	default:
		return -1;
	}
}
//convertSamplerate<

SLresult sinkInit(AudioContext *ctx)
{
	SLresult result;

//srcinit>
	//conversion du samplerate désiré vers la constante opensl associée
	SLuint32 sr = convertSamplerate(ctx->samplerate);

	//definition du format de donnée
	SLDataFormat_PCM format_pcm = {
		SL_DATAFORMAT_PCM, // les données seront au format PCM
		1,  //nombre de canaux audio
		sr, //frequence d'echantillonnage des données
		SL_PCMSAMPLEFORMAT_FIXED_16,  //nos données seront au format 16bits
		SL_PCMSAMPLEFORMAT_FIXED_16, //et contenue dans des mots de 16bits
		SL_SPEAKER_FRONT_CENTER, //cannaux de sortie
		SL_BYTEORDER_LITTLEENDIAN //endianness des données
	};

	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
		SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, //constante à utiliser pour Android.
		1//nombre de buffer dans la queue
	};

	//définition de la source audio
	SLDataSource audioSrc = {
		&loc_bufq, //les données proviennent de la queue opensl
		&format_pcm //et elles sont au format définie précedement
	};
//srcinit<

//sinkinit>
	//reference notre sortie audio
	SLDataLocator_OutputMix loc_outmix = {
		SL_DATALOCATOR_OUTPUTMIX,
		ctx->outputMixObject
	};

	//on va diriger la sortie de notre objet vers la sortie audio
	SLDataSink audioSnk = {
		&loc_outmix,
		NULL
	};
//sinkinit<

//playerinit>
	//nous pouvons enfin creer notre lecteur
	const SLInterfaceID ids1[] = {SL_IID_VOLUME, SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
	const SLboolean req1[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	result = (*ctx->engineEngine)->CreateAudioPlayer(
		ctx->engineEngine, //notre moteur opensl
		&(ctx->bqPlayerObject),
		&audioSrc, //source audio
		&audioSnk, //destination audio
		2, //nombre d'interfaces
		ids1, //type d'interface
		req1 //recupération des interfaces
		);
	if(result != SL_RESULT_SUCCESS)
		return result;

	result = (*ctx->bqPlayerObject)->Realize(ctx->bqPlayerObject, SL_BOOLEAN_FALSE);
	if(result != SL_RESULT_SUCCESS)
		return result;

	//récupération de d'interface du lecteur
	result = (*ctx->bqPlayerObject)->GetInterface(
		ctx->bqPlayerObject,
		SL_IID_PLAY,
		&(ctx->bqPlayerPlay)
		);
	if(result != SL_RESULT_SUCCESS)
		return result;

	//récupération de l'interface vers les queues du lecteur
	result = (*ctx->bqPlayerObject)->GetInterface(
		ctx->bqPlayerObject,
		SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
		&(ctx->bqPlayerBufferQueue)
		);
	if(result != SL_RESULT_SUCCESS)
		return result;

//playerinit<

	return result;

}



//setupcallbacks>
SLresult setupPlayerCallback(AudioContext* ctx)
{
	SLresult result;

	// register callback on the buffer queue
	result = (*ctx->bqPlayerBufferQueue)->RegisterCallback(ctx->bqPlayerBufferQueue, playerCallback, ctx);
	if(result != SL_RESULT_SUCCESS) return result;

	// set the player's state to playing
	result = (*ctx->bqPlayerPlay)->SetPlayState(ctx->bqPlayerPlay, SL_PLAYSTATE_PLAYING);

	//mise en queue d'un premier buffer
	(*ctx->bqPlayerBufferQueue)->Enqueue(
		ctx->bqPlayerBufferQueue,
		ctx->audioBuffer,
		ctx->buffersize*sizeof(int16_t));

	return result;
}
//setupcallbacks<

//audiocontext_release>
void audiocontext_release(AudioContext* ctx)
{
	//libération du player
	if (ctx->bqPlayerObject != NULL)
	{
		SLuint32 state = SL_PLAYSTATE_PLAYING;
		(*ctx->bqPlayerPlay)->SetPlayState(ctx->bqPlayerPlay, SL_PLAYSTATE_STOPPED);
		while (state != SL_PLAYSTATE_STOPPED)
		{
			(*ctx->bqPlayerPlay)->GetPlayState(ctx->bqPlayerPlay, &state);
		}
		(*ctx->bqPlayerObject)->Destroy(ctx->bqPlayerObject);
		ctx->bqPlayerObject = NULL;
		ctx->bqPlayerPlay = NULL;
		ctx->bqPlayerBufferQueue = NULL;
	}

	//libération de l'interface de sortie
	if (ctx->outputMixObject != NULL)
	{
		(*ctx->outputMixObject)->Destroy(ctx->outputMixObject);
		ctx->outputMixObject = NULL;
	}

	//libération du moteur OpenSL
	if (ctx->engineObject != NULL) {
		(*ctx->engineObject)->Destroy(ctx->engineObject);
		ctx->engineObject = NULL;
		ctx->engineEngine = NULL;
	}

	//libération du buffer audio
	free(ctx->audioBuffer);
	free(ctx);
}
//audiocontext_release<



//createaudiocontext>
AudioContext* audiocontext_create(int samplerate, int buffersize)
{
	AudioContext* ctx = (AudioContext*)malloc(sizeof(AudioContext));
	memset(ctx, 0, sizeof(AudioContext));

	ctx->audioBuffer = calloc(buffersize, sizeof(int16_t));
	ctx->buffersize = buffersize;
	ctx->samplerate= samplerate;

	__android_log_print(ANDROID_LOG_WARN, APPTAG, "samplerate %i buffersize %i", samplerate, buffersize);

	SLresult result;

	result = engineInit(ctx);
	if(result != SL_RESULT_SUCCESS)
		goto error;

	result = mixerInit(ctx);
	if(result != SL_RESULT_SUCCESS)
		goto error;

	result = sinkInit(ctx);
	if(result != SL_RESULT_SUCCESS)
		goto error;

	/*result = playerInit(ctx);
	if(result != SL_RESULT_SUCCESS)
		goto error;
*/
	result = setupPlayerCallback(ctx);
	if(result != SL_RESULT_SUCCESS)
		goto error;


	__android_log_print(ANDROID_LOG_WARN, APPTAG, "initialisation OK");

	return ctx;

error:
	audiocontext_release(ctx);
	return NULL;
}
//createaudiocontext<

