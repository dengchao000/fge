#include "stdafx.h"
#include "fgeRandom.h"
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")

static unsigned int g_seed=0;

void Random_Seed(int seed)
{
	if(!seed) g_seed=clock();
	else g_seed=seed;
}
//
//int Random_Int(int Min, int Max)
//{
//	return rand()%(Max-Min+1) + Min;
//}

int Random_Int(int Min, int Max)
{
	g_seed=214013*g_seed+2531011;
	return Min+(g_seed ^ g_seed>>15)%(Max-Min+1);
}

float Random_Float(float Min, float Max)
{
	g_seed=214013*g_seed+2531011;
	//return min+g_seed*(1.0f/4294967295.0f)*(max-min);
	return Min+(g_seed>>16)*(1.0f/65535.0f)*(Max-Min);
}