//
// Created by Yamin on 4/23/2019.
//

#ifndef COMPARCH_CACHE_H
#define COMPARCH_CACHE_H

#include <stdint.h>
#include <stdbool.h>
#include "CacheLine.h"


enum WRITE_TYPE {NO_WRITE_ALLOCATE = 0,WRITE_ALLOCATE};
enum VICTIM_USE {USE_VICTIM_CACHE = 0,NOT_USE_VICTIM_CACHE};

class Cache {
public:
	Cache(unsigned int bsize, unsigned int cache_size, unsigned int cache_cycle, unsigned int cache_assoc);
	virtual ~Cache();

	virtual void ReadLine(uint32_t address) = 0;
	virtual void WriteLine(uint32_t address) = 0;

	CacheLine* getLine(uint32_t address);
	int getAccessNum() const { return AccessNum_;}
	int getMissNum() const { return MissNum_;}
	double GetMissRate() const { return ((double)MissNum_ / AccessNum_);};

protected:
	unsigned int cache_size_, cache_cyc_, cache_assoc_, BSize_;
	const unsigned int NumOfLines = (1 << (cache_size_ - BSize_));
	CacheLine* cache_array_;
	int AccessNum_;
	int MissNum_;

};

#endif //COMPARCH_CACHE_H
