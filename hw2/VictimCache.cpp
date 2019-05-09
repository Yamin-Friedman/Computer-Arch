//
// Created by Yamin on 4/23/2019.
//

#include "VictimCache.h"


void VictimCache::get_and_remove_Line(uint32_t address) {
	uint32_t tag = address >> BSize_;
	CacheLine *currLine;
	bool found = false;

	for(int i = 0; i < VICTIM_CACHE_SIZE; i++) {
		currLine = &fifo_cache[i];

		if(currLine->isValid() && currLine->getTag() == tag) {
			currLine->ChangeValid(false);
			found = true;
		} else {

		}
	}

	access_num++;

	if(found)
		return;
	else {
		miss_num++;
		throw LINE_NOT_FOUND_EXCEPTION();
	}

}

void VictimCache::addLine(uint32_t address) {
	uint32_t tag = address >> BSize_;

	fifo_cache[next_to_push] = CacheLine(tag);
	next_to_push = ((next_to_push + 1) % VICTIM_CACHE_SIZE);
}


void VictimCache::WriteLine(uint32_t address){
	uint32_t tag = address >> BSize_;
	CacheLine *currLine;
	bool found = false;
	access_num++;

	for(int i = 0; i < VICTIM_CACHE_SIZE; i++) {
		currLine = &fifo_cache[i];
		if(currLine->isValid() && currLine->getTag() == tag) {
			if (wr_type==NO_WRITE_ALLOCATE){
				currLine->markDirty();
			}
			found = true;
		}
	}

	if(!found)
		miss_num++;
}