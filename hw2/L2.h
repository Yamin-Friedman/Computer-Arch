//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_L2_H
#define WET2_L2_H

#include "Cache.h"
#include "CacheLine.h"
#include "VictimCache.h"
#include "L1.h"

class L1; //forward declaration

class L2 : public Cache {
public:
    L2(unsigned int L2size, unsigned int L2Cyc,unsigned int L2Assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache, L1* pL1_, unsigned int wr_type);
    void AddLine(uint32_t address,CacheLine nwLine);
    void ReadLine(uint32_t address);
	void WriteLine(uint32_t address);


private:
	VictimCache victimCache;
	unsigned int use_victim_cache;
	unsigned int mem_cycle_;
	L1* pL1_;
	unsigned int wr_type;


};

#endif //WET2_L2_H
