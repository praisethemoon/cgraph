
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include <progressbar.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"
#include "cg_net.h"


struct CGNode* sigmoid(struct CGNode* x){
    return cg_newBinOp(CGBOT_DIV, cg_newDoubleNode(1.0), cg_newBinOp(CGBOT_ADD, cg_newDoubleNode(1.0), cg_newUnOp(CGUOT_EXP, cg_newUnOp(CGUOT_MINUS, x))));
}

struct CGNode* sample_h_given_v(struct CGVector* v){
    return sigmoid(cg_newBinOp(CGBOT_ADD, h_bias, cg_newBinOp(CGBOT_DOT, v)));
}

void trainRBM(RBMLayer* rbm, CGMatrix* data, uint8_t batch_size){
    // initializing weights and biases
    CGMatrix* W = cg_newMatrixRandNode(rbm->v, rbm->h);
    CGVector* h_bias = cg_newVectorRandNode(rbm->h);
    CGVector* v_bias = cg_newVectorRandNode(rbm->v);
}

