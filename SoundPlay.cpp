//
// Created by huckiyang on 4/30/18.
//

/*
 *  SoundPlay.cpp
 */
#include "SoundPlay.h"

SoundPlay::SoundPlay()
{
    alutInit(NULL,0);
    if ((result=alGetError()) != AL_NO_ERROR)
        cout<<"alutInit--"<<result<<endl;
    OpenDevice();
    //PlaySound("2HellUGA.wav",0,100);
}
SoundPlay::~SoundPlay()
{
    CloseDevice();
}
void SoundPlay::OpenDevice()
{
    ALCchar DeviceName[] = "ALSA Software";//ALSA Software,DirectSound3D
    dev=alcOpenDevice(DeviceName);
    cc=alcCreateContext(dev,NULL);
    alcMakeContextCurrent(cc);
}
void SoundPlay::CloseDevice()
{
    ALCcontext* context = alcGetCurrentContext();
    ALCdevice* device = alcGetContextsDevice( context );
    alcMakeContextCurrent( NULL );
    alcDestroyContext( context );
    alcCloseDevice( device );
}
void SoundPlay::ReleaseSound(int index)
{
    alDeleteSources(1,&sources[index]);
    alDeleteBuffers(1,&buffers[index]);
}
void SoundPlay::PlayLongSound(const char* fileName,int index)
{
    alGenSources(1,&sources[index]);
    alGenBuffers(1,&buffers[index]);
    ALvoid *data;
    ALsizei size=0,freq=0;
    ALenum format;
    ALboolean loop;
    alutLoadWAVFile((ALbyte *)fileName,&format,&data,&size,&freq,&loop);
    alBufferData(buffers[index],format,data,size,freq);
    if ((result=alGetError()) != AL_NO_ERROR)
        cout<<"PlayLongSound alBufferData errno"<<result<<endl;
    alutUnloadWAV(format,data,size,freq);
    alSourcei(sources[index],AL_BUFFER,buffers[index]);// Buffer
    if(index==0)
        alSourcei(sources[index],AL_LOOPING,AL_TRUE);
    alSourcePlay(sources[index]);
}
void SoundPlay::PlaySound(string fileName,int index,float volume)
{
    alGenSources(1,&sources[index]);
    alGenBuffers(1,&buffers[index]);
    ReleaseSound(index);
    PlayLongSound(fileName.data(),index);
    alSourcef(sources[index],AL_GAIN,volume);
}
void SoundPlay::SetVolume(int index,float volume)//volume in range (0, 1)
{
    alSourcef(sources[index],AL_GAIN,volume);
}
void SoundPlay::MusicPause(int index)
{
    alSourcePause(sources[index]);
    alSourceStop(sources[index]);
}
void SoundPlay::MusicContinue(int index)
{
    alSourcePlay(sources[index]);
}
bool SoundPlay::CheckPlayState(int i)
{
    ALint state;
    alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING)
    {
        checkstate=false;
        return true;
    }
    return false;
}
int main()
{
    SoundPlay sp;
    sp.PlaySound("/2HellUGA.wav",0,100);
    while(1){}
    return 0;
}