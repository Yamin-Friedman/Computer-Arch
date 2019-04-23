//
// Created by Omer on 4/23/2019.
//

#include "CacheLine.h"

CacheLine::CacheLine(long int tag, uint32_t data) {
    tag_ = tag;
    data_ = data;
    validbit_ = true;
    dirtybit_ = false;
    TimeUsed_ = time(NULL);
}