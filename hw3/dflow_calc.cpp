/* 046267 Computer Architecture - Spring 2019 - HW #3 */
/* Implementation for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>
#include <map>

using std::vector;
using std::map;
using std::pair;

// Maximum number of registers in the processor
#define MAX_REGISTERS 32

// DepNode: A node in the dependency tree.
class DepNode {
public:
	DepNode():first_dep(-1), second_dep(-1), instruction_len(0), is_entry(false){};
	void set_entry_node(){is_entry = true;};


	int first_dep;
	int second_dep;
	unsigned int instruction_len;
	bool is_entry;
};

// DflowCtx: The context for the dflow program.
class DflowCtx {
public:
	DflowCtx() {
		for(int i = 0; i < MAX_REGISTERS; i++)
			last_instruction_write[i] = -1;
	}

	unsigned int num_of_register_false_dependencies[MAX_REGISTERS] = {0};
	int last_instruction_write[MAX_REGISTERS]; // Array of all the last functions to write to each register
	map<unsigned int, DepNode> dep_nodes; // Map of all the instructions in the program and their dependencies
	map<unsigned int, DepNode*> exit_nodes; // Map of all the nodes that are not depended on.

};

// get_node_depth: Recursively calculate the depth of a specific node.
int get_node_depth(DflowCtx *ctx,unsigned int node_num) {
	DflowCtx *dflow_ctx = ctx;
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

/** analyzeProg: Analyze given program and save results
    \param[in] opsLatency An array of MAX_OPS values of functional unit latency for each opcode
               (some entries may be unused - in that case their value would be 0)
    \param[in] progTrace An array of instructions information from execution trace of a program
    \param[in] numOfInsts The number of instructions in progTrace[]
    \returns Analysis context that may be queried using the following query functions or PROG_CTX_NULL on failure */
ProgCtx analyzeProg(const unsigned int opsLatency[], InstInfo progTrace[], int numOfInsts) {
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

		ctx->dep_nodes.insert(pair<unsigned int, DepNode>(i, curr_inst));
		ctx->exit_nodes.insert(pair<unsigned int, DepNode*>(i, &curr_inst));

	}

    return ctx;
}

/** freeProgCtx: Free the resources associated with given program context
    \param[in] ctx The program context to free
*/
void freeProgCtx(ProgCtx ctx) {

	if(ctx == NULL)
		return;

	DflowCtx *curr_ctx = (DflowCtx*)ctx;
	delete curr_ctx;
}

/** getInstDepth: Get the dataflow dependency depth in clock cycles
    Instruction that are direct decendents to the entry node (depend only on Entry) should return 0
    \param[in] ctx The program context as returned from analyzeProg()
    \param[in] theInst The index of the instruction of the program trace to query (the index in given progTrace[])
    \returns >= 0 The dependency depth, <0 for invalid instruction index for this program context
*/
int getInstDepth(ProgCtx ctx, unsigned int theInst) {

	if(ctx == NULL)
		return -1;

	DflowCtx *dflow_ctx = (DflowCtx*)ctx;

	if(theInst < 0 || theInst > dflow_ctx->dep_nodes.size())
		return -1;

	map<unsigned int, DepNode>::iterator it = dflow_ctx->dep_nodes.find(theInst);

	if(it == dflow_ctx->dep_nodes.end())
		return -1;

	int curr_len = it->second.instruction_len;
	return get_node_depth(dflow_ctx,theInst)-curr_len;

}

/** getInstDeps: Get the instructions that a given instruction depends upon
    \param[in] ctx The program context as returned from analyzeProg()
    \param[in] theInst The index of the instruction of the program trace to query (the index in given progTrace[])
    \param[out] src1DepInst Returned index of the instruction that src1 depends upon (-1 if depends on "entry")
    \param[out] src2DepInst Returned index of the instruction that src2 depends upon (-1 if depends on "entry")
    \returns 0 for success, <0 for error (e.g., invalid instruction index)
*/
int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {

	if(ctx == NULL)
		return -1;

	DflowCtx *dflow_ctx = (DflowCtx*)ctx;

	if(theInst < 0 || theInst > dflow_ctx->dep_nodes.size())
		return -1;

	map<unsigned int, DepNode>::iterator it = dflow_ctx->dep_nodes.find(theInst);

	if(it == dflow_ctx->dep_nodes.end())
		return -1;

	*src1DepInst = it->second.first_dep;
	*src2DepInst = it->second.second_dep;

	return 0;

}

/** getRegfalseDeps: Get the number of false dependencies
 *  reg the index of the register
 *  returns the number of times register was found in a false dependency
 */
int getRegfalseDeps(ProgCtx ctx, unsigned int reg){

	if(ctx == NULL)
		return -1;

	DflowCtx *dflow_ctx = (DflowCtx*)ctx;
	return dflow_ctx->num_of_register_false_dependencies[reg];
}

/** getProgDepth: Get the longest execution path of this program (from Entry to Exit)
    \param[in] ctx The program context as returned from analyzeProg()
    \returns The longest execution path duration in clock cycles
*/
int getProgDepth(ProgCtx ctx) {

	if(ctx == NULL)
		return -1;

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


