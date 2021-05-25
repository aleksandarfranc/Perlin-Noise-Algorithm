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

	int nOctaveCount = 1;
	float fScalingBias = 2.0f;
	int nMode = 1;

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

			PerlinNoise1D(nOutputSize, fNoiseSeed1D, nOctaveCount, fScalingBias, fPerlinNoise1D); // svakog frame-a mi generisemo noise
			for (int x = 0; x < nOutputSize; x++) {

				int y = - (fPerlinNoise1D[x] * (float)ScreenHeight()/2.0f) + (float)ScreenHeight()/2.0f; // '-' jer je (0,0) u koo sistemu gore levo
				for (int f = y; f < ScreenHeight()/2; f++) // zasto?
					Draw(x, f, PIXEL_SOLID, FG_GREEN);
			}
		}

		if (nMode == 2) // 2D Noise
		{
			if (m_keys[L'Z'].bReleased)
				for (int i = 0; i < nOutputHeight * nOutputWidth; i++)
					fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
				for (int y = 0; y < nOutputHeight; y++)
				{	// this is code from webcam video
					short bg_col, fg_col;
					wchar_t sym;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 12.0f);
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

	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutout) {
		//fSeed[0] = 0.5;
		for (int x = 0; x < nCount; x++) // why nCount
		{
			float fNoise = 0.0f;
			float fScale = 1.0f;
			float fScaleAcc = 0.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nCount >> o; // 256>>1=128,64,32,16,8,4,2;1
				// x = 63, nPitch = 32,16,8,4,2,1 => nSample1 = 32,48,56,60,62,63
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;
								
				float fBlend = (float)(x - nSample1) / (float)nPitch; //offset izmedju 0 i 1. 
				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];
				fNoise += fSample * fScale;

				fScaleAcc += fScale;
				fScale = fScale / fBias;
			}

			//
			fOutout[x] = fNoise/fScaleAcc;
		}
		
	}

	void PerlinNoise2D(int nWidth,int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutout) {
		fSeed[1] = 0.5; // // ZASTO? Zasto sa 0.5 je sve pravo; a blize jedinici sve vertikalno ; blize 0 sve horizontalno. Probaj 0.9999 i 0.0001 i vidi
		for (int x = 0; x < nWidth; x++) 
			for(int y = 0; y < nHeight; y++)
			{
				float fNoise = 0.0f;
				float fScale = 1.0f;
				float fScaleAcc = 0.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;

					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch; //offset ili ostatak koji je prikazan izmedju 0 i 1. 
					float fBlendY = (float)(y - nSampleY1) / (float)nPitch; //offset ili ostatak koji je prikazan izmedju 0 i 1. 

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale; // jasno mi je sta je cilj ovde (izmedju xSampleova (T i B)...) ali kako smo dosli od toga
					// SKONTAO SAMO NACRTAO SKICU

					fScaleAcc += fScale;
					fScale = fScale / fBias;
				}

				fOutout[y*nWidth + x] = fNoise / fScaleAcc;
			}

	}

};



int main() {

	OneLoneCoder_PerlinNoiseDemo game;
	game.ConstructConsole(256, 256, 2, 2);
	game.Start();

	return 0;
}