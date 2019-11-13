#include <iostream>
#include "Camera.h"

using namespace std;


Camera::Camera(XMFLOAT4 eye, XMFLOAT4 up, XMFLOAT4 at, bool free)
{
	eyeVal = eye;
	upVal = up;
	atVal = at;
	freeCam = free;
	r = 0.0;


	if (!freeCam)
	{
	XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y, eyeVal.z, eyeVal.w);
	XMVECTOR At = XMVectorSet(atVal.x, atVal.y, atVal.z, atVal.w);
	XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

	XMStoreFloat4x4(&_viewM, XMMatrixLookAtLH(Eye, At, Up));
	}
	else if (freeCam)
	{
		XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y, eyeVal.z, eyeVal.w);
		XMVECTOR At = XMVectorSet(eyeVal.x, eyeVal.y, eyeVal.z + 1.0f, atVal.w);
		XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

		XMStoreFloat4x4(&_viewM, XMMatrixLookToLH(Eye, At, Up));
	}

}

Camera::~Camera()
{

}

void Camera::SetEye(XMFLOAT4 eye)
{
	eyeVal = eye;

}

void Camera::SetAt(XMFLOAT4 at)
{
	atVal = at;
}

XMFLOAT4 Camera::GetEye()
{
	return eyeVal;
}

XMFLOAT4 Camera::GetAt()
{
	return atVal;
}

XMFLOAT4 Camera::GetUp()
{
	return upVal;
}

XMFLOAT4X4 Camera::GetView()
{
	return _viewM;
}

void Camera::Update()
{

	if (freeCam)
	{

	if (GetAsyncKeyState(VK_UP))
	{

		eyeVal.x += (atVal.x / 10);
		eyeVal.y += (atVal.y / 10);
		eyeVal.z += (atVal.z / 10);

	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
	
		eyeVal.x -= (atVal.x / 10);
		eyeVal.y -= (atVal.y / 10);
		eyeVal.z -= (atVal.z / 10);

	}

	if (GetAsyncKeyState(VK_LEFT))
	{
	
		r -= 0.5;
		atVal.x = toF.x;
		atVal.y = toF.y;
		atVal.z = toF.z;
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		r += 0.5;
		atVal.x = toF.x;
		atVal.y = toF.y;
		atVal.z = toF.z;
	}


		r = XMConvertToRadians(r);
		XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y, eyeVal.z, eyeVal.w);
		XMVECTOR To = XMVectorSet(atVal.x, atVal.y, atVal.z, atVal.w);
		XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

		XMMATRIX rot = XMMatrixRotationAxis(Up, r);

		To = XMVector3Transform(To, rot);

		XMStoreFloat4(&toF, To);

		XMStoreFloat4x4(&_viewM, XMMatrixLookToLH(Eye, To, Up) * rot);



	}



	if (!freeCam)
	{
	XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y, eyeVal.z, eyeVal.w);
	XMVECTOR At = XMVectorSet(atVal.x, atVal.y, atVal.z, atVal.w);
	XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);
	XMStoreFloat4x4(&_viewM, XMMatrixLookAtLH(Eye, At, Up));
	}




}
