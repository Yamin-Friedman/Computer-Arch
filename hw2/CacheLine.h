//
// Created by Omer on 4/23/2019.
//

#ifndef WET2_CACHELINE_H
#define WET2_CACHELINE_H

#include<time.h>
#include <exception>
#include <stdint.h>

class LINE_NOT_FOUND_EXCEPTION : public std::exception {
	const char* what() const throw() {
		return "Line not found in cache";
	}
};

class CacheLine {
public:
	CacheLine();
    CacheLine(uint32_t tag);
    ~CacheLine(){};
    uint32_t getTag() const {return tag_;};
    bool isValid() const {return validbit_;};
    bool isDirty() const {return dirtybit_;};
	void markDirty() {dirtybit_ = true;};
	void ChangeValid(bool valid){validbit_=valid;};

	int time_counter;

private:
    uint32_t tag_;
    bool dirtybit_;
    bool validbit_;



};


#endif //WET2_CACHELINE_H
