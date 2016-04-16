#include "TilemapGenerator.h"

TilemapGenerator::TilemapGenerator()
{
	useUpLeft = false;
}

void TilemapGenerator::Init()
{
	addNoise = false;

	offsetX = minOffsetX = maxOffsetX = 0;
	offsetY = minOffsetY = maxOffsetY = 0;

	corrections = 100;

	tilemap = CImg<int>(filename.c_str());
	originalTilemap = CImg<int>(tilemap);

	tilesX = tilemap.width() / tileSize;
	tilesY = tilemap.height() / tileSize;

	if (outputWidth == -1)
	{
		outputWidth = tilemap.width();
	}
	if (outputHeight == -1)
	{
		outputHeight = tilemap.height();
	}

	generatedTilesX = totalGeneratedTilesX = outputWidth / tileSize;
	generatedTilesY = totalGeneratedTilesY = outputHeight / tileSize;

	storedTiles.clear();
	storedPixelSums.clear();
	tilemapData.clear();
	generatedTilemap.clear();

	GenerateTileData();

	tileChance.clear();

	if (useUpLeft)
	{
		AnalyseTileDataUpLeft();
	}
	else
	{
		// Analyse data in all directions for the markov radius
		for (int x = -markovRadius; x <= markovRadius; x++)
		{
			for (int y = -markovRadius; y <= markovRadius; y++)
			{
				if (x == 0 && y == 0) continue;

				cout << "\rAnalysing tile offset " << x << ", " << y << "   ";

				tileChance.push_back(vector<vector<int>>());

				AnalyseTileData(x, y);
			}
		}
	}

	tilemap.normalize(0, 255);
	tilemap.save("../res/GeneratedTilemap.png");
}

void TilemapGenerator::UpdateTilemap()
{
	if (generatedTilemap.size() == 0)
	{
		return;
	}

	if (offsetX < minOffsetX)
	{
		for (int newTile = 0; newTile < totalGeneratedTilesY; newTile++)
		{
			int currentTile = generatedTilemap.at(totalGeneratedTilesX * newTile + newTile);
			generatedTilemap.insert(generatedTilemap.begin() + totalGeneratedTilesX * newTile + newTile, GenerateNextTile(currentTile, -1, 0));
		}

		totalGeneratedTilesX++;

		minOffsetX = offsetX;
	}
	else if (offsetX > maxOffsetX)
	{
		for (int newTile = 0; newTile < totalGeneratedTilesY; newTile++)
		{
			int currentTile = generatedTilemap.at(totalGeneratedTilesX * (newTile + 1) + newTile - 1);
			generatedTilemap.insert(generatedTilemap.begin() + totalGeneratedTilesX * (newTile + 1) + newTile, GenerateNextTile(currentTile, 1, 0));
		}

		totalGeneratedTilesX++;

		maxOffsetX = offsetX;
	}

	if (offsetY < minOffsetY)
	{
		for (int newTile = 0; newTile < totalGeneratedTilesX; newTile++)
		{
			int currentTile = generatedTilemap.at(newTile * 2);
			generatedTilemap.insert(generatedTilemap.begin() + newTile, GenerateNextTile(currentTile, 0, -1));
		}

		totalGeneratedTilesY++;

		minOffsetY = offsetY;
	}
	else if (offsetY > maxOffsetY)
	{
		for (int newTile = 0; newTile < totalGeneratedTilesX; newTile++)
		{
			int currentTile = generatedTilemap.at(totalGeneratedTilesX * (totalGeneratedTilesY - 1) + newTile);
			generatedTilemap.insert(generatedTilemap.end(), GenerateNextTile(currentTile, 0, 1));
		}

		totalGeneratedTilesY++;

		maxOffsetY = offsetY;
	}
}

void TilemapGenerator::DrawTileData()
{
	tilemap.resize(outputWidth, outputHeight);

	int tile = totalGeneratedTilesX * (offsetY - minOffsetY) + (offsetX - minOffsetX);

	for (int tileY = 0; tileY < generatedTilesY; tileY++)
	{
		for (int tileX = 0; tileX < generatedTilesX; tileX++)
		{
			DrawTile(tileY * generatedTilesX + tileX, generatedTilemap.at(tile));

			tile += 1;
		}

		tile += totalGeneratedTilesX - generatedTilesX;
	}

	tilemap.normalize(0, 255);
	tilemap.save("../res/GeneratedTilemap.png");
}

