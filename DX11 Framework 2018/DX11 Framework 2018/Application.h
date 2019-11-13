#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <fstream>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "Camera.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "CameraCar.h"


using namespace DirectX;


struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;
	float	pad;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float	SpecularPower;
	XMFLOAT3 EyePosW;

};

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	ID3D11Texture2D*		pBackBuffer;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11RenderTargetView* _pRenderTargetView2;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11VertexShader*     _pVertexShaderSky;
	ID3D11PixelShader*      _pPixelShaderSky;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pVertexBufferPr;
	ID3D11Buffer*           _pIndexBufferPr;
	ID3D11Buffer*           _pVertexBufferFl;
	ID3D11Buffer*           _pIndexBufferFl;
	ID3D11Buffer*           _pVertexBufferTv;
	ID3D11Buffer*           _pIndexBufferTv;
	ID3D11Buffer*           _pVertexBufferSkyBox;
	ID3D11Buffer*           _pIndexBufferSkyBox;
	ID3D11Buffer*           _pVertexBufferSphere;
	ID3D11Buffer*           _pIndexBufferSphere;
	ID3D11Buffer*           _pVertexBufferCage;
	ID3D11Buffer*           _pIndexBufferCage;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world,_world2,_world3,_world4, _world5, _world6, _world7,_world8,_world9,_worldSky;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11Texture2D*		_pBufferTexture;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _soildFill;
	ID3D11RasterizerState* _cullNone;
	ID3D11DepthStencilState* _lessEqual;
	XMFLOAT3				lightDirection;
	XMFLOAT4				diffuseMaterial;
	XMFLOAT4				diffuseLight;
	XMFLOAT4				ambientLight;
	XMFLOAT4				ambientMtrl;
	XMFLOAT4				specularMtrl;
	XMFLOAT4				specularLight;
	float					specularPower;
	XMFLOAT3				eyePosW;
	ID3D11ShaderResourceView* _pTextureRV;
	ID3D11ShaderResourceView* _pTextureRV2;
	ID3D11ShaderResourceView* _pTextureRV3;
	ID3D11ShaderResourceView* _pTextureRV4;
	ID3D11ShaderResourceView* _pSkyRV;
	ID3D11SamplerState*		_pSamplerLinear;
	ID3D11BlendState*		_pTransparency;
	Camera*					_Camera1;
	Camera*					_Camera2;
	CameraCar*				_CameraCar;
	CameraCar*				_CameraCar2;
	Camera*					_pMainCamera;
	CameraCar*				_pMainCameraCar;
	MeshData				objMesh;
	MeshData				objMeshBall;
	MeshData				objGoal;
	XMFLOAT4				carPos;
	XMFLOAT4				carFwd;
	XMFLOAT4				carFwdRot;
	float					col;
	float					row;
	float					maxInd;
	float					r;
	int						sphereVert;
	int						sphereFace;
	bool					activeCam;
	XMFLOAT4				ObjectPos[30];



private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	HRESULT InitVertexBufferPr();
	HRESULT InitIndexBufferPr();
	HRESULT InitVertexBufferFl();
	HRESULT InitIndexBufferFl();
	HRESULT InitVertexBufferTv();
	HRESULT InitIndexBufferTv();
	HRESULT InitVertexBufferSkyBox();
	HRESULT InitIndexBufferSkyBox();
	HRESULT InitVertexBufferCage();
	HRESULT InitIndexBufferCage();
	HRESULT InitSphere(int Latitude, int Longitude);
	char LoadPos(char *filename);


	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

