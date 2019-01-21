
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include <progressbar.h>

#include "cg_api.h"
#include "cg_enums.h"
#include "cg_types.h"

#define PROFILER_DEFINE
#include "profiler.h"
//#include "cg_math.h"

#include "csv.h"

struct CGNode* sigmoid_node(struct CGNode* x){
    return cg_newBinOp(CGBOT_DIV, cg_newDoubleNode(1.0), cg_newBinOp(CGBOT_ADD, cg_newDoubleNode(1.0), cg_newUnOp(CGUOT_EXP, cg_newUnOp(CGUOT_MINUS, x))));
}


static struct CGNode* softmax_node(struct CGNode* x){
    return cg_newBinOp(CGBOT_DIV, cg_newUnOp(CGUOT_EXP, x), cg_newAxisBoundOp(CGABOT_SUM, cg_newUnOp(CGUOT_EXP, x), 1));
}

// todo: learning rate for every weight
void updateWeight(struct CGraph* graph, char* var, struct CGNode* alpha){
    struct CGNode* dx_var = cg_copyNode(cg_getVarDiff(graph, var));
    struct CGNode* val = cg_copyNode(cg_getVar(graph, var));

    //printf("%zu %zu\n", val, dx_var);

    struct CGNode* newVal = cg_resultToConstantNode(cg_evalRawNode(cg_newBinOp(CGBOT_SUB, val, cg_newBinOp(CGBOT_MULT,cg_copyNode(alpha) , dx_var))));
    cg_setVar(graph, var, newVal);

}

int main(int argc, char* argv[]){
    /*struct CGCPUInfo* info = cg_getCPUInformation();
    if(info == NULL){
        printf("whoupsi\n");
    }
    else
    cg_printCPUInfo(info);*/

    cg_selectContext();


    profiler_initialize();
    PROFILER_START(nn);

    CG_SCALAR_TYPE y_val[] = {1};

    struct CGNode* x = cg_newVariable("x");

    struct CGNode* eval  = cg_newBinOp(CGBOT_MULT, x, cg_newVectorRandNode(1080));

    struct CGraph* graph = cg_newGraph("nn", eval);

    struct CGNode* x_val = cg_newMatrixRandNode(1920,1080);
    cg_setVar(graph, "x", x_val);

    //cg_printNodeValue(x_val);
    struct CGResultNode* res = cg_evalGraph(graph);

    PROFILER_STOP(nn);
    profiler_dump_console();
    return 0;
}
