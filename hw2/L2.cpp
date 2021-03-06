
#include "L2.h"

L2::L2(unsigned int L2_size, unsigned int L2_cycle,unsigned int L2_assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache, Cache* L1_, unsigned int wr_type) : Cache(BSize,L2_size,L2_cycle,L2_assoc), use_victim_cache(victim_cache),
                                    mem_cycle_(mem_cycle), victimCache(wr_type,BSize), pL1_(L1_), wr_type(wr_type){
}

//ReadLine: searches for line according to address, if not found seeks from victim cache if it exists and adds line to L2.
void L2::ReadLine(uint32_t address) {
	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		if (use_victim_cache == USE_VICTIM_CACHE) {
			try{
			    bool is_dirty = victimCache.get_and_remove_Line(address);
			    AddLine(address);
				CacheLine *currLine = getLine(address);
				if(is_dirty)
					currLine->markDirty();
			}
			catch(LINE_NOT_FOUND_EXCEPTION){ //line was not found in Victim
			    AddLine(address);  //"bring" line from memory directly to L2
			}
		}
		else{
		    AddLine(address); //"bring" line from memory
		}
	}
	AccessNum_++;
}

//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to the victime cache.
// If we are using WRITE_ALLOCATE then also writes the line in the L2.
void L2::WriteLine(uint32_t address){
    CacheLine *currLine;
    try {
        currLine = getLine(address);
        if(wr_type == NO_WRITE_ALLOCATE){
            currLine->markDirty();
        }
        else currLine->markClean(); // going to be dirty in L1
    }
    catch (LINE_NOT_FOUND_EXCEPTION) {
        MissNum_++;

        if (wr_type == WRITE_ALLOCATE) {
            //must bring line to L2 (but not update as dirty)
            if (use_victim_cache == USE_VICTIM_CACHE) {
                try {
					victimCache.get_and_remove_Line(address);
                    AddLine(address);
                }
                catch (LINE_NOT_FOUND_EXCEPTION) {
                    //bring from mem:
                    AddLine(address);
                }
            } else AddLine(address); //"bring" from mem
        } else {
	        if (use_victim_cache == USE_VICTIM_CACHE) victimCache.WriteLine(address);
        }
    }
    AccessNum_++;
}

//AddLine: Adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with a line with the new address.
void L2::AddLine(uint32_t address) {

	uint32_t set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
	uint32_t tag = address >> (cache_size_ - cache_assoc_);
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff = 0;
    CacheLine* currLine;

    for (int i=0;i < (1 << cache_assoc_);i++){
        currLine=&cache_array_[set + (i * (NumOfLines / (1 << cache_assoc_)))];
        if (!(currLine->isValid())){ //line not valid- can delete instantly and finish
	        *currLine = CacheLine(tag);
	        currLine->time_counter = 0;
	        currLine->ChangeValid(true);
            return;
        }
        //check if current line has the latest LRU
	    timeDiff = LatestLine->time_counter - currLine->time_counter;
        if (timeDiff < 0){
            LatestLine = currLine;
        }
    }

    //could not find an available line- need to evict LRU to Victim/Mem:
    //Check if LatestLine is in L1- if so, evict it:
	uint32_t LatestLine_address = (LatestLine->getTag() << (cache_size_ - cache_assoc_)) + (set << BSize_);
    try{
        CacheLine* L1Evict = pL1_->getLine(LatestLine_address);
        if(L1Evict->isDirty()){ //snoop:if LatestLine was dirty in L1, write back to L2 before invalidation
            LatestLine->markDirty();
        }
        L1Evict->ChangeValid(false);
    }
    catch(LINE_NOT_FOUND_EXCEPTION){
        //if not in L1- just continue;
    }

    if (use_victim_cache == USE_VICTIM_CACHE) {
        victimCache.addLine(LatestLine_address,LatestLine->isDirty());
    }

    //finally, write new line over evicted line
	*LatestLine = CacheLine(tag);
	LatestLine->time_counter = 0;
	LatestLine->ChangeValid(true);
}

//GetAvgTime: Returns the total access time in the L2 cahce and victim cache. Later the average is calculated by the L1.
double L2::GetAvgTime() const {
	double time = 0;

	time += AccessNum_ * cache_cyc_;
	if(use_victim_cache == USE_VICTIM_CACHE) {
		time += victimCache.getAccessNum();
		time += mem_cycle_ * (victimCache.getMissNum());
	} else {
		time += mem_cycle_ * MissNum_;
	}
	return time;
}