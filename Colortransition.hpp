#ifndef _COLORTRANSITION_
#define _COLORTRANSITION_
#include <SDL.h>

void colorTransition(float &pR, float &pG, float &pB, float aR, float aG, float aB, double speed) {
	if (pR < aR) {
		pR += aR / pR * speed;
	}
	if (pG < aG) {
		pG += aG / pG * speed;
	}
	if (pB < aB) {
		pB += aB / pB * speed;
	}

	if (pR > aR) {
		pR -= pR / aR * speed;
	}
	if (pG > aG) {
		pG -= pG / aG * speed;
	}
	if (pB > aB) {
		pB -= pB / aB * speed;
	}
}

void colorTransitionTex(SDL_Texture *texture, float &pR, float &pG, float &pB, float aR, float aG, float aB, double speed) {
	if (pR < aR) {
		pR += aR / pR * speed;
		if (pR > aR)
			pR = aR;
	}
	if (pG < aG) {
		pG += aG / pG * speed;
		if (pG > aG)
			pG = aG;
	}
	if (pB < aB) {
		pB += aB / pB * speed;
		if (pB > aB)
			pB = aB;
	}

	if (pR > aR) {
		pR -= pR / aR * speed;
		if (pR < aR)
			pR = aR;
	}
	if (pG > aG) {
		pG -= pG / aG * speed;
		if (pG < aG)
			pG = aG;
	}
	if (pB > aB) {
		pB -= pB / aB * speed;
		if (pB < aB)
			pB = aB;
	}

	SDL_SetTextureColorMod(texture, pR, pG, pB);		
}

#endif