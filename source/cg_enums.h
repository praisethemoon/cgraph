#ifndef CGRAPH_ENUMS_H
#define CGRAPH_ENUMS_H


/**
 * \brief List of errors that can occure during computations
 */
typedef enum CGErrorType {
	/**
	 * \brief incompatible arguments given i.e device a vector by a matrix, 
	 *        like seriously .. what do you think you are doing?
	 */
	CGET_INCOMPATIBLE_ARGS_EXCEPTION = 0,
	
	/**
	 * \brief arguments' types are fine, yet their dimentions are not compatible.
	 */
	CGET_INCOMPATIBLE_DIMENTIONS_EXCEPTION,
	
	/**
	 * \brief double overflow exception
	 */
	CGET_OVERFLOW_EXCEPTION,
	
	/**
	 * \brief Variable not present in the current graph
	 */
	CGET_VARIABLE_DOES_NOT_EXIST,
	
	/**
	 * \brief Operation is not yet supported.
	 */
	CGET_OPERATION_NOT_IMPLEMENTED,
	
	/**
	 * \brief Matrix is not invertible
	 */
	CGET_MATRIX_NOT_INVERTIBLE,
	
	/**
	 * \brief Did you just yourself in the foot?
	 */
	CGET_DIVIDE_BY_ZERO,
	
}CGErrorType;
#define MAX_ERROR_TYPE CGET_DIVIDE_BY_ZERO

/**
 * \brief List of the available binary operation types
 */
typedef enum CGBinaryOperationType {
	CGBOT_ADD=0,
	CGBOT_SUB,
	CGBOT_MULT,
	CGBOT_DIV,
	CGBOT_POW,
	CGBOT_DOT,
	CGBOT_TMULT, // transpose + mult
} CGBinaryOperationType;
#define MAX_BINARY_OPERATION CGBOT_TMULT

/**
 * \brief List of the available binary operation types
 */
typedef enum CGUnaryOperationType {
	CGUOT_MINUS=0,
	CGUOT_INV,
	CGUOT_TRANSPOSE,
	CGUOT_EXP,
	CGUOT_LOG,
	CGUOT_SIN,
	CGUOT_COS,
	CGUOT_TAN,
	CGUOT_TANH,
	CGUOT_SUM,
} CGUnaryOperationType;

#define MAX_UNARY_OPERATION CGUOT_SUM

/**
 * \brief List of the available node types
 */
typedef enum CGNodeType {
	CGNT_VARIABLE = 0,
	CGNT_CONSTANT,
	CGNT_GRAPH,
	CGNT_BINARY_OPERATION,
	CGNT_UNARY_OPERATION
}CGNodeType;
#define MAX_NODE_TYPE CGNT_UNARY_OPERATION

/**
 * \brief List of the available tensor types
 */
typedef enum CGVarType {
	CGVT_DOUBLE=0,
	CGVT_VECTOR,
	CGVT_MATRIX,
}CGVarType;
#define MAX_VAR_TYPE CGVT_MATRIX

/**
 * \brief Matrix shape, row major by default and it is not changeable for now, probably a future feature.
 */
typedef enum CGMShape {
	CGMS_ROW_MAJOR=0,
	CGMS_COL_MAJOR
}CGMShape;

#endif
