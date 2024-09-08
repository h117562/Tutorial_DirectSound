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


	//DirectSound �ʱ�ȭ
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	//����� ���� �б�
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

	//DSound ����
	result = DirectSoundCreate8(NULL, &m_directSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���� ����
	result = m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	//primary buffer �ʱ�ȭ
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

	//���̳ʸ��� �б�
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return false;
	}

	//���� ��ü�� ũ�⸦ ������
	file.seekg(0, std::ios::end);
	dataSize = file.tellg();
	m_waveData = new unsigned char[dataSize];

	//wav ���� ������ ����
	file.seekg(0, std::ios::beg);
	file.read((char*)m_waveData, dataSize);

	//Wav ���� ����
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;//������
	waveFormat.wBitsPerSample = 16;//���� �����Ϳ���
	waveFormat.nChannels = 2;//���� �о�� ��
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//secondary buffer �ʱ�ȭ
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

	//�޸� ���� ����
	result = m_secondaryBuffer->Lock(0, dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	//����
	memcpy(bufferPtr, m_waveData, dataSize);

	//�� ����
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