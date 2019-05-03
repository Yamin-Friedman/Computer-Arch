//
// Created by Yamin on 4/23/2019.
//

#include "VictimCache.h"


CacheLine* VictimCache::getLine(uint32_t address) {
	uint32_t tag = address >> 2;
	CacheLine currLine,matched_line;
	bool found = false;

	for(int i = 0; i < fifo_cache.size(); i++) {
		currLine = fifo_cache.front();
		fifo_cache.pop();
		if(currLine.getTag() != tag) {
			fifo_cache.push(currLine);
		} else {
			matched_line = currLine;
			found = true;
		}
	}

	access_num++;

	if(found)
		return &matched_line;
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

//AddLine for an existing line evicted from L2 to Victim
void VictimCache::addLine(CacheLine nwLine) {
	if(fifo_cache.size() >= VICTIM_CACHE_SIZE) {
		fifo_cache.pop();
	}
	fifo_cache.push(nwLine);
}

void VictimCache::WriteLine(uint32_t address){
    try{
        CacheLine* CurrLine = getLine(address);
        if (wr_type==NO_WRITE_ALLOCATE){
            CurrLine->markDirty();
        }

    }
    catch(LINE_NOT_FOUND_EXCEPTION){};
}