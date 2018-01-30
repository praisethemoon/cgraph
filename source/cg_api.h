#ifndef CGRAPH_API_H
#define CGRAPH_API_H

/** \file cg_api.h
 *  \brief External API exposed to users
 *         which provides opaque data types.
 */


#include <stdint.h>

#include "cg_enums.h"
#include "cg_types.h"

#define CGRAPH_API

struct CGNode;
struct CGraph;
struct CGPVariable;
struct CGPConstant;
struct CGBinaryOperation;
struct CGUnaryOperation;
struct CGDouble;
struct CGVector;
struct CGMatrix;
struct CGError;
struct CGResultNode;

/* * * * * * * * * * * *
 * Graph Creation API
 * * * * * * * * * * * */

/**
 * \brief Creates a new graph object, handles memory allocation
 * \param[in] name Graph's name
 * \param[in] root Root node to be bound to the new graph
 * \return New graph object
 */
CGRAPH_API CGraph* cg_newGraph(char* name, CGNode* root);

/**
 * \brief Deallocates graph memory
 * \param[out] graph graph to free, should be NULL after deallocation
 */
CGRAPH_API CGraph* cg_freeGraph(CGraph* graph);


/* * * * * * * * * * * *
 * Node Creation API
 * * * * * * * * * * * */

/**
 * \brief Creates a scalar node
 * \param[in] v value to be bound with the scalar node
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newDoubleNode(double v);

/**
 * \brief Creates a 0 initialized scalar value
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newDouble0Node();

/**
 * \brief Creates a vector node
 * \param[in] len Vector length
 * \param[in] v value to be copied into the vector (deep copy), must be exactly of len-elements
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newVectorNode(uint64_t len, double* v);

/**
 * \brief Creates a 0 initialized vector node
 * \param[in] len Vector length
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newVector0Node(uint64_t len);


/**
 * \brief Creates a Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \param[in] v value to be copied into the vector (deep copy), must be exactly of rows*cols-elements
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newMatrixNode(uint64_t rows, uint64_t cols, double* v);


/**
 * \brief Creates a 0 initialized Matrix node, matrices are ROW Major
 * \param[in] rows Matrix rows
 * \param[in] cols Matrix cols
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newMatrix0Node(uint64_t rows, uint64_t cols);

/**
 * \brief Creates a variable node
 * \param[in] name Variable name
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newVariable(char* name);

/**
 * \brief Creates a binary operation node of the given operation type and bound the given
 *        LHS and RHS nodes
 * \param[in] type Binary operation to use
 * \param[in] lhs left hand side node
 * \param[in] rhs right hand side node
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newBinOp(CGBinaryOperationType type, CGNode* lhs, CGNode* rhs);


/**
 * \brief Creates an unary operation node of the given operation type and bound the given
 *        UHS Node
 * \param[in] type Unary operation to use
 * \param[in] uhs Unary hand side
 * \return Graph node
 */
CGRAPH_API CGNode* cg_newUnOp(CGUnaryOperationType type, CGNode* uhs);

/**
 * \brief Creates a node that is actually an entire graph.
 * \param[in] graph Subgraph, must already have its variable setup.
 * \return Graph node
 */

CGRAPH_API CGNode* cg_newGraphNode(CGraph* graph);

/* * * * * * * * * * * *
 * Graph Processing API
 * * * * * * * * * * * */

/**
 * \brief Sets the value of a variable within the given graph
 * \param[in,out] graph Target graph
 * \param[in] var Variable name to update/set
 * \param[in] value Value to bind with the variable
 */
CGRAPH_API void cg_setVar(CGraph* graph, char* var, CGNode* value);


/**
 * \brief Gets the value of a variable within the given graph
 * \param[in] graph Target graph
 * \param[in] var Variable name to update/set
 * \return Value of the given variable within the graph if it exists, otherwise returns NULL
 */
CGRAPH_API CGNode* cg_getVar(CGraph* graph, char* var);


/**
 * \brief Starts the graph computation recusively from its root node
 * \param[in] graph Graph to evaluate
 * \return Result node that can be evaluated for result or error description in case of exception
 */
CGRAPH_API CGResultNode* cg_evalGraph(CGraph* graph);


/**
 * \brief Returns error information that happened during graph evaluation
 * \param[in] result result node to check
 * \return Error structure, or NULL if no error occured.
 */
CGRAPH_API CGError* cg_getResultError(CGResultNode* result);


/**
 * \brief Returns error type 
 * \param[in] error Error data
 * \return Error type.
 */
CGRAPH_API CGErrorType cg_getErrorType(CGError* error);


/**
 * \brief Returns error message
 * \param[in] error Error data
 * \return Error message
 */
CGRAPH_API const char* cg_getErrorMessage(CGError* error);


/**
 * \brief Returns the parent node that caused the error/where the error occured
 * \param[in] error Error data
 * \return Graph node
 */
CGRAPH_API CGNode* cg_getErrorFaultyNode(CGError* error);


/**
 * \brief Returns the Tensor type of the result node
 * \param[in] result Result node
 * \return Result node type
 */
CGRAPH_API CGVarType cg_getResultType(CGResultNode* result);


/**
 * \brief Returns the Double Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Double tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGDouble* cg_getResultDoubleVal(CGResultNode* result);


/**
 * \brief Returns the Vector Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Vector tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGVector* cg_getResultVectorVal(CGResultNode* result);


/**
 * \brief Returns the Matrix Tensor value of the result node, must check using cg_getResultType before using
 * \param[in] result Result node
 * \return Matrix tensor (not opaque, you better keep mutable however)
 */
CGRAPH_API CGMatrix* cg_getResultMatrixVal(CGResultNode* result);


/**
 * \brief  Derivates a graph with respect to a variable
 * \param[in] graph graph to compute its derivative
 * \param[in] newName Resulting graph's name
 * \param[in] wrtVar With respect to variable name
 * \return Graph, representing the derivative of the original
 */
CGRAPH_API CGraph* cg_diffGraph(CGraph* graph, char* newName, char* wrtVar);


#endif
