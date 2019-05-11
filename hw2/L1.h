
#ifndef WET2_L1_H
#define WET2_L1_H

#include "Cache.h"
#include "CacheLine.h"
#include "L2.h"
#include "VictimCache.h"



class L1 : public Cache {
public:

    L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache);
    ~L1(){};

	//ReadLine: searches for line according to address, if not found seeks from L2 and adds line to L1.
    void ReadLine(uint32_t address);

	//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to L2 and if we are
	// using WRITE_ALLOCATE then also writes the line in the L1.
	void WriteLine(uint32_t address);

	//AddLine: Adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with a line with the new address.
    void AddLine(uint32_t address);

	//GetL2MissRate: Gets the miss rate from the L2.
	double GetL2MissRate();

	//GetAvgTime: Gets the average access time of the cache.
	double GetAvgTime() const;


private:
    L2 L2_;
	unsigned int wr_type;
};


#endif //WET2_L1_H
