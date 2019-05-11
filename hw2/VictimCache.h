
#ifndef COMPARCH_VICTIMCACHE_H
#define COMPARCH_VICTIMCACHE_H

#include "CacheLine.h"
#include "Cache.h"

#define VICTIM_CACHE_SIZE 4

class VictimCache {
public:
	VictimCache(unsigned int wr_type_, unsigned int BSize):wr_type(wr_type_), BSize_(BSize){access_num=0;miss_num=0;next_to_push = 0;};
	~VictimCache(){};

	//get_and_remove_Line: Finds and removes a line from the victim cache. If not found throws exception and if it is found
	//returns if the line was dirty or not.
	bool get_and_remove_Line(uint32_t address);

	//addLine: Adds a line to the victim cache. Pushes out old lines in a FIFO order.
	void addLine(uint32_t address,bool is_dirty);

	//getAccessNum: Gets the number of accesses to the victim cache.
	int getAccessNum() const { return access_num;}

	//getMissNum: Gets the number of misses from the victim cache.
	int getMissNum() const { return miss_num;}

	//WriteLine: Accesses the victim cache and if the line is found marks it dirty.
    void WriteLine(uint32_t address);

private:
	int access_num;
	int miss_num;
	unsigned int BSize_;
	int next_to_push;
	CacheLine fifo_cache[VICTIM_CACHE_SIZE];
	unsigned int wr_type;
};


#endif //COMPARCH_VICTIMCACHE_H
