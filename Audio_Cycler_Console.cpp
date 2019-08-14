// Audio_Cycler_Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <math.h>
#include <ctime>

//Windows headers
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioendpoints.h>

#define BASE_VOLUME 0.5		//To avoid the need to do sign checks on the value of the sine function

//useful but ultimately unecessary function 
template <typename T> int signof(T val)
{
	return (T(0) < val) - (val < T(0));
}

int main(int argc, char** argv)
{
	//This is necessary because many of the below objects are COM objects
	CoInitialize(NULL);

    //first step is to create a device enumerator
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	IMMDeviceEnumerator *pEnumerator;
	IMMDevice* defaultDevice;

	HRESULT hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator,
		NULL,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		(void**)& pEnumerator
	);

	//Now we get a reference to the system's default speakers (output device)
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	pEnumerator->Release();
	pEnumerator = NULL;
	
	//get the Volume 
	IAudioEndpointVolume* endVol = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)& endVol);
	defaultDevice->Release();
	defaultDevice = NULL;

	//The value we want to set the volume to
	float currentVol = 0;

	//here for documentation and memory purposes
	//hr = endVol->GetMasterVolumeLevelScalar(&currentVol);
	//hr = endVol->SetMasterVolumeLevelScalar(0.5, NULL);

	//sine contraints
	float amp = 0.5;
	float freq = 3.0;		//approximately #cycles/6s
	float val = 0.0;		//the amount to which the volume will be changed, 0.0 to 1.0 max values

	//once the volume goes past 20 or so on most setups its pretty indistinguishable from 0 
	//(must be specified as a decimal percentage)
	//only accepting min volume because it simplifies the math and makes testing easier with only one moving goal
	float min_vol =0.40;

	//min_vol will always be divided by 2, so might as well do that just once
	min_vol /= 2;

	//must adjust the amp for any min_vol so that the whole waveform is not imbalanced on one side of BASE_VOLUME
	amp =0.5-min_vol;



	time_t start = clock();
	float fCLOCKS_PER_SEC = (float)CLOCKS_PER_SEC;
	std::cout << "Equation: " << amp << "sin("<<freq<<"(x))+" << BASE_VOLUME+min_vol<< std::endl;
	while (1)
	{
		//now wrapped in an absolute value call bc theres no reason to go through a cycle of negative (0) values
		val = BASE_VOLUME+(amp * sin(freq * ( (clock()-start) / fCLOCKS_PER_SEC ) ) ) + min_vol  ;
		//std::cout << val << std::endl;
		hr = endVol->SetMasterVolumeLevelScalar(val, NULL);
	}

	//Signal that we are done with COM objects
	CoUninitialize();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu


