#include <Windows.h>    
#include <Kinect.h>  
#include <iostream>    
#include<ctime>    
#include<cassert>    
#include<process.h>    

#pragma comment(lib,"kinect20.lib")  

using namespace std;

#define _DDDEBUG  


template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}



