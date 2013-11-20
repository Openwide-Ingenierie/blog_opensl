#include <unistd.h>
#include <jni.h>
#include "audioio.h"


JNIEXPORT jlong JNICALL Java_fr_openwide_blogsles_MainActivity_audioStart(JNIEnv * env, jobject obj, jint samplerate, jint buffersize)
{
	AudioContext* ctx =  audiocontext_create(samplerate, buffersize);
	return (jlong)ctx;
}

JNIEXPORT jboolean JNICALL Java_fr_openwide_blogsles_MainActivity_audioStop(JNIEnv * env, jobject obj, jlong instance)
{
	AudioContext* ctx = (AudioContext*)instance;
	audiocontext_release(ctx);
	return 1;
}

JNIEXPORT jboolean JNICALL Java_fr_openwide_blogsles_MainActivity_audioNoteOn(JNIEnv * env, jobject obj, jlong instance, jint tone)
{
	AudioContext* ctx = (AudioContext*)instance;
	ctx->note = tone;
	return 1;
}

JNIEXPORT jboolean JNICALL Java_fr_openwide_blogsles_MainActivity_audioNoteOff(JNIEnv * env, jobject obj, jlong instance)
{
	AudioContext* ctx = (AudioContext*)instance;
	ctx->note = 0;
	return 1;
}

