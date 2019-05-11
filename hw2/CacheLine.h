
#ifndef WET2_CACHELINE_H
#define WET2_CACHELINE_H

#include <exception>
#include <stdint.h>

class LINE_NOT_FOUND_EXCEPTION : public std::exception {
	const char* what() const throw() {
		return "Line not found in cache";
	}
};

class CacheLine {
public:

	//Initializes a valid Cacheline with the given tag.
	CacheLine(uint32_t tag);

	//Initializes an invalid Cacheline, this is used to fill the caches when the simulator starts.
	CacheLine();

    ~CacheLine(){};

	//getTag: gets the tag of the Cacheline.
    uint32_t getTag() const {return tag_;};

	//isValid: Checks the valid bit.
    bool isValid() const {return validbit_;};

	//isDirty: Returns if the cacheline is dirty or not.
    bool isDirty() const {return dirtybit_;};

	//markDirty: Marks the cacheline dirty.
	void markDirty() {dirtybit_ = true;};

	//markClean: Marks the cacheline clean.
	void markClean(){dirtybit_=false;};

	//ChangeValid: Changes the state of the valid bit.
	void ChangeValid(bool valid){validbit_=valid;};

	//Used to calculate LRU.
	int time_counter;

private:
    uint32_t tag_;
    bool dirtybit_;
    bool validbit_;

};


#endif //WET2_CACHELINE_H