void TilemapGenerator::DrawTile(int tilePosition, int tileSprite)
{
	int drawPixelX = (tilePosition % generatedTilesX) * tileSize;
	int drawPixelY = (tilePosition / generatedTilesX) * tileSize;

	int sprite = storedTiles.at(tileSprite);
	int baseSpritePixelX = (sprite % tilesX) * tileSize;
	int baseSpritePixelY = (sprite / tilesX) * tileSize;

	for (int pixelY = 0; pixelY < tileSize; pixelY++)
	{
		for (int pixelX = 0; pixelX < tileSize; pixelX++)
		{
			float noise = 0.0f;

			if (addNoise)
			{
				noise = NoiseMaker.Simplex((drawPixelX + pixelX) * 0.01f, (drawPixelY + pixelY) * 0.01f);
			}

			tilemap(drawPixelX + pixelX, drawPixelY + pixelY, 0, RED) = originalTilemap(baseSpritePixelX + pixelX, baseSpritePixelY + pixelY, 0, RED) + noise * 255 * 0.333f;
			tilemap(drawPixelX + pixelX, drawPixelY + pixelY, 0, GREEN) = originalTilemap(baseSpritePixelX + pixelX, baseSpritePixelY + pixelY, 0, GREEN) + noise * 255 * 0.333f;
			tilemap(drawPixelX + pixelX, drawPixelY + pixelY, 0, BLUE) = originalTilemap(baseSpritePixelX + pixelX, baseSpritePixelY + pixelY, 0, BLUE) + noise * 255 * 0.333f;
		}
	}
}

void TilemapGenerator::CorrectTileMap()
{
	int tileX = rand() % generatedTilesX;
	int tileY = rand() % generatedTilesY;
	int tile = totalGeneratedTilesX * (tileY + (offsetY - minOffsetY)) + (tileX + (offsetX - minOffsetX));

	vector<int> incorrectTiles;
	int correctionCount = 0;

	for (int correction = 0; correction < corrections; correction++)
	{
		int currentTile = generatedTilemap.at(tile);

		for (int x = -markovRadius; x <= markovRadius; x++)
		{
			for (int y = -markovRadius; y <= markovRadius; y++)
			{
				if (x == 0 && y == 0) continue;

				int nextTile = IncorrectTile(x, y, tile, currentTile);

				if (nextTile != -1)
				{
					incorrectTiles.push_back(nextTile);
					correctionCount++;
				}
			}
		}

		if (incorrectTiles.size() == 0)
		{
			tileX = rand() % generatedTilesX;
			tileY = rand() % generatedTilesY;
			tile = totalGeneratedTilesX * (tileY + (offsetY - minOffsetY)) + (tileX + (offsetX - minOffsetX));
		}
		else
		{
			int randomIncorrectTile = rand() % incorrectTiles.size();
			tile = incorrectTiles.at(randomIncorrectTile);
			incorrectTiles.erase(incorrectTiles.begin() + randomIncorrectTile);
		}
	}

	cout << "corrections made: " << correctionCount << "\n";
}

int TilemapGenerator::IncorrectTile(int x, int y, int tile, int currentTile)
{
	int storedTile = Constrain(tile + x + generatedTilesX * y, 0, generatedTilesX * generatedTilesY - 1);

	if (GetChance(x, y, currentTile, generatedTilemap.at(storedTile)) == 0)
	{
		generatedTilemap[storedTile] = GenerateNextTile(currentTile, x, y);

		return storedTile;
	}

	return -1;
}

int TilemapGenerator::GetChance(int x, int y, int currentTile, int otherTile)
{
	int chanceList = GetChanceList(x, y);

	return tileChance.at(chanceList).at(currentTile).at(otherTile);
}

