/* 046267 Computer Architecture - Spring 2019 - HW #1 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct BTB_entry_ {
	uint32_t tag;
	uint32_t target;
	uint32_t history;
	bool valid_bit;
	int *state_array;

} BTB_entry;

typedef struct Predictor_ {
	int btb_size;
	int history_size;
	int tag_size;
	bool is_global_hist;
	bool is_global_table;
	int is_share;
	int default_state;

	SIM_stats stats;

	BTB_entry *btb;
	uint32_t global_hist;//temp
	int *global_state_array; //temp

} Predictor;

Predictor *predictor;


//TODO delete function before submitting
void print_btb(){

	if(!predictor->is_global_table && !predictor->is_global_hist) {
		for (int i = 0; i < predictor->btb_size; i++) {
			printf("Entry:%d\n", i);
			printf("Tag:0x%x,History:%lu,dest:0x%x\n", predictor->btb[i].tag, predictor->btb[i].history,predictor->btb[i].target);
			for (int j = 0; j < (1 << predictor->history_size); j++) {
				printf("array num:%d,value:%d  ", j, predictor->btb[i].state_array[j]);
			}
			printf("\n");
		}
	}
	else {
		for (int i = 0; i < predictor->btb_size; i++) {
			printf("Entry:%d\n", i);
			printf("Tag:0x%x,History:%lu, dest:0x%x\n", predictor->btb[i].tag, predictor->global_hist,predictor->btb[i].target);
			for (int j = 0; j < (1 << predictor->history_size); j++) {
				printf("array num:%d,value:%d  ", j, predictor->global_state_array[j]);
			}
			printf("\n");
		}
	}
	printf("***********\n");
}

/*
 * BP_init - initialize the predictor
 * all input parameters are set (by the main) as declared in the trace file
 * return 0 on success, otherwise (init failure) return <0
 */
int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){
	predictor = malloc(sizeof(Predictor));
	if(predictor == NULL){
		printf("Malloc error");
		return -1;
	}

	predictor->btb_size = btbSize;
	predictor->history_size = historySize;
	predictor->tag_size = tagSize;
	predictor->is_global_hist = isGlobalHist;
	predictor->is_global_table = isGlobalTable;
	predictor->is_share = Shared;
	predictor->global_hist = 0;
	predictor->stats.size = 0;
	predictor->stats.br_num = 0;
	predictor->stats.flush_num = 0;
	predictor->default_state = fsmState;

	predictor->btb = malloc(btbSize * sizeof(BTB_entry));
	if(predictor->btb == NULL){
		printf("Malloc error");
		free(predictor);
		return -1;
	}
	int num_states = (1 << predictor->history_size);

	if(!predictor->is_global_table) {
		for (int i = 0; i < predictor->btb_size; i++) {
			predictor->btb[i].state_array = malloc(num_states * sizeof(int));
			if (predictor->btb[i].state_array == NULL) {
				printf("malloc error");//TODO Free everything
				return -1;
			}

			for (int j = 0; j < num_states; j++) {
				predictor->btb[i].state_array[j] = fsmState;
			}

			predictor->btb[i].history = 0;
			predictor->btb[i].valid_bit = false;
		}
	}
	else {
		predictor->global_state_array = malloc(num_states * sizeof(int));
		for (int j = 0; j < num_states; j++){
			predictor->global_state_array[j] = fsmState;
		}
	}

	return 0;
}

/*
 * BP_predict - returns the predictor's prediction (taken / not taken) and predicted target address
 * param[in] pc - the branch instruction address
 * param[out] dst - the target address (when prediction is not taken, dst = pc + 4)
 * return true when prediction is taken, otherwise (prediction is not taken) return false
 */
bool BP_predict(uint32_t pc, uint32_t *dst){

	uint32_t tag = (pc >> 2) % (1 << predictor->tag_size);
	int btb_entry_num = tag % predictor->btb_size;
	BTB_entry *btb_entry = &predictor->btb[btb_entry_num];
	if(!btb_entry->valid_bit || btb_entry->tag != tag){
		*dst = pc + 4;
		return false;
	}
	else {
		int curr_hist;
		int curr_state;

		if(predictor->is_global_hist){
			curr_hist = predictor->global_hist;
		}
		else {
			curr_hist = btb_entry->history;
		}

		if(predictor->is_global_table){
			uint32_t share_bits;
			switch (predictor->is_share) {
				case 0:
					curr_state = predictor->global_state_array[curr_hist];
					break;
				case 1:
					share_bits = tag % (1 << predictor->history_size);
					curr_state = predictor->global_state_array[curr_hist ^ share_bits];
					break;
				case 2:
					share_bits = ((tag  >> 14) % (1 << predictor->history_size));
					curr_state = predictor->global_state_array[curr_hist ^ share_bits];
					break;

			}
		}
		else {
			curr_state = btb_entry->state_array[curr_hist];
		}

		if(curr_state < 2){
			*dst = pc + 4;
			return false;
		}
		else {
			*dst = btb_entry->target;
			return true;
		}
	}
}

