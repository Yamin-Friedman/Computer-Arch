
#include "CacheLine.h"

//Initializes a valid Cacheline with the given tag.
CacheLine::CacheLine(uint32_t tag) {
    tag_ = tag;
    validbit_ = true;
    dirtybit_ = false;
	time_counter = 0;
}

//Initializes an invalid Cacheline, this is used to fill the caches when the simulator starts.
CacheLine::CacheLine() {
	validbit_ = false;
	dirtybit_ = false;
	time_counter = 0;
}
