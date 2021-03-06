
#include <stdint-gcc.h>
#include "L1.h"

L1::L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache) : Cache(bsize,L1_size,L1_cycle,L1_assoc), wr_type(wr_alloc),
		L2_(L2_size,L2_cycle,L2_assoc,bsize,mem_cycle,victim_cache,this,wr_alloc){

}

//ReadLine: searches for line according to address, if not found seeks from L2 and adds line to L1.
void L1::ReadLine(uint32_t address) {

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;

		L2_.ReadLine(address); //read the line in L2- This makes sure the line is brought to L2 if it does not exist in it yet.
		CacheLine* L2Line = L2_.getLine(address);

        AddLine(address);
		CacheLine *currLine = getLine(address);
		if(L2Line->isDirty())
			currLine->markDirty();
		L2Line->markClean();



	}
    AccessNum_++;
}

//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to L2 and if we are
// using WRITE_ALLOCATE then also writes the line in the L1.
void L1::WriteLine(uint32_t address){
	CacheLine *currLine;

	try {
		currLine = getLine(address);
		currLine->markDirty();
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;

		L2_.WriteLine(address);
		if (wr_type == WRITE_ALLOCATE) {

			AddLine(address);
			//need to get line again to update
			try {
				currLine = getLine(address);
			}
			catch(LINE_NOT_FOUND_EXCEPTION) {
			}
			currLine->markDirty();
		}
	}
	AccessNum_++;
}

//AddLine: Adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with a line with the new address.
void L1::AddLine(uint32_t address) {
	uint32_t set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
	uint32_t tag = (address >> (cache_size_ - cache_assoc_));
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff = 0;
    CacheLine* currLine;
    for (int i=0;i < (1 << cache_assoc_);i++){
        currLine = &cache_array_[set + (i * (NumOfLines / (1 << cache_assoc_)))];
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

    //could not find an empty spot in L1. Evict LRU to L2:

    //if line to be evicted is dirty, mark it dirty and update time in L2 (Write Back + per instructions in forum)
    if(LatestLine->isDirty()){
        try{
	        uint32_t LatestLine_address = (LatestLine->getTag() << (cache_size_ - cache_assoc_)) + (set << BSize_);
            CacheLine* ToEvict = L2_.getLine(LatestLine_address);
            ToEvict->markDirty();

        }
        catch (LINE_NOT_FOUND_EXCEPTION) {
        }
    }

    //write new line to evicted' place
    *LatestLine = CacheLine(tag);
	LatestLine->time_counter = 0;
	LatestLine->ChangeValid(true);
}

//GetL2MissRate: Gets the miss rate from the L2.
double L1::GetL2MissRate(){
	return L2_.GetMissRate();
}

//GetAvgTime: Gets the average access time of the cache.
double L1::GetAvgTime() const{
	double time = 0;
	time += AccessNum_ * cache_cyc_;
	time += L2_.GetAvgTime();
	double avgTime = time / AccessNum_;
	return avgTime;
}

