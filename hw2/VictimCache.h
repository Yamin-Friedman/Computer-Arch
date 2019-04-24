//
// Created by Yamin on 4/23/2019.
//

#ifndef COMPARCH_VICTIMCACHE_H
#define COMPARCH_VICTIMCACHE_H

#include "CacheLine.h"
#include <queue>

#define VICTIM_CACHE_SIZE 4

class VictimCache {
public:
	VictimCache();
	CacheLine getLine(uint32_t address);
	void addLine(uint32_t address);
	int getAccessNum() const { return access_num;}

private:
	int access_num; // Number of times the victim cache was accessed
	std::queue<CacheLine> fifo_cache;


};


#endif //COMPARCH_VICTIMCACHE_H
