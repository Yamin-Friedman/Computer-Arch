//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_CACHELINE_H
#define WET2_CACHELINE_H

#include<time.h>
#include <exception>

class LINE_NOT_FOUND_EXCEPTION : public std::exception {
	const char* what() const throw() {
		return "Line not found in cache";
	}
};

class CacheLine {
public:
	CacheLine();
    CacheLine(long int tag,uint32_t data); //TODO: check actual data length. Yamin: not sure we need to save the data as it is not part of the simulation
    long int getTag() const {return tag_;};
    bool isValid() const {return validbit_;};
    bool isDirty() const {return dirtybit_;};
	void markDirty() {dirtybit_ = true;};
	void ChangeValid(bool valid){validbit_=valid;};
    void UpdateTime(){TimeUsed_=time(NULL);};
    const time_t& getTime() const {return TimeUsed_;};

private:
    long int tag_, data_;
    bool dirtybit_;
    bool validbit_;
    time_t TimeUsed_; //Last time line was used. Yamin: Not sure that this is the method we should use



};


#endif //WET2_CACHELINE_H
