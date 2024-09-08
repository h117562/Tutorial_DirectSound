#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#pragma comment(lib, "dsound.lib")

#include <dsound.h>
#include <directxmath.h>
#include <windows.h>
#include <mmsystem.h>
#include <fstream>

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND, const char*);
	bool PlayAudio();
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	bool LoadSoundFile(const char*);

private:
	LPDIRECTSOUND8 m_directSound;
	LPDIRECTSOUNDBUFFER  m_primaryBuffer;
	LPDIRECTSOUNDBUFFER m_secondaryBuffer;

	unsigned char* m_waveData;

};

#endif