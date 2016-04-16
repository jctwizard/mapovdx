// Application.h
#ifndef _MAPOV_H
#define _MAPOV_H

// Includes
#include <iostream>
#include <string>

#include "BaseApplication.h"
#include "CImg.h"

#include "OrthoMesh.h"
#include "ProcessShader.h"
#include "RenderTexture.h"
#include "TilemapGenerator.h"

#include "Console.h"

#define EFFECT_NONE			0x00
#define EFFECT_BLUR			0x01
#define EFFECT_EDGEDETECT	0x02
#define EFFECT_POSTERISE	0x04
#define EFFECT_INVERT		0x08

using namespace cimg_library;
using std::string;

class Mapov : public BaseApplication
{
public:

	Mapov(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);
	~Mapov();

	bool Frame();

private:
	inline bool FileExists(const std::string& name) 
	{
		ifstream f(name.c_str());
		if (f.good()) {
			f.close();
			return true;
		}
		else {
			f.close();
			return false;
		}
	}

	void InitTilemap();
	void DrawTilemap();
	bool Render();
	void ResizeMesh();

	int screenWidth, screenHeight;

	TilemapGenerator tilemapGenerator;
	ProcessShader* m_ProcessShader;
	OrthoMesh* m_Mesh;
	Texture* m_Texture;

	int effectFlags;
};

#endif