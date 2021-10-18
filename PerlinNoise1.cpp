#include <iostream>
#include <string>
#include <math.h>

using namespace std;

#include "olcConsoleGameEngine.h"


class OneLoneCoder_PerlinNoiseDemo : public olcConsoleGameEngine
 {
public:

	OneLoneCoder_PerlinNoiseDemo() 
	{
		m_sAppName = L"Perlin Noise";
	}
private:

	int nOctave = 1;
	float fScalingBias = 2.0f;

	int nMode = 2;

	// 1D variables
	float* fNoiseSeed1D = nullptr;
	float* fPerlinNoise1D = nullptr;
	int nOutputSize = 256;

	// 2D variables
	float* fNoiseSeed2D = nullptr;
	float* fPerlinNoise2D = nullptr;
	int nOutputWidth = 256;
	int nOutputHeight = 256;

	virtual bool OnUserCreate()
	{
		// 1D
		nOutputSize = ScreenWidth();
		fNoiseSeed1D = new float[nOutputSize];
		fPerlinNoise1D = new float[nOutputSize];

		for (int i = 0; i < nOutputSize; i++) {
			fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
		}

		nOutputWidth = ScreenWidth();
		nOutputHeight = ScreenHeight();
		fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
		fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];


		/*for(int x = 0; x < nOutputWidth; x++)
			for (int y = 0; y < nOutputHeight; y++) {
				fNoiseSeed2D[x * nOutputWidth + y] = (float) rand() / (float)RAND_MAX;
			}*/

		for(int i = 0 ; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
	
		if (m_keys[VK_SPACE].bReleased)
			nOctave++;

		if (m_keys[VK_UP].bReleased)
			fScalingBias -= 0.2f;

		if (m_keys[VK_DOWN].bReleased)
			fScalingBias += 0.2f;

		if (m_keys[L'1'].bReleased)
			nMode = 1;

		if (m_keys[L'2'].bReleased)
			nMode = 2;

		// --------------------------------

		if (fScalingBias < 0.2f)
			fScalingBias = 2.0f;

		if (fScalingBias > 3.2f)
			fScalingBias = 2.0f;

		if (nOctave > 8)
			nOctave = 1;
		

		// mode 1D
		if (nMode == 1) {
			if (m_keys[L'Z'].bReleased)
				for (int i = 0; i < nOutputSize; i++)fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

			PerlinNoisevezba(fNoiseSeed1D, nOutputSize, nOctave, fScalingBias, fPerlinNoise1D); // ovo nije efikasno jer se poziva f-ja svakog frejma ali zbog ovoga je moguce imati dinamicko menjaje terena

			for (int x = 0; x < nOutputSize; x++) {
				int y = -fPerlinNoise1D[x] * ScreenHeight() / 2 + ScreenHeight() / 2;

				for (int j = y; j < ScreenHeight() / 2; j++)
					Draw(x, j, PIXEL_SOLID, FG_GREEN);
			}
		}

		// mode 2D
		if (nMode == 2) {
			if (m_keys[L'Z'].bReleased)
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

			PerlinNoise2D(nOutputWidth, nOutputHeight,fNoiseSeed2D,nOctave,fScalingBias, fPerlinNoise2D);

			for(int x = 0; x<nOutputWidth; x++)
				for (int y = 0; y < nOutputHeight; y++) {
					short bg_col, fg_col;
					wchar_t sym;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 12.0f);

					switch (pixel_bw) {
					case 0: bg_col = BG_BLUE; fg_col = FG_BLUE; sym = PIXEL_SOLID; break;

					case 1: bg_col = BG_BLUE; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 2: bg_col = BG_BLUE; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
					case 3: bg_col = BG_BLUE; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 4: bg_col = BG_BLUE; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
					
					case 5: bg_col = BG_GREY; fg_col = FG_GREEN; sym = PIXEL_QUARTER; break;
					case 6: bg_col = BG_GREY; fg_col = FG_GREEN; sym = PIXEL_HALF; break;
					case 7: bg_col = BG_GREY; fg_col = FG_GREEN; sym = PIXEL_THREEQUARTERS; break;
					case 8: bg_col = BG_GREY; fg_col = FG_GREEN; sym = PIXEL_SOLID; break;
					
					case 9: bg_col = BG_BLACK; fg_col = FG_RED; sym = PIXEL_SOLID; break;
					case 10: bg_col = BG_BLACK; fg_col = FG_RED; sym = PIXEL_THREEQUARTERS; break;
					case 11: bg_col = BG_BLACK; fg_col = FG_RED; sym = PIXEL_HALF; break;
					case 12: bg_col = BG_BLACK; fg_col = FG_RED; sym = PIXEL_QUARTER; break;
					}

					Draw(x, y, sym, fg_col | bg_col);
				}

		}


		return true;
	}

	void PerlinNoisevezba(float* fSeed, int nWidth, int octave, float fBias, float* fPerlin) {

		for (int i = 0; i < nWidth; i++) {

			float fScaling = 1.0f;
			float fScalingSum = 0.0f;
			float between = 0.0f;

			for (int j = 0; j < octave; j++) {
				int nDots = nWidth >> j; // 256,128,64,32,16,8,4,2,1,0
				int lower = ((i / nDots) * nDots) % nWidth; // 0; 0, 128; 0,64,128,196;...  
				int upper = (lower + nDots) % nWidth; // 0; 128, 0; 64,128,196,0; ...

				float left = fSeed[lower] * fScaling;
				float right = fSeed[upper] * fScaling;

				float offset = (float)(i - lower) / (float)nDots;

				between += left - (left - right) * offset;

				fScalingSum += fScaling;
				fScaling /= fBias;
			}

			fPerlin[i] = between / fScalingSum; fScaling = 1.0f;
		}
	}

	void PerlinNoise2D(int nWidth, int nHeight, float* fNoiseSeed2D, int nOctave, float fBias, float* fPerlinNoise2D) {
		for (int x = 0; x < nWidth; x++) {
			for (int y = 0; y < nHeight; y++) {
				float fScaling = 1.0f;
				float fSumScaling = 0.0f;
				float Noise = 0.0f;

				for (int o = 0; o < nOctave; o++) {
					int indexX = nWidth >> o;

					int indexLeftX = (x / indexX) * indexX % nWidth;
					int indexRightX = (indexLeftX + indexX) % nWidth;

					int indexDownY = (y / indexX) * indexX % nWidth;
					int indexUpY = (indexDownY + indexX) % nWidth;

					float offsetX = float(x - indexLeftX) / (float)indexX;
					float offsetY = float(y - indexDownY) / (float)indexX;

					float SampleDownX = (1.0f - offsetX) * fNoiseSeed2D[indexDownY * nWidth + indexLeftX] + offsetX * fNoiseSeed2D[indexDownY * nWidth + indexRightX];
					float SampleUpX = (1.0f - offsetX) * fNoiseSeed2D[indexUpY * nWidth + indexLeftX] + offsetX * fNoiseSeed2D[indexUpY * nWidth + indexRightX];

					Noise += ((1.0f - offsetY) * SampleDownX + offsetY * SampleUpX) * fScaling;
					fSumScaling += fScaling;
					fScaling /= fBias;
				}

				fPerlinNoise2D[y * nWidth + x] = Noise / fSumScaling;
			}

		}

		
	}
};


int main() {

	OneLoneCoder_PerlinNoiseDemo game;
	game.ConstructConsole(256, 256, 2, 2);
	game.Start();

	return 0;
}