/*
 * BP_update - updates the predictor with actual decision (taken / not taken)
 * param[in] pc - the branch instruction address
 * param[in] targetPc - the branch instruction target address
 * param[in] taken - the actual decision, true if taken and false if not taken
 * param[in] pred_dst - the predicted target address
 */
void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){

	uint32_t tag = (pc >> 2) % (1 << predictor->tag_size);
	int btb_entry_num = tag % predictor->btb_size;
	BTB_entry *btb_entry = &predictor->btb[btb_entry_num];

	btb_entry->target = targetPc;

	if (!((taken && targetPc == pred_dst) || (!taken && pred_dst == (pc + 4))))
		predictor->stats.flush_num++;

	uint32_t *curr_hist;
	int *curr_state;

    // Resetting the btb entry for a new branch instruction
    if (tag != btb_entry->tag || !btb_entry->valid_bit) {
        btb_entry->tag = tag;
        btb_entry->target = targetPc;
        btb_entry->valid_bit = true;
        if (!predictor->is_global_hist) {
            btb_entry->history = 0;
        }

        if (!predictor->is_global_table) {
            for (int i = 0; i < (1 << predictor->history_size); i++) {
                btb_entry->state_array[i] = predictor->default_state;
            }
        }
    }

	// Get the correct history for the current instruction
	if(predictor->is_global_hist){
		curr_hist = &predictor->global_hist;
	}
	else {
		curr_hist = &btb_entry->history;
	}

	// Get the correct fsm for the instruction given the correct history
	if(predictor->is_global_table){
		uint32_t share_bits;
		switch (predictor->is_share) {
		    case 0:
				curr_state = &predictor->global_state_array[*curr_hist];
				break;
			case 1:
				share_bits = tag % (1 << predictor->history_size);
				curr_state = &predictor->global_state_array[*curr_hist ^ share_bits];
				break;
			case 2:
				share_bits = ((tag  >> 14) % (1 << predictor->history_size));
				curr_state = &predictor->global_state_array[*curr_hist ^ share_bits];
				break;
		}
	}
	else {
		curr_state = &btb_entry->state_array[*curr_hist];
	}


	// Calculating the change in the history and state machine based on the true result of the branch
	if(taken) {
		*curr_state = (*curr_state + 1);
		if(*curr_state > 3) {
			*curr_state = 3;
		}

		*curr_hist = ((*curr_hist << 1) + 1) % (1 << predictor->history_size);

	}
	else {
		*curr_state = (*curr_state - 1);
		if(*curr_state < 0) {
			*curr_state = 0;
		}

		*curr_hist = (*curr_hist << 1) % (1 << predictor->history_size);
	}

	predictor->stats.br_num++;
}

/*
 * BP_GetStats: Return the simulator stats using a pointer
 * curStats: The returned current simulator state (only after BP_update)
 */
void BP_GetStats(SIM_stats *curStats){
	curStats->br_num = predictor->stats.br_num;
	curStats->flush_num = predictor->stats.flush_num;
	unsigned size;
	size = predictor->btb_size*(predictor->tag_size + 30 + (!predictor->is_global_hist)*predictor->history_size +
			2*(!predictor->is_global_table)*(1 << predictor->history_size));
	size += (predictor->is_global_hist)*predictor->history_size + 2*(predictor->is_global_table)*(1 << predictor->history_size);
	curStats->size = size;

	int num_states = (1 << predictor->history_size);

	// Free all of the allocated memory
	if(!predictor->is_global_table) {
		for (int i = 0; i < predictor->btb_size; i++) {
			free(predictor->btb[i].state_array);
		}
	}
	else {
		free(predictor->global_state_array);
	}
	free(predictor->btb);
	free(predictor);
}

