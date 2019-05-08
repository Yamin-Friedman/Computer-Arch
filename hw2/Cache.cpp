//
// Created by Yamin on 4/23/2019.
//

#include <iostream>
#include "Cache.h"



Cache::Cache(unsigned int bsize, unsigned int cache_size, unsigned int cache_cycle,
             unsigned int cache_assoc) : BSize_(bsize), cache_size_(cache_size),
                                         cache_cyc_(cache_cycle), cache_assoc_(cache_assoc), AccessNum_(0), MissNum_(0), wr_access_num(0)
{

    //DEBUG
//    printf("Num of Lines: %d\n",NumOfLines);
    cache_array_ = new CacheLine[NumOfLines];

}

Cache::~Cache(){
	delete[] cache_array_;
}


//getLine: searches for the given tag in the current cache
CacheLine* Cache::getLine(uint32_t address) {

	uint32_t set = ((address % (1 << (cache_size_ - cache_assoc_))) >> BSize_);
	uint32_t tag = (address >> (cache_size_ - cache_assoc_));
    //DEBUG
    //std::cout<<"looking for tag: "<<tag<<std::endl;

//	std::cout << "address:" << address << std::endl;
//	std::cout << "tag:" << tag << std::endl;
//	std::cout << "preset: " << (address % (1 << cache_size_)) << std::endl;
//	std::cout << "set: " << set << std::endl;
	CacheLine* currLine = NULL;
	CacheLine* foundLine = NULL;

	for (int i=0;i < (1 << cache_assoc_) ;i++){
	    //DEBUG
//	    printf("going to access: %d of set %d, out of %d\n",(set + (i * (NumOfLines / (1 << cache_assoc_)))),set,NumOfLines);
		currLine = &cache_array_[set + (i * (NumOfLines / (1 << cache_assoc_)))];
		currLine->time_counter++;//This pushes all the times for this set up one so that the relative differences remain
		if ((currLine->isValid()) && (tag==(currLine->getTag()))){
//			currLine->UpdateTime();//accessed line- update time
			currLine->time_counter = 0;
			foundLine = currLine;
		}

		//DEBUG
		//if(currLine->isValid()) {
			//std::cout << "Curr line address:" << (currLine->getTag() << cache_size_) + (set << BSize_) << "i = " << i << std::endl;
		//}
	}

	if(foundLine != NULL) {
        //DEBUG
        //std::cout << "found the line, curr tag is: " << currLine->getTag() << " and looked for tag: " << tag
                  //<< std::endl;
        return currLine;
    }
	else
		throw LINE_NOT_FOUND_EXCEPTION();

}