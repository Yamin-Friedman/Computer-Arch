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
	bool need_data_commit;

} Thread_ctx;

Thread_ctx *blocked_thread_ctx_array;
Thread_ctx *fine_thread_ctx_array;
Latency prog_lat;
int blocked_cycle_cnt;
int fine_cycle_cnt;

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
			return 1;
	}
}

Status Core_blocked_Multithreading(){
	int thread_num = Get_thread_number();
	int curr_thread_num = 0;
	blocked_cycle_cnt = 0;
	Thread_ctx *curr_thread_ctx;
	Instuction *curr_inst;

	blocked_thread_ctx_array = (Thread_ctx*)malloc(thread_num * sizeof(Thread_ctx));
	if (blocked_thread_ctx_array == NULL)
		return Failure;
	memset(blocked_thread_ctx_array,0,thread_num * sizeof(Thread_ctx));

	curr_inst = (Instuction*)malloc(sizeof(Instuction));
	if (curr_inst == NULL) {
		free(blocked_thread_ctx_array);
		return Failure;
	}

	Mem_latency((int*)&prog_lat);
	while (1) {
		curr_thread_ctx = &blocked_thread_ctx_array[curr_thread_num];

		if (!curr_thread_ctx->halted) {
			blocked_cycle_cnt++;
			while (1) {
				if (blocked_cycle_cnt - curr_thread_ctx->last_cycle < curr_thread_ctx->curr_inst_cycles) {
					blocked_cycle_cnt++;
					continue;
				}
				if (curr_thread_ctx->need_data_commit) {
					blocked_cycle_cnt++;
					curr_thread_ctx->need_data_commit = false;
					continue;
				}

				SIM_MemInstRead(curr_thread_ctx->curr_inst_addr, curr_inst, curr_thread_num);
				curr_thread_ctx->curr_inst_cycles = perform_op(curr_inst, &curr_thread_ctx->regs);
				curr_thread_ctx->curr_inst_addr++;

				if(curr_inst->opcode == CMD_HALT) {
					curr_thread_ctx->halted = true;
					blocked_cycle_cnt += Get_switch_cycles();
					break;
				}

				if (curr_inst->opcode == CMD_LOAD || curr_inst->opcode == CMD_STORE) {
					curr_thread_ctx->last_cycle = blocked_cycle_cnt;
					curr_thread_ctx->need_data_commit = true;
					blocked_cycle_cnt += Get_switch_cycles();
					break;
				}

				blocked_cycle_cnt++;
			}
		}



		curr_thread_num = (curr_thread_num + 1) % thread_num;

		bool check_halted = blocked_thread_ctx_array[0].halted;
		for (int i = 1; i < thread_num; i++)
			check_halted &= blocked_thread_ctx_array[i].halted;
		if (check_halted)
			break;
	}

	blocked_cycle_cnt -= Get_switch_cycles();
	free(curr_inst);
	return Success;
}


Status Core_fineGrained_Multithreading(){
	int thread_num = Get_thread_number();
	int curr_thread_num = 0;
	fine_cycle_cnt = 0;
	Thread_ctx *curr_thread_ctx;
	Instuction *curr_inst;

	fine_thread_ctx_array = (Thread_ctx*)malloc(thread_num * sizeof(Thread_ctx));
	if (fine_thread_ctx_array == NULL)
		return Failure;
	memset(fine_thread_ctx_array,0,thread_num * sizeof(Thread_ctx));

	curr_inst = (Instuction*)malloc(sizeof(Instuction));
	if (curr_inst == NULL) {
		free(fine_thread_ctx_array);
		return Failure;
	}

	Mem_latency((int*)&prog_lat);

	while (1) {
		curr_thread_ctx = &fine_thread_ctx_array[curr_thread_num];

		if (!curr_thread_ctx->halted)
			fine_cycle_cnt++;

		if (fine_cycle_cnt - curr_thread_ctx->last_cycle >= curr_thread_ctx->curr_inst_cycles && !curr_thread_ctx->halted) {
			if (curr_thread_ctx->need_data_commit) {
				curr_thread_ctx->need_data_commit = false;
			} else {

				SIM_MemInstRead(curr_thread_ctx->curr_inst_addr, curr_inst, curr_thread_num);
				curr_thread_ctx->curr_inst_cycles = perform_op(curr_inst, &curr_thread_ctx->regs);
				curr_thread_ctx->curr_inst_addr++;

				if (curr_inst->opcode == CMD_HALT) {
					curr_thread_ctx->halted = true;
				}

				if (curr_inst->opcode == CMD_LOAD || curr_inst->opcode == CMD_STORE) {
					curr_thread_ctx->last_cycle = fine_cycle_cnt;
					curr_thread_ctx->need_data_commit = true;
				}
			}
		}

		curr_thread_num = (curr_thread_num + 1) % thread_num;

		bool check_halted = fine_thread_ctx_array[0].halted;
		for (int i = 1; i < thread_num; i++)
			check_halted &= fine_thread_ctx_array[i].halted;
		if (check_halted)
			break;
	}

	free(curr_inst);
	return Success;
}


double Core_finegrained_CPI(){
	double  inst_cnt = 0;
	double CPI;

	for (int i = 0; i < Get_thread_number(); i++) {
		inst_cnt += fine_thread_ctx_array[i].curr_inst_addr;
	}

	CPI = (double)fine_cycle_cnt/inst_cnt;
	free(fine_thread_ctx_array);
	return CPI;
}
double Core_blocked_CPI(){
	double  inst_cnt = 0;
	double CPI;

	for (int i = 0; i < Get_thread_number(); i++) {
		inst_cnt += blocked_thread_ctx_array[i].curr_inst_addr;
	}

	CPI = (double)blocked_cycle_cnt/inst_cnt;
	free(blocked_thread_ctx_array);
	return CPI;
}

Status Core_blocked_context(tcontext* bcontext,int threadid){

	if (bcontext == NULL || blocked_thread_ctx_array == NULL)
		return Failure;

	for (int i = 0; i < REGS; i++) {
		(bcontext + threadid)->reg[i] = blocked_thread_ctx_array[threadid].regs.reg[i];
	}

	return Success;
}

Status Core_finegrained_context(tcontext* finegrained_context,int threadid){

	if (finegrained_context == NULL || fine_thread_ctx_array == NULL)
		return Failure;

	for (int i = 0; i < REGS; i++) {
		(finegrained_context + threadid)->reg[i] = fine_thread_ctx_array[threadid].regs.reg[i];
	}

	return Success;
}
