
#ifndef COMPARCH_CACHE_H
#define COMPARCH_CACHE_H

#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include "CacheLine.h"



enum WRITE_TYPE {NO_WRITE_ALLOCATE = 0,WRITE_ALLOCATE};
enum VICTIM_USE {NOT_USE_VICTIM_CACHE = 0,USE_VICTIM_CACHE};

class Cache {
public:
	Cache(unsigned int bsize, unsigned int cache_size, unsigned int cache_cycle, unsigned int cache_assoc);
	virtual ~Cache();

	virtual void ReadLine(uint32_t address) = 0;
	virtual void WriteLine(uint32_t address) = 0;

	CacheLine* getLine(uint32_t address);
	double GetMissRate() const { return ((double)MissNum_ / AccessNum_);};

protected:
	unsigned int BSize_,cache_size_, cache_cyc_, cache_assoc_;
	const unsigned int NumOfLines = (1 << (cache_size_ - BSize_));
	std::vector<CacheLine> cache_array_;
	int AccessNum_;
	int MissNum_;
	unsigned int wr_access_num;

};

#endif //COMPARCH_CACHE_H
