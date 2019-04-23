//
// Created by Yamin on 4/23/2019.
//

#ifndef COMPARCH_VICTIMCACHE_H
#define COMPARCH_VICTIMCACHE_H

#include "CacheLine.h"

#define VICTIM_CACHE_SIZE 4

class VictimCache {
public:
	VictimCache();
	CacheLine& getLine(long int tag);

private:
	int access_num; // Number of times the victim cache was accessed
	int next_fill_loc; // Spot in FIFO to place next entry
	CacheLine cache[VICTIM_CACHE_SIZE];


};


#endif //COMPARCH_VICTIMCACHE_H
