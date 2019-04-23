//
// Created by Omer on 4/23/2019.
//

#include "L2.h"

L2::L2(unsigned int L2_size, unsigned int L2_cycle,unsigned int L2_assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache) : Cache(BSize,L2_size,L2_cycle,L2_assoc), use_victim_cache(victim_cache),
                                    mem_cycle_(mem_cycle), victimCache(){

}

//ReadLine: searches for line according to address, if not found seeks from victim cache if it exists and adds line to L2
void L2::ReadLine(uint32_t address) {
	long int tag = (address >> (BSize_ + cache_assoc_));

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		if (use_victim_cache == USE_VICTIM_CACHE) {
			//TODO check victim cache
		}
		AddLine(address,CacheLine(tag,0));// the zero is temporary
	}
	AccessNum_++;
}

void L2::AddLine(uint32_t address, CacheLine nwLine) {
	int set = (address >> BSize_) % (1 << cache_assoc_);
    CacheLine* currLine;

    for (int i=0;i <= (1 << cache_assoc_);i++){
        currLine=&cache_array_[i*set];
        if (!(currLine->isValid())){ //line not valid- can delete instantly
            *currLine = nwLine;
	        currLine->UpdateTime();
            return;
        }
    }

    //could not find an available line- need to evict to Victim:
    //TODO: complete
}