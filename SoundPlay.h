//
// Created by huckiyang on 4/29/18.
//

#ifndef INC_6122SNAKEBEE_PLAYSOUND_H
#define INC_6122SNAKEBEE_PLAYSOUND_H


/*
 * SoundPlay.h
 *  1: Download [openal] Pack (Software implementation of the Snakebee API(devlopment files)) and [alut] Pack
 *  2：Add header path as：/usr/include/AL
 *  3: Add Source：openal and alut
 */

#include <al.h>
#include <alc.h>
#include <alut.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define numBuffer 4   //Play 4 Song in one time
#define numSource 4   //Play 4 Song in one time

using namespace std;
class SoundPlay {
public:
    SoundPlay();
    virtual ~SoundPlay();
    void PlaySound(string fileName,int index,float volume);//Wheb index=0：Cycle;index!=0: Once
    void SetVolume(int index,float volume);
    void ReleaseSound(int index);
    void MusicPause(int index);
    void MusicContinue(int index);
    bool CheckPlayState(int i);
private:
    void PlayLongSound(const char* fileName,int index);
    void OpenDevice();
    void CloseDevice();
    string GetALCErrorString(ALenum err);
private:
    ALuint buffers[numBuffer];
    ALuint sources[numSource];
    ALCcontext* cc;
    ALCdevice* dev;
    bool checkstate;
    ALenum result;
};


#endif //INC_6122SNAKEBEE_PLAYSOUND_H
