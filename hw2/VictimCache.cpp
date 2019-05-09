//
// Created by Yamin on 4/23/2019.
//

#include "VictimCache.h"


void VictimCache::getLine(uint32_t address) {
	uint32_t tag = address >> 2;
	CacheLine currLine;
	bool found = false;

	for(int i = 0; i < fifo_cache.size(); i++) {
		currLine = fifo_cache.front();
		fifo_cache.pop();

		if(currLine.getTag() == tag){
			found=true;
		}
        fifo_cache.push(currLine);
	}

	access_num++;

	if(found)
		return;
	else {
		miss_num++;
		throw LINE_NOT_FOUND_EXCEPTION();
	}

}

void VictimCache::get_and_remove_Line(uint32_t address) {
	uint32_t tag = address >> 2;
	CacheLine currLine;
	bool found = false;

	for(int i = 0; i < fifo_cache.size(); i++) {
		currLine = fifo_cache.front();
		fifo_cache.pop();
		if(currLine.getTag() != tag) {
			fifo_cache.push(currLine);
		} else {
			found = true;
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
	uint32_t tag = address;
	if(fifo_cache.size() >= VICTIM_CACHE_SIZE) {
		fifo_cache.pop();
	}
	fifo_cache.push(CacheLine(tag));
}


void VictimCache::WriteLine(uint32_t address){
	uint32_t tag = address >> 2;
	CacheLine currLine;

	for(int i = 0; i < fifo_cache.size(); i++) {
		currLine = fifo_cache.front();
		fifo_cache.pop();
		if(currLine.getTag() == tag) {
			if (wr_type==NO_WRITE_ALLOCATE){
				currLine.markDirty();
			}
		}
		fifo_cache.push(currLine);
	}
}