#ifndef CG_MATH_OPS
#define CG_MATH_OPS

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"


CGResultNode* addDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode);
CGResultNode* addMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* addMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode);
CGResultNode* addMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* addVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* addVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode);
CGResultNode* argmax(CGNode* X, CGraph* graph);
CGResultNode* argmin(CGNode* X, CGraph* graph);
CGResultNode* cosD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* cosM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* cosV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* crossVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode);
CGResultNode* divDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode);
CGResultNode* divDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* divDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* divMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* divMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* divVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* divVV(CGVector* V, CGVector* D, CGraph* graph, CGNode* parentNode);
CGResultNode* dotMM(CGMatrix* M, CGMatrix* N, CGraph* graph, CGNode* parentNode);
CGResultNode* dotMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* dotVM(CGVector* V, CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* dotVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode);
CGResultNode* expD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* expM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* expV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* logD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* logM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* logV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* max(CGNode* X, CGraph* graph);
CGResultNode* mean(CGNode* X, CGraph* graph);
CGResultNode* min(CGNode* X, CGraph* graph);
CGResultNode* mulDD(CGDouble* M, CGDouble* V, CGraph* graph, CGNode* parentNode);
CGResultNode* mulDM(CGDouble* a, CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* mulDV(CGDouble* a, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* mulMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode);
CGResultNode* mulMtV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* mulMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* powDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode);
CGResultNode* powMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* powVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* sinD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* sinM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* sinV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* subDD(CGDouble* D1, CGDouble* D2, CGraph* graph, CGNode* parentNode);
CGResultNode* subDM(CGDouble* D, CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* subDV(CGDouble* D, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* subMD(CGMatrix* M, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* subMM(CGMatrix* M1, CGMatrix* M2, CGraph* graph, CGNode* parentNode);
CGResultNode* subMV(CGMatrix* M, CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* subVD(CGVector* V, CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* subVM(CGVector* V, CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* subVV(CGVector* V1, CGVector* V2, CGraph* graph, CGNode* parentNode);
CGResultNode* sumD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* sumM(CGMatrix* M, CGraph* graph, CGNode* parentNode, uint8_t axis);
CGResultNode* sumV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* tanD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* tanhD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* tanhM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* tanhV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* tanM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* tanV(CGVector* V, CGraph* graph, CGNode* parentNode);
CGResultNode* transposeD(CGDouble* D, CGraph* graph, CGNode* parentNode);
CGResultNode* transposeM(CGMatrix* M, CGraph* graph, CGNode* parentNode);
CGResultNode* transposeV(CGVector* V, CGraph* graph, CGNode* parentNode);

#endif