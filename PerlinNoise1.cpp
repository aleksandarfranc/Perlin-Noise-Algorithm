#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

#include "olcConsoleGameEngine.h"


class PerlinNoise : public olcConsoleGameEngine
{
private:
	int* nMap;
	int nWidth;
	int nHeight;
	float fBias = 2.0f;
	int nOctave = 1;
	int nMode = 1;

	// 1D variables
	float* fSeed1D = nullptr;
	float* fNoise1D = nullptr;

	// 2d Variables
	float* fSeed2D = nullptr;
	float* fNoise2D = nullptr;

public:

	bool OnUserCreate()
	{
		nWidth = ScreenWidth();
		nHeight = ScreenHeight();
		nMap = new int[nWidth * nHeight];
		memset(nMap, 0, nWidth * nHeight * sizeof(int));

		// 1D variables
		fSeed1D = new float[nWidth];
		fNoise1D = new float[nWidth];

		for (int i = 0; i < nWidth; i++)
			fSeed1D[i] = (float)rand() / (float)RAND_MAX;

		// 2D variables
		fSeed2D = new float[nWidth * nHeight];
		fNoise2D = new float[nWidth * nHeight];

		for (int i = 0; i < nWidth * nHeight; i++)
			fSeed2D[i] = (float)rand() / (float)RAND_MAX;


		return true;
	}

