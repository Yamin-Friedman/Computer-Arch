//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_CACHELINE_H
#define WET2_CACHELINE_H

#include<time.h>

class CacheLine {
public:
	CacheLine();
    CacheLine(long int tag,uint32_t data); //TODO: check actual data length
    long int getTag() const {return tag_;};
    bool isValid() const {return validbit_;};
    bool isDirty() const {return dirtybit_;};
    void UpdateTime(){TimeUsed_=time(NULL);};
    time_t& getTime() const {return TimeUsed_;};

private:
    long int tag_, data_;
    bool dirtybit_;
    bool validbit_;
    time_t TimeUsed_; //Last time line was used



};


#endif //WET2_CACHELINE_H
