//
// Created by Omer on 4/23/2019.
//

#include "L2.h"

L2::L2(unsigned int L2_size, unsigned int L2_cycle,unsigned int L2_assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache, L1* L1_, unsigned int wr_type) : Cache(BSize,L2_size,L2_cycle,L2_assoc), use_victim_cache(victim_cache),
                                    mem_cycle_(mem_cycle), victimCache(wr_type), pL1_(L1_), wr_type(wr_type){

}

//ReadLine: searches for line according to address, if not found seeks from victim cache if it exists and adds line to L2
void L2::ReadLine(uint32_t address) {
	long int tag = (address >> cache_size_);

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		if (use_victim_cache == USE_VICTIM_CACHE) {
			try{
			    CacheLine* VictimLine = victimCache.getLine(address);
			    AddLine(address,*VictimLine);

			}
			catch(LINE_NOT_FOUND_EXCEPTION){ //line was not found in Victim
			    AddLine(address,CacheLine(tag));  //"bring" line from memory directly to L2
			}

		}
		else{
		    AddLine(address,CacheLine(tag)); //"bring" line from memory
		}
	}
	AccessNum_++;
}

void L2::WriteLine(uint32_t address){
    long int tag = (address >> cache_size_);
    CacheLine *currLine;
    try {
        currLine = getLine(address);
        currLine->UpdateTime();
        if(wr_type == NO_WRITE_ALLOCATE){
            currLine->markDirty();
        }

    }
    catch (LINE_NOT_FOUND_EXCEPTION) {
        MissNum_++;
        if (use_victim_cache == USE_VICTIM_CACHE) victimCache.WriteLine(address);

        if (wr_type == WRITE_ALLOCATE) {
            //must bring line to L2 (but not update as dirty)
            if (use_victim_cache == USE_VICTIM_CACHE) {
                try {
                    CacheLine *VictimLine = victimCache.getLine(address);
                    AddLine(address, *VictimLine);
                }
                catch (LINE_NOT_FOUND_EXCEPTION) {
                    //bring from mem:
                    AddLine(address, CacheLine(tag));
                }
            } else AddLine(address, CacheLine(tag)); //"bring" from mem
        }

    }
    AccessNum_++;

}


void L2::AddLine(uint32_t address, CacheLine nwLine) {

	int set = ((address % (1 << cache_size_)) >> BSize_);
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff;
    CacheLine* currLine;
    for (int i=0;i <= (1 << cache_assoc_);i++){
        currLine=&cache_array_[i*set];
        if (!(currLine->isValid())){ //line not valid- can delete instantly and finish //TODO: here we do not need to check in L1 for eviction
            *currLine = nwLine;
            currLine->UpdateTime();
            return;
        }
        //check if current line has the latest LRU
        timeDiff= difftime(LatestLine->getTime(),currLine->getTime());
        if (timeDiff<0){
            LatestLine=currLine;
        }
    }

    //could not find an available line- need to evict LRU to Victim/Mem:
    //Check if LatestLine is in L1- if so, evict it:
    try{
        CacheLine* L1Evict = pL1_->getLine(set,LatestLine->getTag());
        if(L1Evict->isDirty()){ //snoop:if LatestLine was dirty in L1, write back to L2 before invalidation
            LatestLine->markDirty();
        }
        L1Evict->ChangeValid(false);
    }
    catch(LINE_NOT_FOUND_EXCEPTION){
        //if not in L1- just continue;
    }


    if (use_victim_cache == USE_VICTIM_CACHE) {
        victimCache.addLine(*LatestLine);
    }

    //finally, write new line over evicted line
    *LatestLine=nwLine;
    LatestLine->UpdateTime();
}