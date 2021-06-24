#include <iostream>
#include <string>
#include <math.h>

using namespace std;

#include "olcConsoleGameEngine.h"
#include "pelinNoise.h"

class OneLoneCoder_PerlinNoiseDemo : public olcConsoleGameEngine {
public:
	OneLoneCoder_PerlinNoiseDemo() {
		m_sAppName = L"PerlinNoise";
	}
private:

	//2D noise variables
	int nOutputWidth = 256;
	int nOutputHeight = 256;
	float* fNoiseSeed2D = nullptr;
	float* fPerlinNoise2D = nullptr;

	//1D noise variables
	float* fNoiseSeed1D = nullptr;
	float* fPerlinNoise1D = nullptr;
	int nOutputSize = 256;

	int nOctaveCount = 6;
	float fScalingBias = 2.0f;
	int nMode = 2;

	virtual bool OnUserCreate() {
		// 1D initializing
		nOutputSize = ScreenWidth();
		fNoiseSeed1D = new float[nOutputSize];
		fPerlinNoise1D = new float[nOutputSize];

		for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

		// 2D initializing
		nOutputWidth = ScreenWidth();
		nOutputHeight = ScreenHeight();

		fNoiseSeed2D = new float[nOutputHeight * nOutputWidth];
		fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];

		for (int i = 0; i < nOutputHeight * nOutputWidth; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
 
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

		if (m_keys[VK_SPACE].bReleased || m_keys[VK_UP].bReleased) {
			nOctaveCount++;
			if (nOctaveCount == 9)
				nOctaveCount = 1;
		}
		if (m_keys[VK_DOWN].bReleased)
			nOctaveCount--;

		if (m_keys[L'A'].bReleased)
			fScalingBias -= 0.2f;

		if (m_keys[L'S'].bReleased)
			fScalingBias += 0.2f;

		if (m_keys[L'1'].bReleased)
			nMode = 1;

		if (m_keys[L'2'].bReleased)
			nMode = 2;

		if (m_keys[L'3'].bReleased)
			nMode = 3;

		if (fScalingBias < 0.2f)
			fScalingBias = 0.2f;

		if (fScalingBias > 4.0f)
			fScalingBias = 2.0f;

		if (nMode == 1) {

			if (m_keys[L'Z'].bReleased)
				for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / float(RAND_MAX);

			PerlinNoise1D( fNoiseSeed1D,nOutputSize, nOctaveCount, fScalingBias, fPerlinNoise1D); // svakog frame-a mi generisemo noise
			for (int x = 0; x < nOutputSize; x++) {

				int y = - (fPerlinNoise1D[x] * (float)ScreenHeight()/2.0f) + (float)ScreenHeight()/2.0f; // '-' jer je (0,0) u koo sistemu gore levo
				for (int f = y; f < ScreenHeight()/2; f++) // zasto? crta ga od gore na dole. (0,0) je gore levo
					Draw(x, f, PIXEL_SOLID, FG_GREEN);
			}
		}

		if (nMode == 2) // 2D Noise
		{
			if (m_keys[L'Z'].bReleased)
				for (int i = 0; i < nOutputHeight * nOutputWidth; i++)
					fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
			PerlinNoise2D( nOutputWidth, nOutputHeight,fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
				for (int y = 0; y < nOutputHeight; y++)
				{	
					short bg_col, fg_col;
					wchar_t sym;
					float brisi = fPerlinNoise2D[y * nOutputWidth + x];
					int pixel_bw = (int)(brisi * 12.0f);
					switch (pixel_bw)
					{
					case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

					case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
					case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
					case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

					case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
					case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
					case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

					case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
					case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
					}

					Draw(x, y, sym, fg_col | bg_col);

				}
		}

		if (nMode == 3) // 2D Noise - colourised
		{
			if (m_keys[L'Z'].bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
			{
				for (int y = 0; y < nOutputHeight; y++)
				{
					short bg_col, fg_col;
					wchar_t sym;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 16.0f);
					switch (pixel_bw)
					{
					case 0: bg_col = BG_DARK_BLUE; fg_col = FG_DARK_BLUE; sym = PIXEL_SOLID; break;

					case 1: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_QUARTER; break;
					case 2: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_HALF; break;
					case 3: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_THREEQUARTERS; break;
					case 4: bg_col = BG_DARK_BLUE; fg_col = FG_BLUE; sym = PIXEL_SOLID; break;

					case 5: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_QUARTER; break;
					case 6: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_HALF; break;
					case 7: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_THREEQUARTERS; break;
					case 8: bg_col = BG_BLUE; fg_col = FG_GREEN; sym = PIXEL_SOLID; break;

					case 9:  bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
					case 10: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
					case 11: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
					case 12: bg_col = BG_GREEN; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

					case 13: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
					case 14: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
					case 15: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
					case 16: bg_col = BG_DARK_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
					}

					Draw(x, y, sym, fg_col | bg_col);
				}
			}
		}

		return true;
	}

	void PerlinNoise1D(float* seed, int nWidth, int octave, float fBias, float* output) {

		

		for (int i = 0; i < nWidth; i++) {

			float Noise = 0.0f;
			float fScalling = 1.0f;
			float ScallingSum = 0.0f;

			for (int j = 0; j < octave; j++) {
				int index = nWidth >> j;

				int nLeft_Index_X = ((i / index) * index) % nWidth;
				int rightIndex = (nLeft_Index_X + index) % nWidth;

				float left = seed[nLeft_Index_X] * fScalling;
				float right = seed[rightIndex] * fScalling;

				float offset = (float)(i - nLeft_Index_X) / (float)index;

				Noise += left - (left - right) * offset;

				ScallingSum += fScalling;
				fScalling /= fBias;

			}
			output[i] = Noise / ScallingSum;
			fScalling = 1.0f;
		}

	}

	void PerlinNoise2D( int nWidth, int nHeight, float* seed, int octave, float fBias, float* output) {
		for (int i = 0; i < nWidth; i++) 
			for (int j = 0; j < nHeight; j++) 
			{
				float Sample = 0.0f;
				float SumScalling = 0.0f;
				float scalling = 1.0f;
				

				for (int k = 0; k < octave; k++) 
				{
					int nIndexI = nWidth >> k;
					int nLeft_Index_X = (i / nIndexI) * nIndexI % nWidth;
					int nBack_Index_Y = (j / nIndexI) * nIndexI % nWidth;

					int nRight_Index_X = (nLeft_Index_X + nIndexI) % nWidth;
					int nFront_Index_Y = (nBack_Index_Y + nIndexI) % nWidth;

					float offsetX = (float)(i - nLeft_Index_X) / (float)nIndexI;
					float offsetY = (float)(j - nBack_Index_Y) / (float)nIndexI;
					
					float back = (1.0f - offsetX) * seed[nBack_Index_Y * nWidth + nLeft_Index_X] + offsetX * seed[nBack_Index_Y * nWidth + nRight_Index_X];
					float front = (1.0f - offsetX) * seed[nFront_Index_Y * nWidth + nLeft_Index_X] + offsetX * seed[nFront_Index_Y * nWidth + nRight_Index_X];

					SumScalling += scalling;
					Sample +=  (offsetY *( front - back) + back) * scalling;
					scalling = scalling / fBias;
					
				}
				float f = Sample / SumScalling;
				output[j * nWidth + i] = f;
			}
	}


};



int main() {

	OneLoneCoder_PerlinNoiseDemo game;
	game.ConstructConsole(256, 256, 2, 2);
	game.Start();

	return 0;
}
