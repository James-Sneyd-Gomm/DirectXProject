#ifndef _CAMERA_CAR_H
#define _CAMERA_CAR_H
#include "Camera.h"


class CameraCar : public Camera
{
public:
	CameraCar(XMFLOAT4 eye, XMFLOAT4 up, XMFLOAT4 at, bool free, float offSet);
	~CameraCar();

	void Update(XMFLOAT4 Pos , float Rotation);


private:
	float offSetVal;
};



#endif // !_CAMERA_CAR_H

