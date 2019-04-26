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
	long int tag = (address >> BSize_);

	try {
		getLine(address);
	}
	catch (LINE_NOT_FOUND_EXCEPTION) {
		MissNum_++;
		if (use_victim_cache == USE_VICTIM_CACHE) {
			try{
			    CacheLine* VictimLine = victimCache.getLine(address);
			    AddLine(address,VictimLine);
			}
			catch(LINE_NOT_FOUND_EXCEPTION){ //line was not found in Victim
			    AddLine(address,CacheLine(tag,0));  //"bring" line from memory directly to L2
			}

		}
		else{
		    AddLine(address,CacheLine(tag,0));// the zero is temporary //"bring" line from memory
		}
	}
	AccessNum_++;
}

void L2::WriteLine(uint32_t address){

}

void L2::AddLine(uint32_t address, CacheLine nwLine) {

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
    //could not find an available line- need to evict LRU to Victim/Mem:
    if (use_victim_cache == USE_VICTIM_CACHE) {
        victimCache.addLine(*LatestLine);
    }

    //finally, write new line over evicted line
    *LatestLine=nwLine;
    LatestLine->UpdateTime();
}