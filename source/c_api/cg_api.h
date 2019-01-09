#ifndef CGRAPH_API_H
#define CGRAPH_API_H

/** \file cg_api.h
 *  \brief External API exposed to users
 *         which provides opaque data types.
 */


#include <inttypes.h>

#include "cg_enums.h"
#include "cg_types.h"

#define CGRAPH_API

struct CGNode;
struct CGraph;
struct CGVariable;
struct CGPConstant;
struct CGBinaryOperation;
struct CGUnaryOperation;
struct CGDouble;
struct CGVector;
struct CGMatrix;
struct CGError;
struct CGResultNode;

struct CGCPUInfo;
/* * * * * * * * * * * *
 * Graph Creation API
 * * * * * * * * * * * */

/**
 * \brief Creates a new graph object, handles memory allocation
 * \param[in] name Graph's name
 * \param[in] root Root node to be bound to the new graph
 * \return New graph object
 */
CGRAPH_API struct CGraph* cg_newGraph(char* name, struct CGNode* root);

/**
 * \brief Deallocates graph memory
 * \param[in,out] graph graph to free, should be set freed manually after calling this function (cg_freeGraph(g); free(g))
 */
CGRAPH_API void cg_freeGraph(struct CGraph* graph);

/**
 * \brief Frees a result node. Currently not used.
 * \param[in,out] result result node to deallocate, should be set freed manually after calling this function (cg_freeResultNode(r); free(r))
 */
CGRAPH_API void cg_freeResultNode(struct CGResultNode* result);

/**
 * \brief Frees a node. Currently not used.
 * \param[in,out] result node to deallocate, should be set freed manually after calling this function (cg_freeNode(r); free(r))
 */
CGRAPH_API void cg_freeNode(struct CGraph* graph, struct CGNode* node);



/* * * * * * * * * * * *
 * Node Creation API
 * * * * * * * * * * * */

/**
 * \brief Creates a scalar node
 * \param[in] v value to be bound with the scalar node
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newDoubleNode(double v);

/**
 * \brief Creates a 0 initialized scalar value
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newDouble0Node();


/**
 * \brief Creates a 1 initialized scalar value
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newDouble1Node();


/**
 * \brief Creates a randomly initialized scalar value
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newDoubleRandNode();

/**
 * \brief Creates a vector node
 * \param[in] len Vector length
 * \param[in] v value to be copied into the vector (deep copy), must be exactly of len-elements
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newVectorNode(uint64_t len, double* v);

/**
 * \brief Creates a 0 initialized vector node
 * \param[in] len Vector length
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newVector0Node(uint64_t len);

/**
 * \brief Creates a 1 initialized vector node
 * \param[in] len Vector length
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newVector1Node(uint64_t len);

/**
 * \brief Creates a randomly initialized vector node
 * \param[in] len Vector length
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newVectorRandNode(uint64_t len);


/**
 * \brief Creates a Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \param[in] v value to be copied into the vector (deep copy), must be exactly of rows*cols-elements
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newMatrixNode(uint64_t rows, uint64_t cols, double* v);


/**
 * \brief Creates a 0 initialized Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newMatrix0Node(uint64_t rows, uint64_t cols);

/**
 * \brief Creates a 1 initialized Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newMatrix1Node(uint64_t rows, uint64_t cols);

/**
 * \brief Creates a randomly initialized Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newMatrixRandNode(uint64_t rows, uint64_t cols);



/**
 * \brief Creates a variable node
 * \param[in] name Variable name
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newVariable(char* name);

/**
 * \brief Creates a binary operation node of the given operation type and bound the given
 *        LHS and RHS nodes
 * \param[in] type Binary operation to use
 * \param[in] lhs left hand side node
 * \param[in] rhs right hand side node
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newBinOp(CGBinaryOperationType type, struct CGNode* lhs, struct CGNode* rhs);


/**
 * \brief Creates an unary operation node of the given operation type and bound the given
 *        UHS Node
 * \param[in] type Unary operation to use
 * \param[in] uhs Unary hand side
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newUnOp(CGUnaryOperationType type, struct CGNode* uhs);

/**
 * \brief Creates an axis bound operator that performs operations along the specified axis, 
 * note that axis starts from 0 for the first dimension
 * \param[in] type Operation type
 * \param[in] uhs Unary hand side to sum
 * \param[in] axis axis to sum
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newAxisBoundOp(CGAxisBoundOperationType type, struct CGNode* uhs, uint8_t axis);

/**
 * \brief Create a cross entropy loss function
 * \param[in] x output of softmax
 * \param[in] y classes, should *not* be one hot vectors, but rather integers from 0 to num_classes-1
 * \param[in] num_classes number of classes to consider.
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_newCrossEntropyLoss(struct CGNode* x, struct CGNode* y, uint64_t num_classes);

/**
 * \brief Creates a node that is actually an entire graph.
 * \param[in] graph Subgraph, must already have its variable setup.
 * \return Graph node
 */