	void PerlinNoise1D(float* fSeed, int nWidth, int nOctave, float fBias, float* fOutput)
	{

		for (int i = 0; i < nWidth; i++)
		{
			float fNoise = 0.0f;
			float fScale = 1.0f;
			float fScaleAcc = 0.0f;
			for (int o = 0; o < nOctave; o++)
			{
				int nPitch = nWidth >> o;
				int nSample1 = (i / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nWidth;
				
				float fGrowth = (fSeed[nSample2] - fSeed[nSample1]) / nPitch;
				fNoise += (fSeed[nSample1] + fGrowth * (float)(i - nSample1)) * fScale;
				fScaleAcc += fScale;
				fScale /= fBias;
			}
			fOutput[i] = fNoise / fScaleAcc;
		}
	}

	bool OnUserUpdate(float fElapsedTime) 
	{
		if (m_keys[L'1'].bPressed)
			nMode = 1;

		if (m_keys[L'2'].bPressed)
			nMode = 2;


		if (m_keys[VK_UP].bPressed)
		{
			nOctave++;
			if (nOctave > 8)
				nOctave = 8;
		}

		if (m_keys[VK_DOWN].bPressed)
		{
			nOctave--;
			if (nOctave < 1)
				nOctave = 1;
		}

		if (m_keys[VK_LEFT].bPressed)
		{
			fBias -= 0.2f;
			if (fBias < 0.4f)
				fBias = 0.4f;
		}

		if (m_keys[VK_RIGHT].bPressed)
		{
			fBias += 0.2f;
			if (fBias > 2.4f)
				fBias = 2.4f;;
		}


		// 1D mode
		if (nMode == 1) 
		{

			if (m_keys[L'N'].bPressed)
				for (int i = 0; i < nWidth; i++)
					fSeed1D[i] = (float)rand() / (float)RAND_MAX;

			if (m_keys[L'X'].bPressed)
				for (int i = 0; i < nWidth; i++)
					fSeed1D[i] = 2 * ((float)rand() / (float)RAND_MAX) - 1.0f;



			PerlinNoise1D(fSeed1D, nWidth, nOctave, fBias, fNoise1D);
			memset(nMap, 0, sizeof(int) * nWidth * nHeight);

			// fill the map with zeros, ones and twos
			for (int i = 0; i < nWidth; i++)
			{
				int y = (float)nHeight / 2.0f - fNoise1D[i] * (float)nHeight /2.0f;
				if (y < nHeight / 2.0f)
				{
					for (int j = 0; j < nHeight / 2; j++)
						if (j < y)
							nMap[j * nWidth + i] = 0;
						else 
							nMap[j * nWidth + i] = 1;
				}
				else
				{
					for (int j = nHeight / 2; j < nHeight; j++)
						if (j < y)
							nMap[j * nWidth + i] = 2;
						else
							nMap[j * nWidth + i] = 0;
				}
			}

			// draw terrain
			for (int x = 0; x < nWidth; x++)
				for(int y = 0; y < nHeight; y++)
			{
				switch (nMap[y*nWidth + x])
				{
				case 1:
					Draw(x, y, PIXEL_SOLID, FG_GREEN);
					break;
				case 2:
					Draw(x, y, PIXEL_SOLID, FG_RED);
					break;
				case 0:
					Draw(x, y, PIXEL_SOLID, FG_BLACK);
					break;
				}
			}
		}

		// 2D mode
		else if (nMode == 2)
		{
			if (m_keys[L'N'].bPressed)
				for (int i = 0; i < nWidth * nHeight; i++)
					fSeed2D[i] = (float)rand() / (float)RAND_MAX;

			PerlinNoise2D(fSeed2D, nWidth, nHeight, nOctave, fBias, fNoise2D);

			// fill the map
			for (int i = 0; i < nWidth * nHeight; i++)
			{
				nMap[i] = 16 * fNoise2D[i];
			}

			// draw terrain
			for (int x = 0; x < nWidth; x++)
			{
				for (int y = 0; y < nHeight; y++)
				{
					short bg_col, fg_col;
					wchar_t pixel;
					int size = (int) (fNoise2D[y * nWidth + x] * 16.0f);
					switch (size)
					{
						case 0: pixel = PIXEL_SOLID; bg_col = BG_DARK_BLUE; fg_col = FG_DARK_BLUE; break;
					
						case 1: pixel = PIXEL_SOLID; bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; break;
						case 2: pixel = PIXEL_THREEQUARTERS; bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; break;
						case 3: pixel = PIXEL_HALF; bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; break;
						case 4: pixel = PIXEL_QUARTER; bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; break;

						case 5: pixel = PIXEL_SOLID; bg_col = BG_BLUE; fg_col = FG_GREEN; break;
						case 6: pixel = PIXEL_THREEQUARTERS; bg_col = BG_BLUE; fg_col = FG_GREEN; break;
						case 7: pixel = PIXEL_HALF; bg_col = BG_BLUE; fg_col = FG_GREEN; break;
						case 8: pixel = PIXEL_QUARTER; bg_col = BG_BLUE; fg_col = FG_GREEN; break;

						case 9: pixel = PIXEL_SOLID; bg_col = BG_GREEN; fg_col = FG_DARK_GREY; break;
						case 10: pixel = PIXEL_THREEQUARTERS; bg_col = BG_GREEN; fg_col = FG_DARK_GREY; break;
						case 11: pixel = PIXEL_HALF; bg_col = BG_GREEN; fg_col = FG_DARK_GREY; break;
						case 12: pixel = PIXEL_QUARTER; bg_col = BG_GREEN; fg_col = FG_DARK_GREY; break;

						case 13: pixel = PIXEL_SOLID; bg_col = BG_DARK_GREY; fg_col = FG_WHITE; break;
						case 14: pixel = PIXEL_THREEQUARTERS; bg_col = BG_DARK_GREY; fg_col = FG_WHITE; break;
						case 15: pixel = PIXEL_HALF; bg_col = BG_DARK_GREY; fg_col = FG_WHITE; break;
						case 16: pixel = PIXEL_QUARTER; bg_col = BG_DARK_GREY; fg_col = FG_WHITE; break;
					}
					Draw(x, y, pixel, bg_col | fg_col);
				}
			}
		}

		return true;
	}

	void PerlinNoise2D(float* fSeed, int nWidth, int nHeight, int nOctave, float fBias, float* fOutput)
	{
		for (int x = 0; x < nWidth; x++)
		{
			for (int y = 0; y < nHeight; y++)
			{
				float fNoise = 0.0f;
				float fScale = 1.0f;
				float fScaleAcc = 0.0f;
				for (int o = 0; o < nOctave; o++)
				{
					int nPitch = nWidth >> o;
					int nSample1X = (x / nPitch) * nPitch;
					int nSample2X = (nSample1X + nPitch) % nWidth;

					int nSample1Y = (y / nPitch) * nPitch;
					int nSample2Y = (nSample1Y + nPitch) % nWidth;

					float fGrowthX = (fSeed[nSample1Y * nWidth + nSample2X] - fSeed[nSample1Y * nWidth + nSample1X]) / (float)nPitch;
					float fGrowthY = (fSeed[nSample2Y * nWidth + nSample2X] - fSeed[nSample2Y * nWidth + nSample1X]) / (float)nPitch;

					float fNoiseX = (fSeed[nSample1Y * nWidth + nSample1X] + fGrowthX * (float)(x - nSample1X));
					float fNoiseY = (fSeed[nSample2Y * nWidth + nSample1X] + fGrowthY * (float)(x - nSample1X));

					float fGrowthXY = (fNoiseY - fNoiseX) / (float)nPitch;
					fNoise += (fNoiseX + fGrowthXY * (float)(y - nSample1Y)) * fScale;

					fScaleAcc += fScale;
					fScale /= fBias;
				}
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}
		}
	}
};




int main() {

	PerlinNoise game;
	game.ConstructConsole(256, 256, 2, 2);
	game.Start();

	system("pause");
	return 0;
}
