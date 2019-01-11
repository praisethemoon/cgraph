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

#endif //CGRAPH_PROJEKT_CG_NET_H
