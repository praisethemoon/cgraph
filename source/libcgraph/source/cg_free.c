

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cblas.h>
#include <string.h> // memcpy
#include <math.h>

#include "cgraph.h"
#include "cg_operation.h"
#include "cg_types.h"
#include "cg_variables.h"
#include "cg_errors.h"
#include "cg_constants.h"
#include "cg_enums.h"
#include "cg_factory.h"
#include "cg_math.h"

#include <malloc.h>


void freeDoubleValue(CGDouble* v){
}

void freeVectorValue(CGVector* data){
	free(data->data);
}

void freeMatrixValue(CGMatrix* data){
	free(data->data);
}

void freeNode(CGraph* graph, CGNode* node){
	if(node->result != NULL){
		freeResultNode(node->result);
		free(node->result);
		node->result = NULL;
	}
	
	if(node->diff != NULL){
		freeNode(graph, node->diff);
		free(node->diff);
	}
	
	switch(node->type){
		case CGNT_CONSTANT:{
			switch(node->constant->type){
				case CGVT_DOUBLE:
				{
					free(node->constant->value);
					free(node->constant);
					
					break;
				}
				
				case CGVT_VECTOR:{
					CGVector* vec = (CGVector*)node->constant->value;
					freeVectorValue(vec);
					free(node->constant->value);
					free(node->constant);
					
					break;
				}
				
				case CGVT_MATRIX:{
					CGMatrix* mat = (CGMatrix*)node->constant->value;
					freeMatrixValue(mat);
					free(node->constant->value);
					free(node->constant);
					break;
				}
			}
			break;
		}
		case CGNT_VARIABLE:{
			free(node->var);
			break;
		}
		case CGNT_BINARY_OPERATION:
			free(node->bop);
			break;
		case CGNT_UNARY_OPERATION:
			free(node->uop);
			break;
		case CGNT_AXIS_BOUND_OPERATION:
			free(node->axop);
			break;
		case CGNT_GRAPH:
			free(node->graph);
			break;
		case CGNT_CROSS_ENTROPY_LOSS_FUNC:
			free(node->crossEntropyLoss);
			break;
	}
	
}

void freeResultNode(CGResultNode* node){
	if(node->error){
		free(node->error);
		free(node);
		return;
	}
	
	switch(node->type){
		case CGVT_DOUBLE:
			freeDoubleValue(node->value);
			break;
		case CGVT_VECTOR:
			freeVectorValue(node->value);
			break;
		case CGVT_MATRIX:
			freeMatrixValue(node->value);
			break;
	}
	
	free(node->value);
}

void freeGraph(CGraph* graph){
	if(graph == NULL)
		return;
	
	const char *key;
	
	map_iter_t iter = map_iter(&graph->vars);

	while ((key = map_next(&graph->vars, &iter))) {
		CGNode* node = *map_get(&graph->vars, key);
		if(node != NULL){
			freeNode(graph, node);
			free(node);
		}
	}
	
	map_deinit(&graph->vars);
	
	int i = 0;
	CGNode* node;
	
	vec_foreach(&graph->nodes, node, i) {
		freeNode(graph, node);
		free(node);
	}
	
	vec_deinit(&graph->nodes);
	// graph pointer must be freed elsewhere. in lua API we create a copy so we cannot free the parameter of this function.
}
