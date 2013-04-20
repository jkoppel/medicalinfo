#include "MyPdf.h"
#include "fitz.h"

#include <windows.h>

static bool fz_is_init = false;
static bool fz_is_open = false;

static fz_pixmap *pix = NULL;
static fz_device *dev = NULL;
static fz_context *ctx = NULL;
static fz_document *doc = NULL;
static fz_page *page = NULL;

static BITMAPINFO *dibinf = NULL;
static HBRUSH bgbrush;
static HBRUSH shbrush;

void mupdf_init()
{
    mupdf_destroy();

	bgbrush = CreateSolidBrush(RGB(0x70,0x70,0x70));
	shbrush = CreateSolidBrush(RGB(0x40,0x40,0x40));

    dibinf = malloc(sizeof(BITMAPINFO) + 12);
	assert(dibinf);
	dibinf->bmiHeader.biSize = sizeof(dibinf->bmiHeader);
	dibinf->bmiHeader.biPlanes = 1;
	dibinf->bmiHeader.biBitCount = 32;
	dibinf->bmiHeader.biCompression = BI_RGB;
	dibinf->bmiHeader.biXPelsPerMeter = 2834;
	dibinf->bmiHeader.biYPelsPerMeter = 2834;
	dibinf->bmiHeader.biClrUsed = 0;
	dibinf->bmiHeader.biClrImportant = 0;
	dibinf->bmiHeader.biClrUsed = 0;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

    fz_is_init = true;
}

void mupdf_destroy()
{
    if (fz_is_init) {
        mupdf_close_file();

        free(dibinf);
        dibinf = NULL;

        fz_free_context(ctx);
        ctx = NULL;

        fz_is_init = false;
    }
}

bool mupdf_open_file(char *filename)
{
    mupdf_close_file();

	doc = fz_open_document(ctx, filename);

    if (doc) {
        fz_is_open = true;
        return true;
    }
    return false;
}

void mupdf_close_file()
{
    if (fz_is_open) {
        if (pix){
        	fz_drop_pixmap(ctx, pix);
            pix = NULL;
        }
        if (page) {
    	    fz_free_page(doc, page);
            page = NULL;
        }

        fz_close_document(doc);
        doc = NULL;

        fz_is_open = false;
    }
}

int mupdf_get_page_count()
{
    if (!fz_is_open) {
        return 0;
    }
    return fz_count_pages(doc);
}

bool mupdf_load_page(int pagenumber, int zoom)
{
	fz_rect bounds;
	fz_irect bbox;
	fz_matrix transform;

    if (!fz_is_open) {
        return false;
    }

    if (page) {
    	fz_free_page(doc, page);
        page = NULL;
    }
    if (pix) {
        fz_drop_pixmap(ctx, pix);
        pix = NULL;
    }

    page = fz_load_page(doc, pagenumber);

	fz_rotate(&transform, 0);
	fz_pre_scale(&transform, zoom / 100.0f, zoom / 100.0f);

	fz_bound_page(doc, page, &bounds);
	fz_transform_rect(&bounds, &transform);

	fz_round_rect(&bbox, &bounds);
	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb, &bbox);
	fz_clear_pixmap_with_value(ctx, pix, 0xff);

	dev = fz_new_draw_device(ctx, pix);
	fz_run_page(doc, page, dev, &transform, NULL);
	fz_free_device(dev);

    return true;
}

void mupdf_get_size(int *width, int *height)
{
    if (!fz_is_open){
        return;
    }

	*width= fz_pixmap_width(ctx, pix);
	*height = fz_pixmap_height(ctx, pix);
}

void mupdf_winblit(HDC hdc, int winw, int winh, int cw, int cy)
{
    int image_w, image_h, image_n, x0, y0, x1, y1;
    RECT r;
    unsigned char *samples;

    if (!fz_is_open){
        return;
    }

	image_w = fz_pixmap_width(ctx, pix);
	image_h = fz_pixmap_height(ctx, pix);
	image_n = fz_pixmap_components(ctx, pix);
	samples = fz_pixmap_samples(ctx, pix);
	x0 = (winw - image_w) / 2 + cw;
	y0 = (winh - image_h) / 2 + cy;
	x1 = x0 + image_w;
	y1 = y0 + image_h;

	if (pix)
	{
        /*
		if (gapp.iscopying || justcopied)
		{
			pdfapp_invert(&gapp, &gapp.selr);
			justcopied = 1;
		}

        pdfapp_inverthit(&gapp);
        */

		dibinf->bmiHeader.biWidth = image_w;
		dibinf->bmiHeader.biHeight = -image_h;
		dibinf->bmiHeader.biSizeImage = image_h * 4;

		if (image_n == 2)
		{
			int i = image_w * image_h;
			unsigned char *color = malloc(i*4);
			unsigned char *s = samples;
			unsigned char *d = color;
			for (; i > 0 ; i--)
			{
				d[2] = d[1] = d[0] = *s++;
				d[3] = *s++;
				d += 4;
			}
			SetDIBitsToDevice(hdc,
				x0, y0, image_w, image_h,
				0, 0, 0, image_h, color,
				dibinf, DIB_RGB_COLORS);
			free(color);
		}
		if (image_n == 4)
		{
			SetDIBitsToDevice(hdc,
				x0, y0, image_w, image_h,
				0, 0, 0, image_h, samples,
				dibinf, DIB_RGB_COLORS);
		}

        /*
		pdfapp_inverthit(&gapp);

		if (gapp.iscopying || justcopied)
		{
			pdfapp_invert(&gapp, &gapp.selr);
			justcopied = 1;
		}
        */
	}

	/* Grey background */
	r.top = 0; r.bottom = winh;
	r.left = 0; r.right = x0;
	FillRect(hdc, &r, bgbrush);
	r.left = x1; r.right = winw;
	FillRect(hdc, &r, bgbrush);
	r.left = 0; r.right = winw;
	r.top = 0; r.bottom = y0;
	FillRect(hdc, &r, bgbrush);
	r.top = y1; r.bottom = winh;
	FillRect(hdc, &r, bgbrush);

	/* Drop shadow */
	r.left = x0 + 2;
	r.right = x1 + 2;
	r.top = y1;
	r.bottom = y1 + 2;
	FillRect(hdc, &r, shbrush);
	r.left = x1;
	r.right = x1 + 2;
	r.top = y0 + 2;
	r.bottom = y1;
	FillRect(hdc, &r, shbrush);

	//winblitsearch();
}
