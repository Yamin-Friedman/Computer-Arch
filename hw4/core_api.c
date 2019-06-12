/* 046267 Computer Architecture - Spring 2019 - HW #4 */

#include "core_api.h"
#include "sim_api.h"

#include <stdio.h>


typedef struct thread_ctx_ {
	tcontext regs;
	int curr_inst_addr;
	int curr_inst_cycles;
	int last_cycle;
	bool halted;

} Thread_ctx;

Thread_ctx *thread_ctx_array;
Latency prog_lat;

int perform_op(Instuction *inst, tcontext *t_ctx){

	switch (inst->opcode) {
		case CMD_NOP:
			return 1;
		case CMD_ADD:
			t_ctx->reg[inst->dst_index] = t_ctx->reg[inst->src1_index] + t_ctx->reg[inst->src2_index_imm];
			return 1;
		case CMD_ADDI:
			t_ctx->reg[inst->dst_index] = t_ctx->reg[inst->src1_index] + inst->src2_index_imm;
			return 1;
		case CMD_SUB:
			t_ctx->reg[inst->dst_index] = t_ctx->reg[inst->src1_index] - t_ctx->reg[inst->src2_index_imm];
			return 1;
		case CMD_SUBI:
			t_ctx->reg[inst->dst_index] = t_ctx->reg[inst->src1_index] - inst->src2_index_imm;
			return 1;
		case CMD_LOAD:
			if (inst->isSrc2Imm)
				SIM_MemDataRead(t_ctx->reg[inst->src1_index] + inst->src2_index_imm,&t_ctx->reg[inst->dst_index]);
			else
				SIM_MemDataRead(t_ctx->reg[inst->src1_index] + t_ctx->reg[inst->src2_index_imm],&t_ctx->reg[inst->dst_index]);
			return prog_lat.Load_latecny;
		case CMD_STORE:
			if (inst->isSrc2Imm)
				SIM_MemDataWrite(t_ctx->reg[inst->dst_index] + inst->src2_index_imm, t_ctx->reg[inst->src1_index]);
			else
				SIM_MemDataWrite(t_ctx->reg[inst->dst_index] + t_ctx->reg[inst->src2_index_imm], t_ctx->reg[inst->src1_index]);
			return prog_lat.Store_latency;
		case CMD_HALT:
			return -1;
	}
}

Status Core_blocked_Multithreading(){
	int thread_num = Get_thread_number();
	int curr_thread_num = 0;
	int cycle_cnt = 0;
	Thread_ctx *curr_thread_ctx;
	Instuction *curr_inst;

	thread_ctx_array = (Thread_ctx*)malloc(thread_num * sizeof(Thread_ctx));
	if (thread_ctx_array == NULL)
		return Failure;
	memset(thread_ctx_array,0,thread_num * sizeof(Thread_ctx));

	Mem_latency((int*)&prog_lat);//TODO: Make sure this works.

	while (1) {
		curr_thread_ctx = &thread_ctx_array[curr_thread_num];

		if (cycle_cnt - curr_thread_ctx->last_cycle >= curr_thread_ctx->curr_inst_cycles && !curr_thread_ctx->halted) {
			while (1) {
				SIM_MemInstRead(curr_thread_ctx->curr_inst_addr, curr_inst, curr_thread_num);
				curr_thread_ctx->curr_inst_cycles = perform_op(curr_inst, &curr_thread_ctx->regs);
				curr_thread_ctx->curr_inst_addr++;

				if(curr_thread_ctx->curr_inst_cycles == -1) {
					curr_thread_ctx->halted = true;
					break;
				}

				if (curr_inst->opcode == CMD_LOAD || curr_inst->opcode == CMD_STORE || curr_inst->opcode == CMD_HALT) {
					curr_thread_ctx->last_cycle = cycle_cnt;
					break;
				}

				cycle_cnt++;
			}
		} else {
			cycle_cnt++;
		}



		curr_thread_num = (curr_thread_num + 1) % thread_num;

		bool check_halted = thread_ctx_array[0].halted;
		for (int i = 1; i < thread_num; i++)
			check_halted &= thread_ctx_array[i].halted;
		if (check_halted)
			break;
	}
	return Success;
}


Status Core_fineGrained_Multithreading(){

	return Success;
}


double Core_finegrained_CPI(){
	return 0;
}
double Core_blocked_CPI(){
	return 0;
}

Status Core_blocked_context(tcontext* bcontext,int threadid){
	return Success;
}

Status Core_finegrained_context(tcontext* finegrained_context,int threadid){
	return Success;
}
