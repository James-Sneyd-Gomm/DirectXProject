#include "CameraCar.h"




CameraCar::CameraCar(XMFLOAT4 eye, XMFLOAT4 up, XMFLOAT4 at, bool free, float offSet) : Camera(eye, up, at, free)
{

	offSetVal = offSet;
	eyeVal = eye;
	atVal = at;
	upVal = up;

	XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y + offSetVal, eyeVal.z + -1.0f, eyeVal.w);
	XMVECTOR At = XMVectorSet(atVal.x, atVal.y + offSetVal, atVal.z, atVal.w);
	XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

	XMStoreFloat4x4(&_viewM, XMMatrixLookAtLH(Eye, At, Up));

}

CameraCar::~CameraCar()
{

}

void CameraCar::Update(XMFLOAT4 pos, float Rotation)
{
	eyeVal = pos;
	atVal = pos;

	if (freeCam == true)
	{
		XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y + offSetVal, eyeVal.z + -1.0f, eyeVal.w);
		XMVECTOR At = XMVectorSet(atVal.x, atVal.y + offSetVal, atVal.z, 0.0f);
		XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

		XMStoreFloat4x4(&_viewM, XMMatrixLookAtLH(Eye, At, Up)* XMMatrixRotationRollPitchYaw(0.0f, (Rotation * - 1), 0.0f));
	}

	if (freeCam == false)
	{
		XMVECTOR Eye = XMVectorSet(eyeVal.x, eyeVal.y , eyeVal.z, eyeVal.w);
		XMVECTOR At = XMVectorSet(atVal.x, atVal.y + 1.0f, atVal.z, 0.0f);
		XMVECTOR Up = XMVectorSet(upVal.x, upVal.y, upVal.z, upVal.w);

		XMStoreFloat4x4(&_viewM, XMMatrixLookAtLH(Eye, At, Up));
	}
}