// Application.h
#ifndef _MAPOV_H
#define _MAPOV_H

// Includes
#include <iostream>
#include "Shlwapi.h"
#include <vector>
#include <string>

#include "BaseApplication.h"
#include "CImg.h"

#include "OrthoMesh.h"
#include "ProcessShader.h"
#include "RenderTexture.h"
#include "makesomenoise.h"

#include "Console.h"

#define RED 0
#define GREEN 1
#define BLUE 2

#define EFFECT_NONE			0x00
#define EFFECT_BLUR			0x01
#define EFFECT_EDGEDETECT	0x02
#define EFFECT_POSTERISE	0x04
#define EFFECT_INVERT		0x08

#define PI 3.14159265359

using namespace cimg_library;
using std::string;
using std::vector;

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

	void Init();
	bool Render();
	void UpdateTilemap();
	void GenerateTileData();
	void AnalyseTileData(int x, int y);
	int SumPixels(int tileX, int tileY);
	int GetChanceList(int x, int y);
	int GetChance(int x, int y, int currentTile, int otherTile);
	int GenerateNextTile(int currentTile, int x, int y);
	void GenerateTile(int x, int y, int tile, int currentTile);
	void GenerateTileMap();
	int IncorrectTile(int x, int y, int tile, int currentTile);
	void CorrectTileMap();
	void DrawTileData();
	void DrawTile(int tilePosition, int tileSprite);
	void ResizeMesh();

	int screenWidth, screenHeight;
	int outputWidth, outputHeight;
	int tileSize;
	int tilesX, tilesY, generatedTilesX, generatedTilesY, totalGeneratedTilesX, totalGeneratedTilesY;
	int markovRadius;
	int corrections;

	CImg<int> originalTilemap;
	CImg<int> tilemap;
	vector<int> storedTiles, storedPixelSums, tilemapData, generatedTilemap;
	vector<vector<vector<int>>> tileChance;

	int Constrain(int value, int lower, int upper);
	bool HasValue(vector<int> list, int value);

	ProcessShader* m_ProcessShader;
	OrthoMesh* m_Mesh;
	Texture* m_Texture;

	int effectFlags;

	int offsetX, minOffsetX, maxOffsetX;
	int offsetY, minOffsetY, maxOffsetY;

	bool addNoise;

	MakeSomeNoise NoiseMaker;
};

#endif