
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

	//ReadLine: searches for line according to address, if not found seeks from victim cache if it exists and adds line to L2.
	void ReadLine(uint32_t address);

	//AddLine: Adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with a line with the new address.
    void AddLine(uint32_t address);

	//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to the victime cache.
	// If we are using WRITE_ALLOCATE then also writes the line in the L2.
	void WriteLine(uint32_t address);

	//GetAvgTime: Returns the total access time in the L2 cahce and victim cache. Later the average is calculated by the L1.
	double GetAvgTime() const;

	Cache* pL1_;

private:
	VictimCache victimCache;
	unsigned int use_victim_cache;
	unsigned int mem_cycle_;
	unsigned int wr_type;
};

#endif //WET2_L2_H
