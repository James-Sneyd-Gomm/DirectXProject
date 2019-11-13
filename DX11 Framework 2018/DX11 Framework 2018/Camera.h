#ifndef _CAMERA_H
#define _CAMERA_H

#include <directxmath.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace DirectX;


class Camera
{
public:
	Camera(XMFLOAT4 eye, XMFLOAT4 up, XMFLOAT4 at, bool free );
	~Camera();

	void SetEye(XMFLOAT4 eye);
	XMFLOAT4 GetEye();
	void SetAt(XMFLOAT4 at);
	XMFLOAT4 GetAt();
	XMFLOAT4 GetUp();
	XMFLOAT4X4 GetView();

	virtual void Update();


protected:
	XMFLOAT4X4   _viewM;
	XMFLOAT4	 eyeVal;
	XMFLOAT4	 upVal;
	XMFLOAT4     atVal;
	XMFLOAT4	 toF;
	bool		 freeCam;
	float		 r;
	//UINT _WindowHeight;
	//UINT _WindowWidth;
};
#endif // !_CAMERA_H

