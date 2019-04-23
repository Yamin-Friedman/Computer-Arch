//
// Created by Yamin on 4/23/2019.
//

#ifndef COMPARCH_CACHE_H
#define COMPARCH_CACHE_H


class Cache {
public:
	Cache(int mem_cycle, int bsize, int L1_size, int L2_size, int L1_cycle, int L2_cycle, int L1_assoc, int L2_assoc,
	      int wr_alloc, int victim_cache);

	void Read_Line(unsigned long int address);
	void Write_Line(unsigned long int address);

};

#endif //COMPARCH_CACHE_H
