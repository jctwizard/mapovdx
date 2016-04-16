#ifndef TILEMAP_GENERATOR
#define TILEMAP_GENERATOR

#include <iostream>
#include <vector>
#include <string>

#include "CImg.h"
#include "makesomenoise.h"

using namespace cimg_library;
using std::vector;
using std::cout;
using std::string;

#define RED 0
#define GREEN 1
#define BLUE 2

#define PI 3.14159265359

class TilemapGenerator
{
public:
	TilemapGenerator::TilemapGenerator();

	void Init();
	void GenerateTileMap();
	void CorrectTileMap();
	void DrawTileData();
	void DrawTile(int tilePosition, int tileSprite);
	void UpdateTilemap();

	void GenerateTileMapUpLeft();

	string filename;
	int outputWidth, outputHeight;
	int tileSize;
	int markovRadius;
	bool addNoise;
	bool useUpLeft;

	int offsetX, offsetY;

	CImg<int> tilemap;

private:
	int IncorrectTile(int x, int y, int tile, int currentTile);
	int GenerateNextTile(int currentTile, int x, int y);
	int GenerateTile(int x, int y, int tile, int currentTile);
	void GenerateTileData();
	void AnalyseTileData(int x, int y);
	int SumPixels(int tileX, int tileY);
	int GetChanceList(int x, int y);
	int GetChance(int x, int y, int currentTile, int otherTile);

	void AnalyseTileDataUpLeft();

	int corrections;
	int tilesX, tilesY, generatedTilesX, generatedTilesY, totalGeneratedTilesX, totalGeneratedTilesY;

	CImg<int> originalTilemap;
	vector<int> storedTiles, storedPixelSums, tilemapData, generatedTilemap;
	vector<vector<vector<int>>> tileChance;

	int Constrain(int value, int lower, int upper);
	bool HasValue(vector<int> list, int value);

	int minOffsetX, maxOffsetX;
	int minOffsetY, maxOffsetY;

	MakeSomeNoise NoiseMaker;
};

#endif