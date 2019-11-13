#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include <DirectXMath.h>

using namespace DirectX;

struct SimpleVertex // if you change this change the input layout!
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

#endif // !_STRUCTURES_H_


