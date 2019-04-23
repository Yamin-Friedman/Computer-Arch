//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_L2_H
#define WET2_L2_H

#include "Cache.h"
#include "CacheLine.h"
#include "VictimCache.h"



class L2 : public Cache {
public:
    L2(unsigned int L2size, unsigned int L2Cyc,unsigned int L2Assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache);
    void AddLine(int set,CacheLine& nwLine);
    void ReadLine();


private:
	VictimCache victimCache;
	unsigned int use_victim_cache;
	unsigned int mem_cycle_;


};

#endif //WET2_L2_H