#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "types.hpp"

#define SEGMENT(__v__) FP_SEG(__v__)
#define OFFSET(__v__) FP_OFF(__v__)
#define POINTER(__seg__,__ofs__) MK_FP(((unsigned int)__seg__),((unsigned int)__ofs__))

bool allocateDosMemory(int size, int &segment);
bool releaseDosMemory(int segment);
uint64_t mapPhysicalToLinear(unsigned int physicalAddress, unsigned int size);
bool unmapPhysical(unsigned int linearAddress);

#endif //__MEMORY_HPP__
