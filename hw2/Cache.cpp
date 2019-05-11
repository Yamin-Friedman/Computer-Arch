

#include <iostream>
#include "Cache.h"



Cache::Cache(unsigned int bsize, unsigned int cache_size, unsigned int cache_cycle,
             unsigned int cache_assoc) : BSize_(bsize), cache_size_(cache_size),
                                         cache_cyc_(cache_cycle), cache_assoc_(cache_assoc), AccessNum_(0), MissNum_(0), cache_array_(NumOfLines) {
}

Cache::~Cache(){
}


//getLine: searches for the given address in the current cache. Also updates the LRU time for all the Cachelines in the same set.
CacheLine* Cache::getLine(uint32_t address) {

	uint32_t set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
	uint32_t tag = (address >> (cache_size_ - cache_assoc_));

	CacheLine* currLine = NULL;
	CacheLine* foundLine = NULL;

	for (int i=0;i < (1 << cache_assoc_) ;i++){

		currLine = &cache_array_[set + (i * (NumOfLines / (1 << cache_assoc_)))];
		currLine->time_counter++;//This pushes all the times for this set up one so that the relative differences remain
		if ((currLine->isValid()) && (tag==(currLine->getTag()))){
			currLine->time_counter = 0;
			foundLine = currLine;
		}

	}

	if(foundLine != NULL) {

        return foundLine;
    }
	else
		throw LINE_NOT_FOUND_EXCEPTION();

}