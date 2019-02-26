#ifndef CG_PLOT_H
#define CG_PLOT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cgraph.h"

struct CGPlotSettings;

void cg_plot(uint64_t len, cg_float* x, cg_float* y, struct CGPlotSettings* settings, const char* filename);

#endif