int TilemapGenerator::GetChanceList(int x, int y)
{
	int chanceList = 0;

	for (int _x = -markovRadius; _x <= markovRadius; _x++)
	{
		for (int _y = -markovRadius; _y <= markovRadius; _y++)
		{
			if (_x == 0 && _y == 0)
			{
				continue;
			}

			if (x == _x && y == _y)
			{
				return chanceList;
			}

			chanceList++;
		}
	}

	return chanceList;
}

void TilemapGenerator::GenerateTileMap()
{
	generatedTilemap.clear();
	totalGeneratedTilesX = generatedTilesX;
	totalGeneratedTilesY = generatedTilesY;
	offsetX = minOffsetX = maxOffsetX = 0;
	offsetY = minOffsetY = maxOffsetY = 0;

	for (int tile = 0; tile < generatedTilesX * generatedTilesY; tile++)
	{
		generatedTilemap.push_back(-1);
	}

	if (useUpLeft)
	{
		GenerateTileMapUpLeft();
		return;
	}

	int currentTile = rand() % storedTiles.size();
	generatedTilemap[0] = currentTile;

	int tile = 0;

	while (HasValue(generatedTilemap, -1))
	{
		currentTile = generatedTilemap.at(tile);

		if (currentTile == -1)
		{
			currentTile = rand() % storedTiles.size();
			generatedTilemap[tile] = currentTile;
		}

		for (int x = -markovRadius; x <= markovRadius; x++)
		{
			for (int y = -markovRadius; y <= markovRadius; y++)
			{
				if (x == 0 && y == 0) continue;

				GenerateTile(x, y, tile, currentTile);
			}
		}

		tile = rand() % (generatedTilesX * generatedTilesY);
	}
}

void TilemapGenerator::AnalyseTileDataUpLeft()
{
	for (int leftTile = 0; leftTile < storedTiles.size(); leftTile++)
	{
		tileChance.push_back(vector<vector<int>>());

		for (int upTile = 0; upTile < storedTiles.size(); upTile++)
		{
			tileChance.at(leftTile).push_back(vector<int>());
		}
	}

	// Increment the tile chance based on the left tile and the above tile (start after the first row and ignore first tile in each row)
	for (int tile = 0; tile < tilemapData.size(); tile++)
	{
		if ((tile >= generatedTilesX) && (tile % generatedTilesX != 0))
		{
			int leftTileIndex = tilemapData.at(tile - 1);
			int upTileIndex = tilemapData.at(tile - generatedTilesX);
			int currentTileIndex = tilemapData.at(tile);

			tileChance.at(leftTileIndex).at(upTileIndex).push_back(currentTileIndex);
		}

		if (tile % generatedTilesX == 0)
		{
			cout << "\Analysed data for row " << tile % generatedTilesX + 1 << "/" << generatedTilesY;
		}
	}
}

void TilemapGenerator::GenerateTileMapUpLeft()
{
	for (int tile = 0; tile < generatedTilesX * generatedTilesY; tile++)
	{
		int nextTile, upTile, leftTile;

		if (tile > generatedTilesX)
		{
			upTile = generatedTilemap.at(tile - generatedTilesX);
		}
		else
		{
			upTile = tilemapData.at(rand() % tilemapData.size());
		}
		
		if (tile % generatedTilesX != 0)
		{
			leftTile = generatedTilemap.at(tile - 1);
		}
		else
		{
			leftTile = tilemapData.at(rand() % tilemapData.size());
		}

		int patternCount = tileChance.at(leftTile).at(upTile).size();

		if (patternCount == 0)
		{
			nextTile = tilemapData.at(rand() % tilemapData.size());
		}
		else
		{
			nextTile = tileChance.at(leftTile).at(upTile).at(rand() % patternCount);
			cout << nextTile;
		}

		generatedTilemap.at(tile) = nextTile;
	}
}

int TilemapGenerator::GenerateTile(int x, int y, int tile, int currentTile)
{
	if (generatedTilemap.at(Constrain(tile + x + generatedTilesX * y, 0, generatedTilesX * generatedTilesY - 1)) == -1)
	{
		return generatedTilemap[Constrain(tile + x + generatedTilesX * y, 0, generatedTilesX * generatedTilesY - 1)] = GenerateNextTile(currentTile, x, y);
	}

	return generatedTilemap.at(Constrain(tile + x + generatedTilesX * y, 0, generatedTilesX * generatedTilesY - 1));
}

