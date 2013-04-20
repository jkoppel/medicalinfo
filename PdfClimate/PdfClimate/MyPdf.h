#ifndef _MUPDF_H_
#define _MUPDF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif

void mupdf_init();
void mupdf_destroy();
bool mupdf_open_file(char *filename);
void mupdf_close_file();
int mupdf_get_page_count();
bool mupdf_load_page(int pagenumber, int zoom);
void mupdf_winblit(HDC hdc, int winw, int winh, int cw, int ch);

#ifdef __cplusplus
}
#endif

#endif
