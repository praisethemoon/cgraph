//
// Created by praisethemoon on 09/01/2019.
//

#ifndef CGRAPH_PROJEKT_CG_NET_H
#define CGRAPH_PROJEKT_CG_NET_H

#include <stdlib.h>
#include <stdint.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

typedef struct CGTensorShape{
    uint8_t ndims;
}CGTensorShape;

typedef struct CGNNInput{
    uint64_t len;
};

typedef struct CGNNDense {

};

typedef struct RBMLayer {
    uint64_t v;
    uint64_t h;

    cg_float* W;
    cg_float* vbias;
    cg_float* hbias;
}RBMLayer;

void trainRBM(RBMLayer* rbm, CGMatrix* data, uint8_t batch_size);

#endif //CGRAPH_PROJEKT_CG_NET_H
