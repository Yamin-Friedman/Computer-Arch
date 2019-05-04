//
// Created by Omer on 4/23/2019.
//

#include "CacheLine.h"

CacheLine::CacheLine(uint32_t tag) {
    tag_ = tag;
    validbit_ = true;
    dirtybit_ = false;
    TimeUsed_ = time(NULL);
	time_counter = 0;
}

CacheLine::CacheLine() {
	validbit_ = false;
	dirtybit_ = false;
	time_counter = 0;
}
