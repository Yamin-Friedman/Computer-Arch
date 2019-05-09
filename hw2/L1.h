//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_L1_H
#define WET2_L1_H

#include "Cache.h"
#include "CacheLine.h"
#include "L2.h"
#include "VictimCache.h"
#include <iostream>

using std::cout;
using std::endl;


class L1 : public Cache {
public:
    L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache);
    ~L1(){};
    void ReadLine(uint32_t adrs);
	void WriteLine(uint32_t address);
    void AddLine(uint32_t address);
    void EvictLine(uint32_t address, CacheLine VictimLine);
	double GetL2MissRate();
	double GetAvgTime() const;


private:
    L2 L2_;
	unsigned int wr_type;



};


#endif //WET2_L1_H
