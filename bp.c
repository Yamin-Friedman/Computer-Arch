/* 046267 Computer Architecture - Spring 2019 - HW #1 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

#include <stdio.h>
#include <stdlib.h>

Predictor *predictor;

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
	predictor->stats.size = 0;//TODO calc size
	predictor->stats.br_num = 0;
	predictor->stats.flush_num = 0;

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

bool BP_predict(uint32_t pc, uint32_t *dst){

	uint32_t tag = pc >> 2;
	int btb_entry_num = tag % predictor->btb_size;
	BTB_entry *btb_entry = &predictor->btb[btb_entry_num];
	if(!btb_entry->valid_bit || btb_entry->tag != tag){
		//TODO Check about tag size
		*dst = ((tag + 1) << 2);
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
			*dst = ((tag + 1) << 2);
			return false;
		}
		else {
			*dst = btb_entry->target;
			return true;
		}
	}
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){

	uint32_t tag = pc >> 2;
	int btb_entry_num = tag % predictor->btb_size;
	BTB_entry *btb_entry = &predictor->btb[btb_entry_num];

	btb_entry->tag = tag;
	btb_entry->target = targetPc;
	btb_entry->valid_bit = true;


	if (!((taken && targetPc == pred_dst) || (!taken && pred_dst == (pc + 4))))
		predictor->stats.flush_num++;

	int *curr_hist;
	int *curr_state;

	if(predictor->is_global_hist){
		curr_hist = &predictor->global_hist;
	}
	else {
		curr_hist = &btb_entry->history;
	}

	if(predictor->is_global_table){
		uint32_t share_bits;
		switch (predictor->is_share) {
			case 0:
				curr_state = predictor->global_state_array[*curr_hist];
				break;
			case 1:
				share_bits = tag % (1 << predictor->history_size);
				curr_state = predictor->global_state_array[*curr_hist ^ share_bits];
				break;
			case 2:
				share_bits = ((tag  >> 14) % (1 << predictor->history_size));
				curr_state = predictor->global_state_array[*curr_hist ^ share_bits];
				break;
		}
	}
	else {
		curr_state = &btb_entry->state_array[*curr_hist];
	}

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

	return;
}

void BP_GetStats(SIM_stats *curStats){
	curStats->br_num = predictor->stats.br_num;
	curStats->flush_num = predictor->stats.flush_num;
	unsigned size;
	size = predictor->btb_size*(predictor->tag_size + 32 + (!predictor->is_global_hist)*predictor->history_size +
			2*(!predictor->is_global_table)*(1 << predictor->history_size));
	size += (predictor->is_global_hist)*predictor->history_size + 2*(predictor->is_global_table)*(1 << predictor->history_size);
	curStats->size = size;

	int num_states = (1 << predictor->history_size);
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
	return;
}

