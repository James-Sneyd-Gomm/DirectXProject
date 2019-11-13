#include "Application.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	pBackBuffer = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexShaderSky = nullptr;
	_pPixelShaderSky = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	_depthStencilBuffer = nullptr;
	_depthStencilView = nullptr;
	_wireFrame = nullptr;
	_pVertexBufferPr = nullptr;
	_pIndexBufferPr = nullptr;
	_pVertexBufferFl = nullptr;
	_pIndexBufferFl = nullptr;
	_pVertexBufferTv = nullptr;
	_pIndexBufferTv = nullptr;
	_pVertexBufferSkyBox = nullptr;
	_pIndexBufferSkyBox = nullptr;
	_pVertexBufferSphere = nullptr;
	_pIndexBufferSphere = nullptr;
	_pTextureRV = nullptr;
	_pTextureRV2 = nullptr;
	_pTextureRV3 = nullptr;
	_pTextureRV4 = nullptr;
	_pSkyRV = nullptr;
	_pSamplerLinear = nullptr;
	_pTransparency = nullptr;
	_Camera1 = nullptr;
	_Camera2 = nullptr;
	_pMainCamera = nullptr;
	_pVertexBufferCage = nullptr;
	_pIndexBufferCage = nullptr;
	_lessEqual = nullptr;
	_cullNone = nullptr;


	
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix

	
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	carPos.x = 0.0f;
	carPos.y = 0.0f;
	carPos.z = 0.0f;
	carFwd.x = 0.0f;
	carFwd.y = 0.0f;
	carFwd.z = 1.0f;
	carFwd.w = 0.0f;
	r = 0.0f;
	activeCam = false;
 //   // Initialize the view matrix, cam location change eye to move cam
	_Camera1 = new Camera({ 0.0f,5.0f,-9.0f,0.0f }, { 0.0f,1.0f,0.0f,0.0f, }, { 0.0f,0.0f,1.0f,0.0f}, true);
	_Camera2 = new Camera({ 0.0f,70.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f,0.0f, }, { 0.0,0.0f,1.0f,0.0f }, false);
	_CameraCar = new CameraCar({ carPos.x,carPos.y,carPos.z ,0.0f }, { 0.0f,1.0f,0.0f,0.0f, }, { carPos.x,carPos.y + 1.0f,carPos.z,0.0f }, true , 10.0f);
	_pMainCamera = _Camera1;
	_pMainCameraCar = _CameraCar;

	LoadPos("Pos.txt");




    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 300.0f));
	objMesh = OBJLoader::Load("Car.obj", _pd3dDevice);
	objMeshBall = OBJLoader::Load("Sphere.obj", _pd3dDevice);
	objGoal = OBJLoader::Load("cylinder.obj", _pd3dDevice);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shaders
    ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pVSBlobSky = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
	hr = CompileShaderFromFile(L"DX11 Framework SkyBox.fx", "VS", "vs_4_0", &pVSBlobSky);


	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shaders
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	hr = _pd3dDevice->CreateVertexShader(pVSBlobSky->GetBufferPointer(), pVSBlobSky->GetBufferSize(), nullptr, &_pVertexShaderSky);

	if (FAILED(hr))
	{
		pVSBlobSky->Release();
		return hr;
	}

	// Compile the pixel shaders
	ID3DBlob* pPSBlob = nullptr;
	ID3DBlob* pPSBlobSky = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	hr = CompileShaderFromFile(L"DX11 Framework SkyBox.fx", "PS", "ps_4_0", &pPSBlobSky);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

	hr = _pd3dDevice->CreatePixelShader(pPSBlobSky->GetBufferPointer(), pPSBlobSky->GetBufferSize(), nullptr, &_pPixelShaderSky);
	pPSBlobSky->Release();

	if (FAILED(hr))
		return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // starts at bite 0 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},

	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();
	//pVSBlobSky->Release();


	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		// front face
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2 (0.0f,0.0f) }, //0 
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) },//1 
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) },//2  
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//3  

		// back face 
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5   
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6	 
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7   

		// top face
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5  
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) }, //0
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//1 

		// bottom face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) },//2  
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) },//3  
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6	 
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7  

		// Left Face
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4 -16
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) }, //0 -17
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6	 - 18
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//2 - 19

		// Right Face
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) },//1  -20
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5  -21
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) },//3 -22
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7  -23
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24; // change this if you add new vertices or there wont be ehough memory
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer() //change this to cube
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		// front 
        0,1,2,
        2,1,3,
		//bottom 
		12,13,14,
		13,12,15,
		// back 
		5,4,7,
		7,4,6,
		// top 
		8,9,10,
		10,9,11,
		// left
		16,17,18,
		18,17,19,
		// right
		20,21,22,
		22,21,23,


    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 72;     // change this if adding new indicies 
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitVertexBufferSkyBox()
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		// front face
	{ XMFLOAT3(-150.0f, 150.0f, -150.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) }, //0 
	{ XMFLOAT3(150.0f, 150.0f, -150.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) },//1 
	{ XMFLOAT3(-150.0f, -150.0f, -150.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) },//2  
	{ XMFLOAT3(150.0f, -150.0f, -150.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//3  

																						// back face 
	{ XMFLOAT3(-150.0f, 150.0f, 150.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  
	{ XMFLOAT3(150.0f, 150.0f, 150.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5   
	{ XMFLOAT3(-150.0f, -150.0f, 150.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6	 
	{ XMFLOAT3(150.0f, -150.0f, 150.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7   

																					  // top face
	{ XMFLOAT3(-150.0f, 150.0f, 150.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  
	{ XMFLOAT3(150.0f, 150.0f, 150.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5  
	{ XMFLOAT3(-150.0f, 150.0f, -150.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) }, //0
	{ XMFLOAT3(150.0f, 150.0f, -150.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//1 

																					  // bottom face
	{ XMFLOAT3(-150.0f, -150.0f, -150.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) },//2 12 
	{ XMFLOAT3(150.0f, -150.0f, -150.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) },//3 13 
	{ XMFLOAT3(-150.0f, -150.0f, 150.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6 -14	 
	{ XMFLOAT3(150.0f, -150.0f, 150.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7 - 15 

																					  // Left Face
	{ XMFLOAT3(-150.0f, 150.0f, 150.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4 -16
	{ XMFLOAT3(-150.0f, 150.0f, -150.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) }, //0 -17
	{ XMFLOAT3(-150.0f, -150.0f, 150.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,0.0f) },//6	 - 18
	{ XMFLOAT3(-150.0f, -150.0f, -150.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) },//2 - 19

																						  // Right Face
	{ XMFLOAT3(150.0f, 150.0f, -150.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) },//1  -20
	{ XMFLOAT3(150.0f, 150.0f, 150.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) },//5  -21
	{ XMFLOAT3(150.0f, -150.0f, -150.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) },//3 -22
	{ XMFLOAT3(150.0f, -150.0f, 150.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },//7  -23
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24; 
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferSkyBox);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBufferSkyBox() //change this to cube
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		// front 
		2,1,0,
		3,1,2,
		//bottom 
		12,13,14,
		14,15,13,
		// back 
		7,4,5,
		6,4,7,
		// top 
		10,9,8,
		11,9,10,
		// left
		18,17,16,
		19,17,18,
		// right
		22,21,20,
		23,21,22,


	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 72;     // change this if adding new indicies 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferSkyBox);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitVertexBufferCage()
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		// front face
	{ XMFLOAT3(-75.0f, 75.0f, -75.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) }, //0 
	{ XMFLOAT3(75.0f, 75.0f, -75.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,75.0f) },//1 
	{ XMFLOAT3(-75.0f, -75.0f, -75.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(75.0f,0.0f) },//2  
	{ XMFLOAT3(75.0f, -75.0f, -75.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(75.0f,75.0f) },//3  

																						// back face 
	{ XMFLOAT3(-75.0f, 75.0f, 75.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  
	{ XMFLOAT3(75.0f, 75.0f, 75.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,75.0f) },//5   
	{ XMFLOAT3(-75.0f, -75.0f, 75.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(75.0f,0.0f) },//6	 
	{ XMFLOAT3(75.0f, -75.0f, 75.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(75.0f,75.0f) },//7   

																					  // top face
	{ XMFLOAT3(-75.0f, 75.0f, 75.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4  -8
	{ XMFLOAT3(75.0f, 75.0f, 75.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,75.0f) },//5  -9
	{ XMFLOAT3(-75.0f, 75.0f, -75.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(75.0f,0.0f) }, //0 -10
	{ XMFLOAT3(75.0f, 75.0f, -75.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(75.0f,75.0f) },//1  -11


																					  // Left Face
	{ XMFLOAT3(-75.0f, 75.0f, 75.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f),XMFLOAT2(0.0f,0.0f) },//4 -12
	{ XMFLOAT3(-75.0f, 75.0f, -75.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,75.0f) }, //0 -13
	{ XMFLOAT3(-75.0f, -75.0f, 75.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(75.0f,0.0f) },//6	 - 14
	{ XMFLOAT3(-75.0f, -75.0f, -75.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(75.0f,75.0f) },//2 - 15

																						  // Right Face
	{ XMFLOAT3(75.0f, 75.0f, -75.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) },//1  -16
	{ XMFLOAT3(75.0f, 75.0f, 75.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f,75.0f) },//5  -17
	{ XMFLOAT3(75.0f, -75.0f, -75.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(75.0f,0.0f) },//3 -18
	{ XMFLOAT3(75.0f, -75.0f, 75.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(75.0f,75.0f) },//7  -19
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 20; // change this if you add new vertices or there wont be ehough memory
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferCage);

	if (FAILED(hr))
		return hr;

	return S_OK;
}


HRESULT Application::InitIndexBufferCage() 
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		// front 
		2,1,0,
		3,1,2,
		//left 
		12,13,14,
		14,15,13,
		// back 
		7,4,5,
		6,4,7,
		// top 
		10,9,8,
		11,9,10,
		// right
		18,17,16,
		19,17,18,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 66;     // change this if adding new indicies 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferCage);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitVertexBufferPr()
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
	{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, //1
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, //2
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },//3
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },//4
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 5; // change this if you add new vertices or there wont be ehough memory
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferPr);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBufferPr() //change this to cube
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		0,2,1,
		0,3,4,
		0,4,2,
		0,1,3,
		3,1,2,
		2,4,3,
		
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 18;     // change this if adding new indicies 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferPr);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitVertexBufferFl() // Floor Vertex Buffer
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{


		{ XMFLOAT3(-75.0f, 0.0f, 75.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(75.0f, 0.0f, 75.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f,75.0f) },
		{ XMFLOAT3(-75.0f, 0.0f, -75.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(75.0f,0.0f) },
		{ XMFLOAT3(75.0f, 0.0f, -75.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(75.0f,75.0f) },



	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4; 
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferFl);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBufferFl() // Floor Index Buffer
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		0,1,2,
		2,1,3,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;     // change this if adding new indicies 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferFl);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitVertexBufferTv() // Floor Vertex Buffer
{
	HRESULT hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{


	{ XMFLOAT3(-20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(0.0f,0.0f) },
	{ XMFLOAT3(20.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f,0.0f) },
	{ XMFLOAT3(-20.0f, -20.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f,1.0f) },
	{ XMFLOAT3(20.0f, -20.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f,1.0f) },



	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferTv);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBufferTv()
{
	HRESULT hr;

	// Create index buffer
	WORD indices[] =
	{
		0,1,2,
		2,1,3,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;     // change this if adding new indicies 
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferTv);

	if (FAILED(hr))
		return hr;

	return S_OK;
}


HRESULT Application::InitSphere(int Latitude, int Longitude)
{
	HRESULT hr;
	sphereVert = ((Latitude - 2) * Longitude) + 2;
	sphereFace = ((Latitude - 3) * (Longitude) * 2) + (Longitude * 2);

	float Yaw = 0.0f;
	float Pitch = 0.0f;
	SimpleVertex* vertices = new SimpleVertex[sphereVert];

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);	
	XMMATRIX rotationX;
	XMMATRIX rotationY;

	vertices[0].Pos.x = 0.0f;
	vertices[0].Pos.y = 0.0f;
	vertices[0].Pos.z = 1.0f;

	for (int i = 0; i < Latitude - 2; i++)
	{
		Pitch = (i + 1) * (3.14 / (Latitude - 1));
		rotationX = XMMatrixRotationX(Pitch);
		
		for (int j = 0; j < Longitude; j++)
		{
			Yaw = j * (6.28 / (Longitude));
			rotationY = XMMatrixRotationZ(Yaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (rotationX * rotationY));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i* Longitude + j + 1].Pos.x = XMVectorGetX(currVertPos);
			vertices[i* Longitude + j + 1].Pos.y = XMVectorGetX(currVertPos);
			vertices[i* Longitude + j + 1].Pos.z = XMVectorGetX(currVertPos);
		}
	}

	vertices[sphereVert - 1].Pos.x = 0.0f;
	vertices[sphereVert - 1].Pos.y = 0.0f;
	vertices[sphereVert - 1].Pos.z = -1.0f;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * sphereVert;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferSphere);

	if (FAILED(hr))
	return hr;

	WORD* indices = new WORD[(sphereFace * 3)];

	int k = 0;
	for (int l = 0; l < Longitude - 1; l++)
		{
			indices[k] = 0;
			indices[k + 1] = l + 1;
			indices[k + 2] = l + 2;
			k += 3;
		}

		indices[k] = 0;
		indices[k + 1] = Longitude;
		indices[k + 2] = 1;
		k += 3;

		for (int i = 0; i < Latitude - 3; i++)
		{
			for (int j = 0; j < Longitude - 1; j++)
			{
				indices[k] = i * Longitude + j + 1;
				indices[k + 1] = i * Longitude + j + 2;
				indices[k + 2] = i * Longitude + j + 3;
				indices[k + 3] = (i + 1) * Longitude + j + 1;
				indices[k + 4] = i * Longitude + j + 2;
				indices[k + 5] = (i + 1) * Longitude + j + 2;

				k += 6;
			}

			indices[k] = (i * Longitude) + Longitude;
			indices[k + 1] = (i * Longitude) + 1;
			indices[k + 2] = ((i + 1) * Longitude) + Longitude;
			indices[k + 3] = ((i + 1) * Longitude) + Longitude;
			indices[k + 4] = (i * Longitude) + 1;
			indices[k + 5] = ((i + 1) * Longitude) + 1;

			k += 6;
		}

		for (int l = 0; l < Longitude - 1; l++)
		{
			indices[k] = sphereVert - 1;
			indices[k + 1] = (sphereVert - 1) - (l + 1);
			indices[k + 2] = (sphereVert - 1) - (l + 2);
			k += 3;
		}

	indices[k] = sphereVert - 1;
	indices[k + 1] = (sphereVert - 1) - Longitude;
	indices[k + 2] = sphereVert - 2;

	D3D11_BUFFER_DESC bdi;
	ZeroMemory(&bdi, sizeof(bdi));
		
	bdi.Usage = D3D11_USAGE_DEFAULT;
	bdi.ByteWidth = sizeof(WORD) * sphereFace * 3;
	bdi.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdi.CPUAccessFlags = 0;
	bdi.MiscFlags = 0;
		
	D3D11_SUBRESOURCE_DATA InitDataIn;
	ZeroMemory(&InitDataIn, sizeof(InitDataIn));
	InitDataIn.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bdi, &InitDataIn, &_pIndexBufferSphere);

	if (FAILED(hr))
		return hr;

	return S_OK;

}

// atempted generation of terain which could be used for hight maps
//HRESULT Application::InitVertexBufferTr() // terain vert buffer
//{
//	HRESULT hr;
//	int i = 0;
//	int j = 0;
//	int k = 0;
//	col = 5;
//	row = 5;
//
//	// Create vertex buffer
//	SimpleVertex* vertices = new SimpleVertex[col * row];
//
//	for (i = 0; i < row; i++)
//	{
//		for (j = 0; j < col; j++)
//		{
//			vertices[k].Pos.x = j;
//			vertices[k].Pos.y = 0.0;
//			vertices[k].Pos.z = i;
//			vertices[k].Normal.x = 0.0;
//			vertices[k].Normal.y = 1.0;
//			vertices[k].Normal.z = 0.0;
//			k++;
//		}
//	}
//
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(SimpleVertex) * (col * row); 
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = vertices;
//
//	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBufferTr);
//
//	if (FAILED(hr))
//		return hr;
//
//	return S_OK;
//}
//
//HRESULT Application::InitIndexBufferTr() // Floor Index Buffer
//{
//	HRESULT hr;
//	int i = 0;
//	int j = 0;
//	int k = 0;
//	maxInd = ((col * row) * 6);
//
//
//	// Create index buffer
//	WORD* indices = new WORD[((col * row) * 6)];
//
//	for (i = 0; i < (col*row); i++)
//	{
//		indices[j] = k;
//		indices[j + 1] = k + col;
//		indices[j + 2] = k + 1;
//		indices[j + 3] = k + 1;
//		indices[j + 4] = k + col;
//		indices[j + 5] = (k + col) + 1;
//
//		j += 6;
//		k++;
//	}
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(WORD) * maxInd;  
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = indices;
//	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBufferTr);
//
//	if (FAILED(hr))
//		return hr;
//
//	return S_OK;
//}



HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"James sneyd-gomm gw000979", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	// Render to the back buffer
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // how many bits per channel, unorm = unsigned normalised
    sd.BufferDesc.RefreshRate.Numerator = 60; // refresh rate FPS
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView); // change this to change were DX renders too
    //pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	//Define the depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create depth/stencil buffer
	hr = _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);

	if (FAILED(hr))
		return hr;
	//Create depth/stencil view
	hr = _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);
	if (FAILED(hr))
		return hr;


	// om = output merger, nullptr used for depth stencil buffer

	// Texture Desc

	D3D11_TEXTURE2D_DESC texture2DDesc;

	ZeroMemory(&texture2DDesc, sizeof(texture2DDesc));
	texture2DDesc.Width = _WindowWidth;
	texture2DDesc.Height = _WindowHeight;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;

	hr = _pd3dDevice->CreateTexture2D(&texture2DDesc, nullptr, &_pBufferTexture);

	if (FAILED(hr))
		return hr;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texture2DDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	hr = _pd3dDevice->CreateRenderTargetView(_pBufferTexture, &renderTargetViewDesc, &_pRenderTargetView2);


	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = texture2DDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = _pd3dDevice->CreateShaderResourceView(_pBufferTexture, &shaderResourceViewDesc, &_pTextureRV4);

	if (FAILED(hr))
		return hr;

    // Setup the viewport , can be used for splitscreen etc
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	//vertex buffers
	InitVertexBufferPr();
	InitVertexBufferFl();
	InitVertexBufferTv();
	InitVertexBuffer();
	InitVertexBufferCage();
	InitVertexBufferSkyBox();

	//index buffers
	InitIndexBufferPr();
	InitIndexBufferFl();
	InitIndexBufferTv();
	InitIndexBuffer();
	InitIndexBufferCage();
	InitIndexBufferSkyBox();





    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	if (FAILED(hr))
		return hr;

	//Rastrizer
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC sfdesc;
	ZeroMemory(&sfdesc, sizeof(D3D11_RASTERIZER_DESC));
	sfdesc.FillMode = D3D11_FILL_SOLID;
	sfdesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&sfdesc, &_soildFill);

	_pImmediateContext->RSSetState(_soildFill);

	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC cndesc;
	ZeroMemory(&cndesc, sizeof(D3D11_RASTERIZER_DESC));
	cndesc.FillMode = D3D11_FILL_SOLID;
	cndesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cndesc, &_cullNone);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Chainlink.dds", nullptr, &_pTextureRV2);
	CreateDDSTextureFromFile(_pd3dDevice, L"asphalt.dds", nullptr, &_pTextureRV3);
	CreateDDSTextureFromFileEx(_pd3dDevice, L"SpaceMap.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false,nullptr,&_pSkyRV);

	

	if (FAILED(hr))
		return hr;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	hr = _pd3dDevice->CreateBlendState(&blendDesc, &_pTransparency);

	if (FAILED(hr))
		return hr;
    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
	if (_pVertexShaderSky) _pVertexShaderSky->Release();
	if (_pPixelShaderSky) _pPixelShaderSky->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pRenderTargetView2) _pRenderTargetView2->Release();
	if (pBackBuffer)  pBackBuffer->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_pVertexBufferPr) _pVertexBufferPr->Release();
	if (_pIndexBufferPr) _pIndexBufferPr->Release();
	if (_pVertexBufferFl) _pVertexBufferFl->Release();
	if (_pIndexBufferFl) _pIndexBufferFl->Release();
	if (_pVertexBufferTv) _pVertexBufferTv->Release();
	if (_pIndexBufferTv) _pIndexBufferTv->Release();
	if (_pVertexBufferSphere) _pVertexBufferSphere->Release();
	if (_pIndexBufferSphere) _pIndexBufferSphere->Release();
	if (_pTextureRV) _pTextureRV->Release();
	if (_pTextureRV2) _pTextureRV2->Release();
	if (_pTextureRV3) _pTextureRV3->Release();
	if (_pTextureRV4) _pTextureRV4->Release();
	if (_pSkyRV) _pSkyRV->Release();
	if (_pSamplerLinear) _pSamplerLinear->Release();
	if (_pTransparency) _pTransparency->Release();
	if (_pVertexBufferCage) _pVertexBufferCage->Release();
	if (_pIndexBufferCage) _pIndexBufferCage->Release();
	if (_pVertexBufferSkyBox) _pVertexBufferSkyBox->Release();
	if (_pIndexBufferSkyBox) _pIndexBufferSkyBox->Release();
	if (_lessEqual) _lessEqual->Release();
	if (_cullNone) _cullNone->Release();
	delete _Camera1;
	delete _Camera2;
	
	

}

void Application::Update()
{
    // Update our time

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView2, _depthStencilView);
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView2, ClearColor);
	Draw();
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);



    static float t = 0.0f;


	XMMATRIX fwdMatrix = XMLoadFloat4x4(&_world);
	XMVECTOR fwdSet = XMVectorSet(carFwd.x, carFwd.y, carFwd.z, carFwd.w);
	XMVECTOR fwd = XMVector4Transform(fwdSet, fwdMatrix);

	XMVECTOR Pos = XMVectorSet(carPos.x, carPos.y, carPos.z, carPos.w);

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	if (GetAsyncKeyState(VK_SHIFT))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}

	if (GetAsyncKeyState(0x57)) // W
	{
		Pos += fwd;
	}

	if (GetAsyncKeyState(0x53)) // S
	{
		Pos -= fwd;
	}

	if (GetAsyncKeyState(0x41)) // A
	{
		r -= 0.005;
	}

	if (GetAsyncKeyState(0x44)) // D
	{
		r += 0.005;
	}

	if (GetAsyncKeyState(0x31)) //1
	{
		_pMainCamera = _Camera1;
		activeCam = false;
	}
	if (GetAsyncKeyState(0x32)) //2
	{
		_pMainCamera = _Camera2;
		activeCam = false;
	}
	if (GetAsyncKeyState(0x33)) //3
	{
		activeCam = true;
	}

	XMStoreFloat4(&carPos, Pos);
	XMStoreFloat4(&carFwdRot, fwd);

	_pMainCamera->Update();
	_pMainCameraCar->Update(carPos , r);
	XMStoreFloat4x4(&_world, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationRollPitchYaw(0.0f,r,0.0f)* XMMatrixTranslation(carPos.x, carPos.y, carPos.z));
	XMStoreFloat4x4(&_world2, XMMatrixTranslation(0.0f,0.0f,0.0f));
	XMStoreFloat4x4(&_world3, XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(0.0f, 5.0f, 20.0f));
	XMStoreFloat4x4(&_world4, XMMatrixTranslation(0.0f,0.0f,0.0f));
	XMStoreFloat4x4(&_world5, XMMatrixTranslation(0.0f, 30.0f, 74.0f));
	XMStoreFloat4x4(&_world6, XMMatrixScaling(1.0f, 5.0f, 1.0f) * XMMatrixTranslation(16.0f, 5.0f, 70.0f));
	XMStoreFloat4x4(&_world7, XMMatrixScaling(1.0f, 5.0f, 1.0f) * XMMatrixTranslation(-16.0f, 5.0f, 70.0f));
	XMStoreFloat4x4(&_world8, XMMatrixScaling(1.0f, 5.0f, 1.0f) * XMMatrixTranslation(16.0f, 5.0f, -70.0f));
	XMStoreFloat4x4(&_world9, XMMatrixScaling(1.0f, 5.0f, 1.0f) * XMMatrixTranslation(-16.0f, 5.0f, -70.0f));
	XMStoreFloat4x4(&_worldSky, XMMatrixTranslation(ObjectPos[9].x, ObjectPos[9].y, ObjectPos[9].z ));
	
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view;
	if (activeCam == true)
	{
		 view = XMLoadFloat4x4(&_pMainCameraCar->GetView());
	}

	if (activeCam == false)
	{
		view = XMLoadFloat4x4(&_pMainCamera->GetView());
	}
	
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
	lightDirection = XMFLOAT3(0.25f, 0.5f, 1.0f);
	diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ambientMtrl = XMFLOAT4(0.2f, 0.2f, 0.3f, 1.0f);
	ambientLight = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.2f);
	specularMtrl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	specularPower = 10.0f;
	eyePosW = XMFLOAT3(0.0f, 0.0f, -9.0f);

	//Blending
	float blendFactor[] = { 0.75f,0.75f,0.75f,1.0f };
	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
	//_pImmediateContext->OMSetBlendState(_pTransparency, blendFactor, 0xffffffff);
    //
    // Update variables
    //
    ConstantBuffer cb; // sends data to graphics card, Must be in 16 byte chuncks anything else will break it.
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.DiffuseLight = diffuseLight;
	cb.DiffuseMtrl = diffuseMaterial;
	cb.LightVecW = lightDirection;
	cb.AmbientLight = ambientLight;
	cb.AmbientMtrl = ambientMtrl;
	cb.SpecularMtrl = specularMtrl;
	cb.SpecularLight = specularLight;
	cb.SpecularPower = specularPower;
	cb.EyePosW = eyePosW;
	
	

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	


	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

	// Car
	_pImmediateContext->IASetVertexBuffers(0, 1, &objMesh.VertexBuffer, &objMesh.VBStride, &objMesh.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objMesh.IndexCount, 0, 0);

	//Cage
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV2);
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferCage, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBufferCage, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world2);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(66, 0, 0);

	//Ball
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshBall.VertexBuffer, &objMeshBall.VBStride, &objMeshBall.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objMeshBall.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world3);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objMeshBall.IndexCount, 0, 0);

	// Floor
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferFl, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBufferFl, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world4);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(6, 0, 0);

	//Jumbotron
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV4);
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferTv, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBufferTv, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world5);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(6, 0, 0);

	//goal
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->IASetVertexBuffers(0, 1, &objGoal.VertexBuffer, &objGoal.VBStride, &objGoal.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objGoal.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world6);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objGoal.IndexCount, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &objGoal.VertexBuffer, &objGoal.VBStride, &objGoal.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objGoal.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world7);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objGoal.IndexCount, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &objGoal.VertexBuffer, &objGoal.VBStride, &objGoal.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objGoal.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world8);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objGoal.IndexCount, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &objGoal.VertexBuffer, &objGoal.VBStride, &objGoal.VBOffset);
	_pImmediateContext->IASetIndexBuffer(objGoal.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world9);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(objGoal.IndexCount, 0, 0);
	//skybox


	_pImmediateContext->VSSetShader(_pVertexShaderSky, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShaderSky, nullptr, 0);

	_pImmediateContext->PSSetShaderResources(0, 1, &_pSkyRV);
	_pImmediateContext->RSSetState(_cullNone);
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferSkyBox, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBufferSkyBox, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_worldSky);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(72, 0, 0);



	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);


    
 /*    Present our back buffer to our front buffer*/
  
    _pSwapChain->Present(0, 0);
}

char Application::LoadPos(char *filename)
{
	int i = 0;
	FILE* inFile;
	inFile = fopen(filename, "r");

	while (1)
	{
		//char buf[256];
		//int line = fscanf(inFile, "%s", buf);
		if (i == 10) 
		{
			break;
		}


			fscanf(inFile, "%f %f %f %f\n",
				&ObjectPos[i].x,
				&ObjectPos[i].y,
				&ObjectPos[i].z,
				&ObjectPos[i].w);
			i++;
		

	}

	fclose(inFile);
	return(1);

}