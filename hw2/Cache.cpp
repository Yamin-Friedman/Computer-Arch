//
// Created by Yamin on 4/23/2019.
//

#include "Cache.h"



Cache::Cache(unsigned int bsize, unsigned int cache_size, unsigned int cache_cycle,
             unsigned int cache_assoc) : BSize_(bsize), cache_size_(cache_size),
                                         cache_cyc_(cache_cycle), cache_assoc_(cache_assoc), AccessNum_(0), MissNum_(0)
{
	cache_array_ = new CacheLine[NumOfLines];
}

Cache::~Cache(){
	delete[]cache_array_;
}


//getLine: searches for the given tag in the current cache
CacheLine& Cache::getLine(int set, long int tag) {
	CacheLine* currLine;

	for (int i=0;i <= (1 << cache_assoc_) ;i++){
		currLine = &cache_array_[i*set];
		if ((currLine->isValid()) && (tag==(currLine->getTag()))){
			//currLine->UpdateTime();//accessed line- update time
			return *currLine;
		}
	}

	throw LINE_NOT_FOUND_EXCEPTION();

}