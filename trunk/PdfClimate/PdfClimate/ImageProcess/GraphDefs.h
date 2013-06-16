#ifndef _GRAPH_DEFS_H_
#define _GRAPH_DEFS_H_

#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#define MAX_GRAPH_NUM	128
#define MAX_GRAPH_ITEM_NUM	128
	
#define snprintf _snprintf

typedef enum GraphType {
    Graph_Dotted,
    Graph_Facet,
    Graph_Linear,
    Graph_Columnar,
    Graph_Unknown,
} GraphType;

typedef struct CoorPixelPair {
	float coor_x;
	float coor_y;
	int pix_x;
	int pix_y;
} CoorPixelPair;

inline BITMAPINFO *dib_init()
{
	BITMAPINFO *pDIBInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 12);
	pDIBInfo->bmiHeader.biSize = sizeof(pDIBInfo->bmiHeader);
	pDIBInfo->bmiHeader.biPlanes = 1;
	pDIBInfo->bmiHeader.biBitCount = 32;
	pDIBInfo->bmiHeader.biCompression = BI_RGB;
	pDIBInfo->bmiHeader.biXPelsPerMeter = 2834;
	pDIBInfo->bmiHeader.biYPelsPerMeter = 2834;
	pDIBInfo->bmiHeader.biClrUsed = 0;
	pDIBInfo->bmiHeader.biClrImportant = 0;
	pDIBInfo->bmiHeader.biClrUsed = 0;

	return pDIBInfo;
}

inline void dib_destroy(BITMAPINFO *pDIBInfo)
{
	char *p = (char*)pDIBInfo;
	free(p);
}

#endif
