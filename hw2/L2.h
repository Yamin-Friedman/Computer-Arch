
#ifndef WET2_L2_H
#define WET2_L2_H

#include "Cache.h"
#include "CacheLine.h"
#include "VictimCache.h"


class L2 : public Cache {
public:
    L2(unsigned int L2size, unsigned int L2Cyc,unsigned int L2Assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache, Cache* pL1_, unsigned int wr_type);
    ~L2(){};
    void AddLine(uint32_t address);
    void ReadLine(uint32_t address);
	void WriteLine(uint32_t address);
	double GetAvgTime() const;

	Cache* pL1_;

private:
	VictimCache victimCache;
	unsigned int use_victim_cache;
	unsigned int mem_cycle_;
	unsigned int wr_type;


};

#endif //WET2_L2_H
