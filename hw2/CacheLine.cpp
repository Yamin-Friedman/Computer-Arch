//
// Created by Omer on 4/23/2019.
//

#include "CacheLine.h"

CacheLine::CacheLine(long int tag) {
    tag_ = tag;
    validbit_ = true;
    dirtybit_ = false;
    TimeUsed_ = time(NULL);
}

CacheLine::CacheLine() {
	validbit_ = false;
	dirtybit_ = false;
}