CGRAPH_API struct CGNode* cg_newGraphNode(struct CGraph* graph);

/* * * * * * * * * * * *
 * Graph Processing API
 * * * * * * * * * * * */

/**
 * \brief Sets the value of a variable within the given graph
 * \param[in,out] graph Target graph
 * \param[in] var Variable name to update/set
 * \param[in] value Value to bind with the variable
 */
CGRAPH_API void cg_setVar(struct CGraph* graph, const char* var, struct CGNode* value);


/**
 * \brief Gets the value of a variable within the given graph. If the graph already has a variable, it will freed and replaced with the new one.
 * \param[in] graph Target graph
 * \param[in] var Variable name to update/set
 * \return Value of the given variable within the graph if it exists, otherwise returns NULL
 */
CGRAPH_API struct CGNode* cg_getVar(struct CGraph* graph, const char* var);


/**
 * \brief Unsets the variable from the graph, i.e removes it but it does not free it.
 * \param[in] graph Target graph
 * \param[in] var Variable name to unset
 */
CGRAPH_API void graphUnsetVar(struct CGraph* graph, const char* name);
/**
 * \brief Starts the graph computation recusively from its root node
 * \param[in] graph Graph to evaluate
 * \return Result node that can be evaluated for result or error description in case of exception
 */
CGRAPH_API struct CGResultNode* cg_evalGraph(struct CGraph* graph);



CGRAPH_API struct CGResultNode* cg_evalGraphNode(struct CGraph* graph, struct CGNode* node);

/**
 * \brief Returns error information that happened during graph evaluation
 * \param[in] result result node to check
 * \return Error structure, or NULL if no error occured.
 */
CGRAPH_API struct CGError* cg_getResultError(struct CGResultNode* result);


/**
 * \brief Returns error type 
 * \param[in] error Error data
 * \return Error type.
 */
CGRAPH_API CGErrorType cg_getErrorType(struct CGError* error);


/**
 * \brief Returns error message
 * \param[in] error Error data
 * \return Error message
 */
CGRAPH_API const char* cg_getErrorMessage(struct CGError* error);


/**
 * \brief Returns the parent node that caused the error/where the error occured
 * \param[in] error Error data
 * \return Graph node
 */
CGRAPH_API struct CGNode* cg_getErrorFaultyNode(struct CGError* error);


/**
 * \brief Returns the Tensor type of the result node
 * \param[in] result Result node
 * \return Result node type
 */
CGRAPH_API CGVarType cg_getResultType(struct CGResultNode* result);


/**
 * \brief Returns the Double Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Double tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGDouble* cg_getResultDoubleVal(struct CGResultNode* result);


/**
 * \brief Returns the Vector Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Vector tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGVector* cg_getResultVectorVal(struct CGResultNode* result);


/**
 * \brief Returns the Matrix Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Matrix tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGMatrix* cg_getResultMatrixVal(struct CGResultNode* result);

/**
 * \brief Derivates a graph with respect to final output, usually the loss
 * \param[in,out] graph Graph to derive, fills in `diff` field of every node.
 */
CGRAPH_API void cg_autoDiffGraph(struct CGraph* graph);

/**
 * \brief Returns the derivative of a given node variable's name with respect to the parent node, must have already called `cg_diffGraph` before usage
 * \param[in] graph Original graph
 * \param[in] name variable name to return
 * \return CGNode*, a constant node representing the derivative. Unlike forward mode which returns a CGResultNode, this one returns a node.
 */
CGRAPH_API struct CGNode* cg_getVarDiff(struct CGraph* graph, const char*  name);

/**
 * \brief Prints a given node, or rather, dumps it. Works only with constant node values.
 * \param[in] node node to print.
 */
CGRAPH_API struct CGNode* cg_printNodeValue(struct CGNode* node);

/**
 * \brief Converts a CGResultNode to a constant CGNode
 * \param[in] result CGResultNode to convert
 * \return CGNode version of the given result
 */
CGRAPH_API struct CGNode* cg_resultToConstantNode(struct CGResultNode* result);


/**
 * \brief Converts a constant CGNode to a CGResultNode
 * \param[in] node CGNode to convert
 * \return CGResultNode version of the result
 */
CGRAPH_API struct CGResultNode* cg_constantToResult(struct CGNode* node);


#ifdef CG_USE_LIBCPUID
struct CGCPUInfo* getCPUInformation();
void printCPUInfo(struct CGCPUInfo* cpuInfo);
#endif

#endif
