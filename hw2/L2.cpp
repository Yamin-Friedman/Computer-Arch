//
// Created by Omer on 4/23/2019.
//

#include "L2.h"

L2::L2(unsigned int L2_size, unsigned int L2_cycle,unsigned int L2_assoc, unsigned int BSize, unsigned int mem_cycle,
       unsigned int victim_cache) : Cache(BSize,L2_size,L2_cycle,L2_assoc), use_victim_cache(victim_cache),
                                    mem_cycle_(mem_cycle){
	if (use_victim_cache == USE_VICTIM_CACHE)
		victimCache();
}

void L2::ReadLine() {
	int set = (adrs >> BSize_) % (1 << cache_assoc_);
	long int tag = (adrs >> (BSize_ + cache_assoc_));
	CacheLine &CurrLine;

	try {
		CurrLine = getLine(set, tag);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		CurrLine = CacheLine(tag,0);// the zero is temporary
		AddLine(set,CurrLine);
	}
	AccessNum_++;
}

void L2::AddLine(int set, CacheLine& nwLine) {
    CacheLine* currLine;
    for (int i=0;i<=L2Assoc_;i++){
        currLine=CacheArray_[i*set];
        if (!(currLine->isValid())){ //line not valid- can delete instantly
            currLine=&nwLine;
            return;
        }
    }

    //could not find an available line- need to evict to Victim:
    //TODO: complete
}