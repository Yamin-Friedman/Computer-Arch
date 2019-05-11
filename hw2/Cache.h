
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

	//ReadLine: Reads the given address from the current cache.
	virtual void ReadLine(uint32_t address) = 0;

	//WriteLine: Writes the given address to the current cache.
	virtual void WriteLine(uint32_t address) = 0;

	//AddLine: Adds a new line to the cache. If needed, evicts an existing line from cache according to LRU policy and replaces with a line with the new address.
	virtual void AddLine(uint32_t address) = 0;

	//getLine: searches for the given address in the current cache. Also updates the LRU time for all the Cachelines in the same set.
	CacheLine* getLine(uint32_t address);
	double GetMissRate() const { return ((double)MissNum_ / AccessNum_);};

protected:
	unsigned int BSize_,cache_size_, cache_cyc_, cache_assoc_;
	const unsigned int NumOfLines = (1 << (cache_size_ - BSize_));
	std::vector<CacheLine> cache_array_;
	int AccessNum_;
	int MissNum_;

};

#endif //COMPARCH_CACHE_H
