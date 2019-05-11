

#include "VictimCache.h"

//get_and_remove_Line: Finds and removes a line from the victim cache. If not found throws exception and if it is found
//returns if the line was dirty or not.
bool VictimCache::get_and_remove_Line(uint32_t address) {
	uint32_t tag = address >> BSize_;
	CacheLine *currLine;
	bool found = false;
	bool is_dirty = false;

	for(int i = 0; i < VICTIM_CACHE_SIZE; i++) {
		currLine = &fifo_cache[i];
		if(currLine->isValid() && currLine->getTag() == tag) {
			found = true;
			is_dirty = currLine->isDirty();
		}
	}

	access_num++;

	if(found)
		return is_dirty;
	else {
		miss_num++;
		throw LINE_NOT_FOUND_EXCEPTION();
	}
}

//addLine: Adds a line to the victim cache. Pushes out old lines in a FIFO order.
void VictimCache::addLine(uint32_t address,bool is_dirty) {
	uint32_t tag = address >> BSize_;

	fifo_cache[next_to_push] = CacheLine(tag);
	if(is_dirty)
		fifo_cache[next_to_push].markDirty();
	next_to_push = ((next_to_push + 1) % VICTIM_CACHE_SIZE);
}

//WriteLine: Accesses the victim cache and if the line is found marks it dirty.
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