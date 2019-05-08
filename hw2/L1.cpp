//
// Created by Omer on 4/23/2019.
//

#include <stdint-gcc.h>
#include "L1.h"

L1::L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache) : Cache(bsize,L1_size,L1_cycle,L1_assoc), wr_type(wr_alloc),
		L2_(L2_size,L2_cycle,L2_assoc,bsize,mem_cycle,victim_cache,this,wr_alloc){

}

//ReadLine: searches for line according to address, if not found seeks from L2 and adds line to L1
void L1::ReadLine(uint32_t address) {
	uint32_t tag = (address >> (cache_size_ - cache_assoc_));

	try {
		getLine(address);
		//DEBUG
		//std::cout << "L1 hit" << std::endl;
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		//DEBUG
		//std::cout << "L1 miss" << std::endl;
		L2_.ReadLine(address); //read the line in L2- This makes sure the line is brought to L2 if it does not exist in it yet.
		AddLine(address,CacheLine(tag));

	}
    AccessNum_++;
}

//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to L2 and if we are
// using WRITE_ALLOCATE then also writes the line in the L1.
void L1::WriteLine(uint32_t address){
	uint32_t tag = (address >> (cache_size_ - cache_assoc_));
	CacheLine *currLine;

	try {
		currLine = getLine(address);
		currLine->markDirty();
		//DEBUG
		//std::cout << "L1 hit" << std::endl;
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		//DEBUG
		//std::cout << "L1 miss" << std::endl;
		L2_.WriteLine(address);
		if (wr_type == WRITE_ALLOCATE) {

			AddLine(address,CacheLine(tag));
			//need to get line again to update
			try {
				currLine = getLine(address);
			}
			catch(LINE_NOT_FOUND_EXCEPTION) {
				std::cout << "error" << std::endl;
			}
			currLine->markDirty();
		} else {
			wr_access_num++;
		}
	}
	AccessNum_++;
}

//adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with nwLine
void L1::AddLine(uint32_t address, CacheLine nwLine) {
	uint32_t set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff = 0;
    CacheLine* currLine;
//	nwLine.UpdateTime();
    for (int i=0;i < (1 << cache_assoc_);i++){
        currLine = &cache_array_[set + (i * (NumOfLines / (1 << cache_assoc_)))];
        if (!(currLine->isValid())){ //line not valid- can delete instantly and finish
            *currLine = CacheLine(address >> (cache_size_ - cache_assoc_));
//	        currLine->UpdateTime();
	        currLine->time_counter = 0;
            return;
        }
        //check if current line has the latest LRU
//        timeDiff= difftime(LatestLine->getTime(),currLine->getTime());
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
	        //DEBUG
	        //std::cout << "Evict line address:" << LatestLine_address << std::endl;
            CacheLine* ToEvict = L2_.getLine(LatestLine_address);
            ToEvict->markDirty();
	        L2_.getLine(address);

			//TODO: make sure if it should be current time or last recorded saved in L1
        }
        catch (LINE_NOT_FOUND_EXCEPTION) {
	        //DEBUG
	        //cout << "Reached unreachable spot number 1" << endl;
            //TODO: since cache in inclusive, we should not reach here- make sure this does not happen (can remove try-catch if ok)
        }
    }

    //write new line to evicted' place
    *LatestLine = CacheLine(address >> (cache_size_ - cache_assoc_));
//	LatestLine->UpdateTime();
	LatestLine->time_counter = 0;
}

double L1::GetL2MissRate(){
	return L2_.GetMissRate();
}

double L1::GetAvgTime() const{
	double time = 0;
	time += AccessNum_ * cache_cyc_;
	time += L2_.GetAvgTime();
	double avgTime = time / AccessNum_;
	return avgTime;
}

