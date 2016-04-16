// Mapov.cpp
// Lab 1 example, simple coloured triangle mesh
#include "Mapov.h"

Mapov::Mapov(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in) : BaseApplication(hinstance, hwnd, screenWidth, screenHeight, in)
{
	RedirectIOToConsole();

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	InitTilemap();

	m_Texture = new Texture(m_Direct3D->GetDevice(), L"../res/GeneratedTilemap.png");
	m_ProcessShader = new ProcessShader(m_Direct3D->GetDevice(), hwnd);
}

Mapov::~Mapov()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (m_Mesh)
	{
		delete m_Mesh;
		m_Mesh = 0;
	}

	if (m_ProcessShader)
	{
		delete m_ProcessShader;
		m_ProcessShader = 0;
	}
}

bool Mapov::Frame()
{
	bool result;

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// Input
	if (m_Input->isKeyDown(VK_SPACE))
	{
		cout << "Begin generating" << endl;

		tilemapGenerator.GenerateTileMap();

		DrawTilemap();

		m_Input->SetKeyUp(VK_SPACE);
	}
	
	if (m_Input->isKeyDown('C'))
	{
		tilemapGenerator.CorrectTileMap();

		DrawTilemap();

		m_Input->SetKeyUp('C');
	}

	if (m_Input->isKeyDown('R'))
	{
		InitTilemap();

		m_Input->SetKeyUp('R');
	}

	// Set effects
	if (m_Input->isKeyDown('1'))
	{
		effectFlags |= EFFECT_BLUR;
		m_Input->SetKeyUp('1');
	}
	if (m_Input->isKeyDown('2'))
	{
		effectFlags |= EFFECT_EDGEDETECT;
		m_Input->SetKeyUp('2');
	}
	if (m_Input->isKeyDown('3'))
	{
		effectFlags |= EFFECT_POSTERISE;
		m_Input->SetKeyUp('3');
	}
	if (m_Input->isKeyDown('4'))
	{
		effectFlags |= EFFECT_INVERT;
		m_Input->SetKeyUp('4');
	}

	// Clear effects
	if (m_Input->isKeyDown('0'))
	{
		effectFlags = EFFECT_NONE;
		DrawTilemap();

		m_Input->SetKeyUp('0');
	}

	// Add Noise
	if (m_Input->isKeyDown('N'))
	{
		tilemapGenerator.addNoise = !tilemapGenerator.addNoise;
		DrawTilemap();

		m_Input->SetKeyUp('N');
	}

	// Pan around
	if (m_Input->isKeyDown(VK_LEFT))
	{
		tilemapGenerator.offsetX--;
		tilemapGenerator.UpdateTilemap();
		DrawTilemap();

		m_Input->SetKeyUp(VK_LEFT);
	}
	else if (m_Input->isKeyDown(VK_RIGHT))
	{
		tilemapGenerator.offsetX++;
		tilemapGenerator.UpdateTilemap();
		DrawTilemap();

		m_Input->SetKeyUp(VK_RIGHT);
	}
	if (m_Input->isKeyDown(VK_UP))
	{
		tilemapGenerator.offsetY--;
		tilemapGenerator.UpdateTilemap();
		DrawTilemap();

		m_Input->SetKeyUp(VK_UP);
	}
	else if (m_Input->isKeyDown(VK_DOWN))
	{
		tilemapGenerator.offsetY++;
		tilemapGenerator.UpdateTilemap();
		DrawTilemap();

		m_Input->SetKeyUp(VK_DOWN);
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Mapov::Render()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	
	// Clear the scene. (default blue colour)
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Send mesh data again
	m_Mesh->SendData(m_Direct3D->GetDeviceContext());

	// Set shader parameters (matrices and texture) using render texture
	m_ProcessShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, m_Texture->GetTexture(), tilemapGenerator.tilemap.width(), tilemapGenerator.tilemap.height(), effectFlags);

	// Render object (combination of mesh geometry and shader process
	m_ProcessShader->Render(m_Direct3D->GetDeviceContext(), m_Mesh->GetIndexCount());
	
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void Mapov::InitTilemap()
{
	effectFlags = EFFECT_NONE;

	// Create Mesh object
	cout << "\n\nMAPOV DX\n--------\n\n";

	string easySetup;
	cout << "Would you like to use the easy set up? (Y/N): ";
	cin >> easySetup;

	if (easySetup == "y" || easySetup == "Y" || easySetup == "yes" || easySetup == "Yes" || easySetup == "YES")
	{
		tilemapGenerator.filename = "../res/pallet_town.png";
		tilemapGenerator.tileSize = 32;
		tilemapGenerator.markovRadius = 3;
		tilemapGenerator.outputWidth = 1024;
		tilemapGenerator.outputHeight = 512;
	}
	else
	{
		cout << "\n\nPlease input the image path to process (e.g. '../res/pallet_town.png'): \n";
		cin >> tilemapGenerator.filename;
		cout << "\nNext the pixel size of the tiles to process (e.g. 16, 32): ";
		cin >> tilemapGenerator.tileSize;
		cout << "\nThe degree of markov chain analysis (1-5): ";
		cin >> tilemapGenerator.markovRadius;
		cout << "\nNow the width of the image to output: ";
		cin >> tilemapGenerator.outputWidth;
		cout << "\nAnd lastly the height: ";
		cin >> tilemapGenerator.outputHeight;
	}

	cout << endl << endl;

	while (!FileExists(tilemapGenerator.filename))
	{
		cout << "\n\nThe image path given was incorrect please try again (e.g. '../res/pallet_town.png'): \n";
		cin >> tilemapGenerator.filename;
	}

	tilemapGenerator.Init();

	cout << endl << "\nSuccess!" << endl << "\nControls:\nSpace:\tGenerate,\nC:\tCorrect,\n1-4:\tFilters,\n0:\tReset filters";

	ResizeMesh();
}

void Mapov::DrawTilemap()
{
	tilemapGenerator.DrawTileData();

	ResizeMesh();

	delete m_Texture;
	m_Texture = new Texture(m_Direct3D->GetDevice(), L"../res/GeneratedTilemap.png");
}

void Mapov::ResizeMesh()
{
	float meshWidth, meshHeight;
	float tilemapRatio = float(tilemapGenerator.tilemap.width()) / tilemapGenerator.tilemap.height();
	float screenRatio = float(screenWidth) / screenHeight;

	if (tilemapRatio > screenRatio)
	{
		meshWidth = screenWidth;
		meshHeight = screenWidth / tilemapRatio;
	}
	else
	{
		meshWidth = screenHeight * tilemapRatio;
		meshHeight = screenHeight;
	}

	if (m_Mesh != nullptr)
	{
		delete m_Mesh;
	}

	m_Mesh = new OrthoMesh(m_Direct3D->GetDevice(), meshWidth, meshHeight, 0.0f, 0.0f);
}