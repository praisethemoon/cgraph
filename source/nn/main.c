
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
    struct CGCPUInfo* info = cg_getCPUInformation();
    if(info == NULL){
        printf("whoupsi\n");
    }
    else
    cg_printCPUInfo(info);

    cg_selectContext();

    profiler_initialize();
    PROFILER_START(nn);

    double y_val[] = {1};

    struct CGNode* x = cg_newVariable("x");
    struct CGNode* y = cg_newVariable("y");
    struct CGNode* T_1 = cg_newVariable("T_1");
    struct CGNode* b_1 = cg_newVariable("b_1");
    struct CGNode* T_2 = cg_newVariable("T_2");
    struct CGNode* b_2 = cg_newVariable("b_2");
    struct CGNode* T_3 = cg_newVariable("T_3");
    struct CGNode* b_3 = cg_newVariable("b_3");

    struct CGNode* L1 = cg_newUnOp(CGUOT_RELU, cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, x, T_1), b_1));
    struct CGNode* L2 = cg_newUnOp(CGUOT_RELU, cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, L1, T_2), b_2));
    struct CGNode* L3 = cg_newUnOp(CGUOT_RELU, cg_newBinOp(CGBOT_ADD, cg_newBinOp(CGBOT_DOT, L2, T_3), b_3));
    struct CGNode* H  = cg_newCrossEntropyLoss((L3), y, 3);

    struct CGNode* eval  = cg_newAxisBoundOp(CGABOT_ARGMAX, L3, 0);

    struct CGraph* graph = cg_newGraph("nn", H);

    cg_setVar(graph, "x", cg_newMatrixRandNode(1, 4));
    cg_setVar(graph, "y", cg_newVectorNode(1, y_val));
    cg_setVar(graph, "T_1", cg_newMatrixRandNode(4, 5));
    cg_setVar(graph, "b_1", cg_newVectorRandNode(5));
    cg_setVar(graph, "T_2", cg_newMatrixRandNode(5, 5));
    cg_setVar(graph, "b_2", cg_newVectorRandNode(5));
    cg_setVar(graph, "T_3", cg_newMatrixRandNode(5, 3));
    cg_setVar(graph, "b_3", cg_newVectorRandNode(3));

    int max_len_size = 1024;
    int i=0;
    double X[100][4] = {0};
    double Y[100][1] = {0};

    double X_test[50][4] = {0};
    double Y_test[50][1] = {0};

    struct csv file;
    char** fields;
    char* error;
    int ret;

    csv_open(&file, "../../datasets/iris/iris-rand-train.csv", ',', 5);

    // skip header
    //csv_read_record(&file, &fields);

    while ((ret = csv_read_record(&file, &fields)) == CSV_OK) {

        if(i<100){
            X[i][0] = atof(fields[0]);
            X[i][1] = atof(fields[1]);
            X[i][2] = atof(fields[2]);
            X[i][3] = atof(fields[3]);

            Y[i][0] = atof(fields[4]);
        }
        else{
            int k = i - 100;
            X_test[k][0] = atof(fields[0]);
            X_test[k][1] = atof(fields[1]);
            X_test[k][2] = atof(fields[2]);
            X_test[k][3] = atof(fields[3]);

            Y_test[k][0] = atof(fields[4]);
        }
        i++;
    }

    if (ret == CSV_END) {
        csv_close(&file);
    }

    else {
        csv_error_string(ret, &error);
        printf("ERROR: %s\n", error);
        csv_close(&file);
        return EXIT_FAILURE;
    }
    progressbar *progress = progressbar_new("Loading",1000);
    for(; i < 1000; i++)
    {
        int j = 0;
        for (j=0;j <100;j++){

            cg_setVar(graph, "x", cg_newMatrixNode(1, 4, X[j]));
            cg_setVar(graph, "y", cg_newVectorNode(1, Y[j]));


            struct CGResultNode* res = cg_evalGraph(graph);

            cg_autoDiffGraph(graph);

            struct CGNode* alpha = cg_newDoubleNode(.03);

            updateWeight(graph, "T_1", alpha);
            updateWeight(graph, "T_2", alpha);
            updateWeight(graph, "T_3", alpha);
            updateWeight(graph, "b_1", alpha);
            updateWeight(graph, "b_2", alpha);
            updateWeight(graph, "b_3", alpha);
        }
        // Do some stuff
        progressbar_inc(progress);
    }
    progressbar_finish(progress);

    printf("Last Error: \n");
    struct CGResultNode* res = cg_evalGraph(graph);


    switch(cg_getResultType(res)){
        case CGVT_DOUBLE:
        {
            CGDouble* d = cg_getResultDoubleVal(res);
            printf("%f\n",  d->value);
            break;
        }

        case CGVT_VECTOR:
        {
            CGVector* vec = cg_getResultVectorVal(res);
            uint64_t i = 0;
            printf("(");
            for(; i < vec->len; i++){
                printf("%f, ", vec->data[i]);
            }
            printf(")\n");
            break;
        }

        case CGVT_MATRIX:
        {
            CGMatrix* m = cg_getResultMatrixVal(res);
            uint64_t i = 0;
            uint64_t j = 0;
            printf("(");
            for(; i < m->rows; i++){
                printf("\n\t");
                for(j = 0; j < m->cols; j++){
                    printf("%f, ", m->data[i*m->cols+j]);
                }
            }
            printf(")\n");
            break;
        }
    }

    uint8_t confusionMat[3][3] = {0};

    for(i = 0; i < 50; i++) {
        cg_setVar(graph, "x", cg_newMatrixNode(1, 4, X_test[i]));
        cg_setVar(graph, "y", cg_newVectorNode(1, Y_test[i]));

        struct CGResultNode *res = cg_evalGraphNode(graph, eval);

        CGDouble *d = cg_getResultDoubleVal(res);
        uint8_t y = (uint8_t)Y_test[i][0];
        uint8_t y_hat = (uint8_t)(d->value);
        confusionMat[y_hat][y]++;
    }


    for(i = 0; i < 3; i++)
        printf("%d\t%d\t%d\n", confusionMat[i][0], confusionMat[i][1], confusionMat[i][2]);

    cg_freeGraph(graph);
    free(graph);
    printf("Training & testing completed.\n");
    PROFILER_STOP(nn);
    profiler_dump_console();
    return 0;
}
