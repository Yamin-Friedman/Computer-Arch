//
// Created by Yamin on 4/23/2019.
//

#ifndef COMPARCH_VICTIMCACHE_H
#define COMPARCH_VICTIMCACHE_H

#include "CacheLine.h"
#include "Cache.h"
#include <queue>

#define VICTIM_CACHE_SIZE 4

class VictimCache {
public:
	VictimCache(unsigned int wr_type):wr_type_(wr_type){};
	CacheLine getLine(uint32_t address);
	void addLine(uint32_t address);
	void VictimCache::addLine(CacheLine nwLine);
	void addLine(int );
	int getAccessNum() const { return access_num;}
    void WriteLine(uint32_t address);

private:
	int access_num; // Number of times the victim cache was accessed
	std::queue<CacheLine> fifo_cache;
	unsigned int wr_type_;


};


#endif //COMPARCH_VICTIMCACHE_H
