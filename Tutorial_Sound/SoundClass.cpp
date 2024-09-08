#include "SoundClass.h"

SoundClass::SoundClass()
{
	m_directSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer = 0;
	m_waveData = 0;
}

SoundClass::SoundClass(const SoundClass& other)
{
}

SoundClass::~SoundClass()
{
}

bool SoundClass::Initialize(HWND hwnd, const char* filePath)
{
	bool result;


	//DirectSound 초기화
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	//오디오 파일 읽기
	result = LoadSoundFile(filePath);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;

	//DSound 생성
	result = DirectSoundCreate8(NULL, &m_directSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//협력 수준 세팅
	result = m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	//primary buffer 초기화
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundClass::LoadSoundFile(const char* filePath)
{
	HRESULT result;

	unsigned long dataSize, bufferSize;
	unsigned char* bufferPtr = 0;

	//바이너리로 읽기
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	//파일 전체의 크기를 가져옴
	file.seekg(0, std::ios::end);
	dataSize = file.tellg();
	m_waveData = new unsigned char[dataSize];

	//wav 파일 데이터 저장
	file.seekg(0, std::ios::beg);
	file.read((char*)m_waveData, dataSize);

	//Wav 포맷 설정
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;//원래는
	waveFormat.wBitsPerSample = 16;//파일 데이터에서
	waveFormat.nChannels = 2;//직접 읽어야 함
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//secondary buffer 초기화
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_directSound->CreateSoundBuffer(&bufferDesc, &m_secondaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//메모리 직접 접근
	result = m_secondaryBuffer->Lock(0, dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//복사
	memcpy(bufferPtr, m_waveData, dataSize);

	//락 해제
	result = m_secondaryBuffer->Unlock(bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundClass::PlayAudio()
{
	HRESULT result;

	result = m_secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	result = m_secondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
	{
		return false;
	}
	
	return true;
}

void SoundClass::Shutdown()
{
	if (m_waveData)
	{
		delete[] m_waveData;
		m_waveData = 0;
	}

	if (m_secondaryBuffer)
	{
		m_secondaryBuffer->Release();
		m_secondaryBuffer = 0;
	}

	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	if (m_directSound)
	{
		m_directSound->Release();
		m_directSound = 0;
	}

	return;
}