int TilemapGenerator::GenerateNextTile(int currentTile, int x, int y)
{
	int chanceList = GetChanceList(x, y);

	vector<int> tileChances;

	for (int otherTileIndex = 0; otherTileIndex < tileChance.at(chanceList).at(currentTile).size(); otherTileIndex++)
	{
		for (int chance = 0; chance < tileChance.at(chanceList).at(currentTile).at(otherTileIndex); chance++)
		{
			tileChances.push_back(otherTileIndex);
		}
	}

	if (tileChances.size() > 0)
	{
		int randomIndex = rand() % tileChances.size();

		return tileChances.at(randomIndex);
	}

	return 0;
}

void TilemapGenerator::AnalyseTileData(int x, int y)
{
	int chanceList = GetChanceList(x, y);

	for (int storedTile = 0; storedTile < storedTiles.size(); storedTile++)
	{
		tileChance.at(chanceList).push_back(vector<int>());

		for (int storedTileChance = 0; storedTileChance < storedTiles.size(); storedTileChance++)
		{
			tileChance.at(chanceList).at(tileChance.at(chanceList).size() - 1).push_back(0);
		}
	}

	for (int tile = 0; tile < tilemapData.size(); tile++)
	{
		int currentTileIndex = tilemapData.at(tile);

		int position = tile + x + (y * tilesX);

		if (position >= 0 && position < tilemapData.size())
		{
			int nextTileIndex = tilemapData.at(position);
			int currentTileChance = tileChance.at(chanceList).at(currentTileIndex).at(nextTileIndex);
			tileChance.at(chanceList).at(currentTileIndex).at(nextTileIndex) = currentTileChance + 1;
		}
	}
}

void TilemapGenerator::GenerateTileData()
{
	int duplicateTiles = 0;

	for (int tileY = 0; tileY < tilesY; tileY++)
	{
		for (int tileX = 0; tileX < tilesX; tileX++)
		{
			int tileID = -1;
			int pixelSum = SumPixels(tileX, tileY);

			for (int storedTile = 0; storedTile < storedTiles.size(); storedTile++)
			{
				if (storedPixelSums.at(storedTile) == pixelSum)
				{
					tileID = storedTile;

					duplicateTiles++;
				}
			}

			if (tileID == -1)
			{
				storedTiles.push_back(tileY * tilesX + tileX);
				storedPixelSums.push_back(pixelSum);
				tileID = storedTiles.size() - 1;
			}

			tilemapData.push_back(tileID);
		}

		cout << "\rGenerated data for row " << tileY + 1 << "/" << tilesY;
	}
	cout << "\nDuplicate tiles: " << duplicateTiles << "\n";
	cout << "Stored tiles: " << storedTiles.size() << "\n";
}

int TilemapGenerator::SumPixels(int tileX, int tileY)
{
	int sum = 0;

	int basePixelX = tileX * tileSize;
	int basePixelY = tileY * tileSize;

	for (int pixelY = 0; pixelY < tileSize; pixelY++)
	{
		for (int pixelX = 0; pixelX < tileSize; pixelX++)
		{
			sum += (tilemap(basePixelX + pixelX, basePixelY + pixelY, 0, RED) + tilemap(basePixelX + pixelX, basePixelY + pixelY, 0, GREEN) + tilemap(basePixelX + pixelX, basePixelY + pixelY, 0, BLUE)) * pixelX * pixelY;
		}
	}

	return sum;
}

int TilemapGenerator::Constrain(int value, int lower, int upper)
{
	if (value < lower)
	{
		return lower;
	}
	else if (value > upper)
	{
		return upper;
	}

	return value;
}

bool TilemapGenerator::HasValue(vector<int> list, int value)
{
	for each (int item in list)
	{
		if (item == value)
		{
			return true;
		}
	}

	return false;
}
