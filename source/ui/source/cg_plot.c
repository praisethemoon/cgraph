#include <stdint.h>
#include <stdlib.h>
#include <cairo.h>
#include <sys/types.h>
#include <unistd.h>

#include "kplot.h"
#include "cg_plot.h"
#include "cgraph.h"

typedef struct CGPlotSettings{
    uint32_t width;
    uint32_t height;
    enum kplottype type;
    enum ksmthtype smoothType;
}CGPlotSettings;

CGPlotSettings defaultPlotSettings = {
    .width = 600,
    .height = 400,
    .type = KPLOT_LINES,
    .smoothType = KSMOOTH_MOVAVG
};

void cg_plot(uint64_t len, cg_float* x, cg_float* y, struct CGPlotSettings* settings, const char* filename)
{
    CGPlotSettings* set = settings;
    if(settings == NULL){
        set = &defaultPlotSettings;
    }
	struct kpair* points1 = calloc(len, sizeof(struct kpair));
    
	struct kdata	*d1, *d2;
	struct kplot	*p;
	cairo_surface_t	*surf;
	size_t		 i;
	cairo_t		*cr;
 
	for (i = 0; i < len; i++) {
		points1[i].x = (double)x[i];
		points1[i].y = (double)y[i];
	}

	d1 = kdata_array_alloc(points1, len);

	p = kplot_alloc(NULL);
	
	kplot_attach_data(p, d1, set->type, NULL);
	kdata_destroy(d1);
	
	surf = cairo_image_surface_create
		(CAIRO_FORMAT_ARGB32, set->width, set->height);
	cr = cairo_create(surf);
	cairo_surface_destroy(surf);
	kplot_draw(p, (double)set->width, (double)set->height, cr);
	cairo_surface_write_to_png
		(cairo_get_target(cr), filename);
	cairo_destroy(cr);
	kplot_free(p);
}