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
	VictimCache(unsigned int wr_type_):wr_type(wr_type_){access_num=0;miss_num=0;};
	~VictimCache(){};
	void getLine(uint32_t address);
	void get_and_remove_Line(uint32_t address);
	void addLine(uint32_t address);
	int getAccessNum() const { return access_num;}
	int getMissNum() const { return miss_num;}
    void WriteLine(uint32_t address);

private:
	int access_num; // Number of times the victim cache was accessed
	int miss_num;
	std::queue<CacheLine> fifo_cache;
	unsigned int wr_type;


};


#endif //COMPARCH_VICTIMCACHE_H
