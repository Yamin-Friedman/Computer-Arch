//
// Created by Omer on 4/23/2019.
//

#include <stdint-gcc.h>
#include "L1.h"

L1::L1(unsigned int mem_cycle, unsigned int bsize, unsigned int L1_size, unsigned int L2_size, unsigned int L1_cycle,
       unsigned int L2_cycle, unsigned int L1_assoc, unsigned int L2_assoc,
       unsigned int wr_alloc, unsigned int victim_cache) : Cache(bsize,L1_size,L1_cycle,L1_assoc), wr_type(wr_alloc),
                                                           L2_(bsize,L2_size,L2_cycle,L2_assoc,mem_cycle,victim_cache,this,wr_type) {
}

//ReadLine: searches for line according to address, if not found seeks from L2 and adds line to L1
void L1::ReadLine(uint32_t address) {
	uint32_t tag = (address >> BSize_);

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		L2_.ReadLine(address); //read the line in L2- This makes sure the line is brought to L2 if it does not exist in it yet.
		CacheLine* Line = L2_.getLine(address); //get the line from L2
		//AddLine(address,CacheLine(tag,0)); //TODO: can delete if ok
		AddLine(address,*Line); //add new line to L1.

	}
    AccessNum_++;
}

//WriteLine: Searches for the line according to address, if found marks dirty, if not then writes to L2 and if we are
// using WRITE_ALLOCATE then also writes the line in the L1.
void L1::WriteLine(uint32_t address){
	long int tag = (address >> BSize_);
	CacheLine *currLine;

	try {
		currLine = getLine(address);
		currLine->markDirty();
		currLine->UpdateTime();
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		L2_.WriteLine(address);
		if (wr_type == WRITE_ALLOCATE) {
		    //get the line from L2 (not marked dirty yet, must come through L2)
		    CacheLine* ToAdd = L2_.getLine(address); //cannot fail because of L2_.Write_Line
			AddLine(address,*ToAdd);
			//need to get line again to update- cannot update ToAdd directly since it is a pointer in L2, copied to L1 at AddLine
			currLine = getLine(address);
			currLine->markDirty();
			currLine->UpdateTime();
		}
	}
	AccessNum_++;
}

//adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with nwLine
void L1::AddLine(uint32_t address, CacheLine nwLine) {
	int set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
    CacheLine* LatestLine = &cache_array_[set];//get from first way
    double timeDiff;
    CacheLine* currLine;
    for (int i=0;i <= (1 << cache_assoc_);i++){
        currLine=&cache_array_[i*set];
        if (!(currLine->isValid())){ //line not valid- can delete instantly and finish
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

    //could not find an empty spot in L1. Evict LRU to L2:

    //if line to be evicted is dirty, mark it dirty and update time in L2 (Write Back + per instructions in forum)
    if(LatestLine->isDirty()){
        try{
            CacheLine* ToEvict = L2_.getLine(set,LatestLine->getTag());
            ToEvict->markDirty();
            ToEvict->UpdateTime(); //TODO: make sure if it should be current time or last recorded saved in L1
        }
        catch (LINE_NOT_FOUND_EXCEPTION) {
            //TODO: since cache in inclusive, we should not reach here- make sure this does not happen (can remove try-catch if ok)
        }
    }

    //write new line to evicted' place
    *LatestLine=nwLine;
	LatestLine->UpdateTime();
}

//This function recieves a line, checks if it exists in the cache- if it does, it evicts it
//TODO: I think this is needed only in L1. If not, can make virtual in Cache Class
void EvictLine(uint32_t address, CacheLine VictimLine){
    int set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);

}
