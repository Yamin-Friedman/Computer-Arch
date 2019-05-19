/* 046267 Computer Architecture - Spring 2019 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>
#include <map>

using std::vector
using std::map
using std::pair


#define MAX_REGISTERS 32

class DepNode {
public:
	DepNode():first_dep(-1), second_dep(-1), is_entry(false), instruction_len(0){};
	bool check_is_entry(){ return is_entry;};
	void set_entry_node(){is_entry = true;};


	int first_dep;
	int second_dep;
	unsigned int instruction_len;
	bool is_entry;
};

class DflowCtx {
public:
	DflowCtx() {
		entry_node.set_entry_node();
	}

	unsigned int num_of_register_false_dependencies[MAX_REGISTERS] = {0};
	int last_instruction_write[MAX_REGISTERS] = {-1};
	map<unsigned int, DepNode> dep_nodes;
	map<unsigned int, DepNode*> exit_nodes;
	DepNode entry_node;

};

int get_node_depth(DflowCtx *ctx,unsigned int node_num) {
	DflowCtx *dflow_ctx = (DflowCtx*)ctx;
	int curr_depth = 0;
	int tmp_depth = 0;
	DepNode *curr_node = &dflow_ctx->dep_nodes[node_num];

	if(curr_node->is_entry) {
		return curr_node->instruction_len;
	}

	if(curr_node->first_dep != -1) {
		curr_depth = get_node_depth(ctx,curr_node->first_dep);
	}

	if(curr_node->second_dep != -1) {
		tmp_depth = get_node_depth(ctx,curr_node->second_dep);
		if(tmp_depth > curr_depth)
			curr_depth = tmp_depth;
	}

	return curr_depth + curr_node->instruction_len;
}

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
	int i;
	DflowCtx *ctx = new DflowCtx();

	//Add instructions to graph
	for(i = 0; i < numOfInsts; i++) {
		DepNode curr_inst;
		if(ctx->last_instruction_write[progTrace[i].src1Idx] != -1) {
			curr_inst.first_dep = ctx->last_instruction_write[progTrace[i].src1Idx];
			ctx->exit_nodes.erase((unsigned int)ctx->last_instruction_write[progTrace[i].src1Idx]);
		}
		if(ctx->last_instruction_write[progTrace[i].src2Idx] != -1) {
			curr_inst.second_dep = ctx->last_instruction_write[progTrace[i].src2Idx];
			ctx->exit_nodes.erase((unsigned int)ctx->last_instruction_write[progTrace[i].src2Idx]);
		}

		if(curr_inst.first_dep == -1 && curr_inst.second_dep == -1){
			curr_inst.set_entry_node();
		}

		//make instruction the latest to write to the dst register.
		ctx->last_instruction_write[progTrace[i].dstIdx] = i;

		//Check if there is a false dependency.
		if(i > 0) {
			//WAW
			if(progTrace[i].dstIdx == progTrace[i - 1].dstIdx)
				ctx->num_of_register_false_dependencies[progTrace[i].dstIdx]++;
			//WAR
			else if(progTrace[i].dstIdx == progTrace[i - 1].src1Idx || progTrace[i].dstIdx == progTrace[i - 1].src2Idx)
				ctx->num_of_register_false_dependencies[progTrace[i].dstIdx]++;
		}

		curr_inst.instruction_len = opsLatency[progTrace[i].opcode];

		ctx->dep_nodes.insert(pair(i, curr_inst));
		ctx->exit_nodes.insert(pair(i, &curr_inst));

	}

    return ctx;
}

void freeProgCtx(ProgCtx ctx) {
	DflowCtx *curr_ctx = (DflowCtx*)ctx;
	delete curr_ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
	DflowCtx *dflow_ctx = (DflowCtx*)ctx;

	map<unsigned int, DepNode>::iterator it = dflow_ctx->dep_nodes.find(theInst);

	if(it == dflow_ctx->dep_nodes.end())
		return -1;

	return get_node_depth(dflow_ctx,theInst);

}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
	DflowCtx *dflow_ctx = (DflowCtx*)ctx;

	map<unsigned int, DepNode>::iterator it = dflow_ctx->dep_nodes.find(theInst);

	if(it == dflow_ctx->dep_nodes.end())
		return -1;

	*src1DepInst = it->second.first_dep;
	*src2DepInst = it->second.second_dep;

	return 0;

}

int getRegfalseDeps(ProgCtx ctx, unsigned int reg){
	DflowCtx *dflow_ctx = (DflowCtx*)ctx;
	return dflow_ctx->num_of_register_false_dependencies[reg];
}

int getProgDepth(ProgCtx ctx) {
	DflowCtx *dflow_ctx = (DflowCtx*)ctx;
	map<unsigned int, DepNode*>::iterator it = dflow_ctx->exit_nodes.begin();
	int max_depth = 0;
	int curr_depth = 0;

	for(; it != dflow_ctx->exit_nodes.end(); it++) {
		curr_depth = get_node_depth(dflow_ctx,it->first);
		if(curr_depth > max_depth)
			max_depth = curr_depth;
	}

	return max_depth;
}


