#include "fitz.h"
#include <windows.h>

static fz_pixmap *pix = NULL;
static fz_device *dev = NULL;
static fz_context *ctx = NULL;
static fz_document *doc = NULL;
static fz_page *page = NULL;
static int page_num = 0;
static int curr_page = 0;
static BITMAPINFO *dibinf = NULL;
static HBRUSH bgbrush;
static HBRUSH shbrush;

void mupdf_close();
void mupdf_open(char *filename)
{
    mupdf_close();

	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	doc = fz_open_document(ctx, filename);

    page_num = mupdf_get_page_count();

	/* And a background color */
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
}

void mupdf_close()
{
    if (pix){
    	fz_drop_pixmap(ctx, pix);
        pix = NULL;
    }
    if (page) {
    	fz_free_page(doc, page);
        page = NULL;
    }
    if (doc) {
    	fz_close_document(doc);
        doc = NULL;
    }
    if (ctx) {
	    fz_free_context(ctx);
        ctx = NULL;
    }
    if (dibinf) {
        free(dibinf);
        dibinf = NULL;
    }
}

int mupdf_get_page_count()
{
    if (!doc) {
        return 0;
    }
    return fz_count_pages(doc);
}

const fz_pixmap *mupdf_load_page(int pagenumber, int zoom)
{
	fz_rect bounds;
	fz_irect bbox;
	fz_matrix transform;

    if (!doc) {
        return NULL;
    }

    if (page) {
    	fz_free_page(doc, page);
        page = NULL;
    }
    if (pix) {
        fz_drop_pixmap(ctx, pix);
        pix = NULL;
    }

    page = fz_load_page(doc, pagenumber - 1);

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

    return pix;
}

/*
void render(char *filename, int pagenumber, int zoom, int rotation)
{
	// Create a context to hold the exception stack and various caches.
	fz_rect bounds;
	fz_irect bbox;
    fz_pixmap *pix;
    fz_device *dev;
	fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

	// Open the PDF, XPS or CBZ document.

	fz_document *doc = fz_open_document(ctx, filename);

	// Retrieve the number of pages (not used in this example).

	int pagecount = fz_count_pages(doc);

	// Load the page we want. Page numbering starts from zero.

	fz_page *page = fz_load_page(doc, pagenumber - 1);

	// Calculate a transform to use when rendering. This transform
	// contains the scale and rotation. Convert zoom percentage to a
	// scaling factor. Without scaling the resolution is 72 dpi.

	fz_matrix transform;
	fz_rotate(&transform, rotation);
	fz_pre_scale(&transform, zoom / 100.0f, zoom / 100.0f);

	// Take the page bounds and transform them by the same matrix that
	// we will use to render the page.

	fz_bound_page(doc, page, &bounds);
	fz_transform_rect(&bounds, &transform);

	// Create a blank pixmap to hold the result of rendering. The
	// pixmap bounds used here are the same as the transformed page
	// bounds, so it will contain the entire page. The page coordinate
	// space has the origin at the top left corner and the x axis
	// extends to the right and the y axis extends down.

	fz_round_rect(&bbox, &bounds);
	pix = fz_new_pixmap_with_bbox(ctx, fz_device_rgb, &bbox);
	fz_clear_pixmap_with_value(ctx, pix, 0xff);

	// A page consists of a series of objects (text, line art, images,
	// gradients). These objects are passed to a device when the
	// interpreter runs the page. There are several devices, used for
	// different purposes:
	//
	//	draw device -- renders objects to a target pixmap.
	//
	//	text device -- extracts the text in reading order with styling
	//	information. This text can be used to provide text search.
	//
	//	list device -- records the graphic objects in a list that can
	//	be played back through another device. This is useful if you
	//	need to run the same page through multiple devices, without
	//	the overhead of parsing the page each time.

	// Create a draw device with the pixmap as its target.
	// Run the page with the transform.

	dev = fz_new_draw_device(ctx, pix);
	fz_run_page(doc, page, dev, &transform, NULL);
	fz_free_device(dev);

	// Save the pixmap to a file.

	fz_write_png(ctx, pix, "out.png", 0);

	// Clean up.

	fz_drop_pixmap(ctx, pix);
	fz_free_page(doc, page);
	fz_close_document(doc);
	fz_free_context(ctx);
}
*/


void mupdf_winblit(HDC hdc, int winw, int winh)
{
	int image_w = fz_pixmap_width(ctx, pix);
	int image_h = fz_pixmap_height(ctx, pix);
	int image_n = fz_pixmap_components(ctx, pix);
	unsigned char *samples = fz_pixmap_samples(ctx, pix);
	int x0 = 0;
	int y0 = 0;
	int x1 = x0 + image_w;
	int y1 = y0 + image_h;
	RECT r;

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
