/* 046267 Computer Architecture - Spring 2019 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>
#include <map>

using std::vector
using std::map


#define MAX_REGISTERS 32

class DepNode {
public:
	DepNode():first_dep(NULL), second_dep(NULL), is_entry(false), instruction_len(0){};
	bool check_is_entry(){ return is_entry;};
	void set_entry_node(){is_entry = true;};


	DepNode *first_dep;
	DepNode *second_dep;
	unsigned int instruction_len;
	bool is_entry;
};

class DflowCtx {
public:
	DflowCtx() {
		entry_node.set_entry_node();
	}


private:
	unsigned int num_of_register_false_dependencies[MAX_REGISTERS] = {0};
	vector<DepNode> dep_nodes;
	map<DepNode*,unsigned int> exit_nodes;
	DepNode entry_node;

};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getRegfalseDeps(ProgCtx ctx, unsigned int reg){
	return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


