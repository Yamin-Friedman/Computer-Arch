//
// Created by Omer on 4/23/2019.
//

#include <stdint-gcc.h>
#include "L1.h"

L1::L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache) : Cache(bsize,L1_size,L1_cycle,L1_assoc), wr_type(wr_alloc),
                                                           L2_(bsize,L2_size,L2_cycle,L2_assoc,mem_cycle,victim_cache) {
}

//ReadLine: searches for line according to address, if not found seeks from L2 and adds line to L1
void L1::ReadLine(uint32_t address) {
	uint32_t tag = (address >> BSize_);

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		AddLine(address,CacheLine(tag,0));
	}
    AccessNum_++;
}

void L1::WriteLine(uint32_t address){
	long int tag = (address >> BSize_);
	CacheLine *currLine;

	try {
		currLine = getLine(address);
		currLine->markDirty();
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;

		L2_.Write_Line(address);
		if (wr_type == WRITE_ALLOCATE) {
			AddLine(address,CacheLine(tag,0));
			currLine = getLine(address);
			currLine->markDirty();
		}
	}
	AccessNum_++;
}

//adds a new line to the cache. If needed, evicts from cache according to LRU policy
void L1::AddLine(uint32_t address, CacheLine nwLine) {
	int set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
    CacheLine* currLine;
    for (int i=0;i <= (1 << cache_assoc_);i++){
        currLine=&cache_array_[i*set];
        if (!(currLine->isValid())){ //line not valid- can delete instantly
            *currLine = nwLine;
	        currLine->UpdateTime();
            return;
        }
    }

    //could not find an available line- need to evict to L2:

    //first, go over relevant set+ways and choose LRU:
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff;
    for(int i=1;i <= (1 << cache_assoc_);i++){
        currLine=&cache_array_[set*i];
        timeDiff= difftime(LatestLine->getTime(),currLine->getTime());
        if (timeDiff<0){
            LatestLine=currLine;
        }
    }

    //TODO: question: before we evict, do we need to update the evicted line's time in L2 to its time here?
    // TODO: cont: if not then its time in L2 would be the last time asked for by L1, even if it was accessed again here

    //write new line to evicted' place
    *LatestLine=nwLine;
	LatestLine->UpdateTime();
}
