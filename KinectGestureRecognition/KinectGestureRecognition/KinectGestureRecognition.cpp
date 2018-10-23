#include <Kinect.h>  
#include <iostream>
#include <math.h>       
#include "KinectGestureRecognition.h"

static int firstStateTime = 0;
static int secondStateTime = 0;

//��������Ƿ�չ��
bool RightHandOpen(IBody* pSingleBody)
{
	HandState rightHandState = HandState_Unknown;
	//��ȡ�ֲ�״̬
	pSingleBody->get_HandRightState(&rightHandState);
	if (HandState_Open == rightHandState)
	{
		//cout << "������ȭ\n";
		return true;
	}
	else
	{
		//cout << "�����ſ�\n";
		return false;
	}
}

//���ֻӶ�
bool WaveRightHand(IBody* pSingleBody)
{
	const double ArmShrink = 0.2;//�ֱ������ľ���
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
	hr = GetDefaultKinectSensor(&kinect);  //  �õ�Ĭ�ϵ��豸  

	if (FAILED(hr) || kinect == NULL)
	{
		cout << "���� sensor ʧ��\n";
		return;
	}
	if (kinect->Open() != S_OK) // �Ƿ�򿪳ɹ�  
	{
		cout << "Kinect sensor û׼����\n";
		return;
	}
	IBodyFrameSource *bady = nullptr;  // ��ȡԴ  
	hr = kinect->get_BodyFrameSource(&bady);

	IBodyFrameReader *pBodyReader;

	hr = bady->OpenReader(&pBodyReader); // �򿪻�ȡ������Ϣ��  Reader  
	if (FAILED(hr))
	{
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return;
	}

	std::cout << "��Ϸ������ʼ������3���ӽ����ŵ���Ϸ����\n";
	Sleep(3000);
	std::cout << "��Ϸ��ʼ\n";
	while (true)
	{
		IBodyFrame* pBodyFrame = nullptr;
		hr = pBodyReader->AcquireLatestFrame(&pBodyFrame);

		if (SUCCEEDED(hr))
		{
			IBody* pBody[BODY_COUNT] = { 0 }; // Ĭ�ϵ��� 6 ������ ����ʼ�����еĹ�����Ϣ  
			//���¹�������    
			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody); // ˢ�¹�����Ϣ��6����  
			if (SUCCEEDED(hr))
			{
				BOOLEAN bTracked = false;

				for (int i = 0; i < BODY_COUNT; i++)
				{
					hr = pBody[i]->get_IsTracked(&bTracked); // ����Ƿ�׷��  
				}
			}
			for (int i = 0; i < BODY_COUNT; i++)
			{
				IBody* pSingleBody = pBody[i];

				if (pSingleBody)
				{
					Joint joints[JointType_Count];
					//��ȡ������Ϣ�ڵ�  
					pSingleBody->GetJoints(JointType::JointType_Count, joints);
					//����ȭ��չΪ��ת
					if (WaveRightHand(pSingleBody))
					{
						cout << "�������" << endl;
					}

				}

			}//for
			for (int count = 0; count < BODY_COUNT; count++)
			{
				SafeRelease(pBody[count]);
			}
			SafeRelease(pBodyFrame);  // �������ͷ� 
		}//if	

	}
	kinect->Close();  // ���ŵĹر�kinect
	cout << "kinect�ر�\n";
	return;
}
int main()
{
	KinectGestureRecognition();

	system("pause");
	return 0;
}
