#include <Kinect.h>  
#include <iostream>
#include <math.h>       
#include "KinectGestureRecognition.h"

static int firstStateTime = 0;
static int secondStateTime = 0;

//检测右手是否展开
bool RightHandOpen(IBody* pSingleBody)
{
	HandState rightHandState = HandState_Unknown;
	//获取手部状态
	pSingleBody->get_HandRightState(&rightHandState);
	if (HandState_Open == rightHandState)
	{
		//cout << "右手握拳\n";
		return true;
	}
	else
	{
		//cout << "右手张开\n";
		return false;
	}
}

//右手挥动
bool WaveRightHand(IBody* pSingleBody)
{
	const double ArmShrink = 0.2;//手臂收缩的距离
	Joint joints[JointType_Count];
	pSingleBody->GetJoints(JointType::JointType_Count, joints);
	float handRightX = joints[JointType_HandRight].Position.X;
	float spineMidX = joints[JointType::JointType_SpineBase].Position.X;
	float distanceDifference = handRightX - spineMidX;
	if (RightHandOpen(pSingleBody) && distanceDifference > 0.3)
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		firstStateTime = st.wMinute * 60 + st.wSecond;
	}
	if (RightHandOpen(pSingleBody) && distanceDifference < 0.04)
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		secondStateTime = st.wMinute * 60 + st.wSecond;
	}

	if (secondStateTime - firstStateTime < 2 && secondStateTime - firstStateTime > 0)
	{
		firstStateTime = 0;
		secondStateTime = 0;
		return true;
	}
	else
	{
		return false;
	}
}
void KinectGestureRecognition(void)
{
	IKinectSensor *kinect = NULL;
	HRESULT hr = S_OK;
	hr = GetDefaultKinectSensor(&kinect);  //  得到默认的设备  

	if (FAILED(hr) || kinect == NULL)
	{
		cout << "创建 sensor 失败\n";
		return;
	}
	if (kinect->Open() != S_OK) // 是否打开成功  
	{
		cout << "Kinect sensor 没准备好\n";
		return;
	}
	IBodyFrameSource *bady = nullptr;  // 获取源  
	hr = kinect->get_BodyFrameSource(&bady);

	IBodyFrameReader *pBodyReader;

	hr = bady->OpenReader(&pBodyReader); // 打开获取骨骼信息的  Reader  
	if (FAILED(hr))
	{
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return;
	}

	std::cout << "游戏即将开始，你有3秒钟将鼠标放到游戏窗口\n";
	Sleep(3000);
	std::cout << "游戏开始\n";
	while (true)
	{
		IBodyFrame* pBodyFrame = nullptr;
		hr = pBodyReader->AcquireLatestFrame(&pBodyFrame);

		if (SUCCEEDED(hr))
		{
			IBody* pBody[BODY_COUNT] = { 0 }; // 默认的是 6 个骨骼 ，初始化所有的骨骼信息  
			//更新骨骼数据    
			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody); // 刷新骨骼信息（6个）  
			if (SUCCEEDED(hr))
			{
				BOOLEAN bTracked = false;

				for (int i = 0; i < BODY_COUNT; i++)
				{
					hr = pBody[i]->get_IsTracked(&bTracked); // 检查是否被追踪  
				}
			}
			for (int i = 0; i < BODY_COUNT; i++)
			{
				IBody* pSingleBody = pBody[i];

				if (pSingleBody)
				{
					Joint joints[JointType_Count];
					//获取骨骼信息节点  
					pSingleBody->GetJoints(JointType::JointType_Count, joints);
					//右握拳伸展为右转
					if (WaveRightHand(pSingleBody))
					{
						cout << "挥手完成" << endl;
					}

				}

			}//for
			for (int count = 0; count < BODY_COUNT; count++)
			{
				SafeRelease(pBody[count]);
			}
			SafeRelease(pBodyFrame);  // 别忘了释放 
		}//if	

	}
	kinect->Close();  // 优雅的关闭kinect
	cout << "kinect关闭\n";
	return;
}
int main()
{
	KinectGestureRecognition();

	system("pause");
	return 0;
}
