#ifndef RM_RID_H
#define RM_RID_H

#include "pf.h"

typedef int SlotNum; // 第几个slot

#define RM_RIDNOTEXIST             (START_RM_ERR - 4)

class RID {

    const int NO_PAGE = -1;
    const int NO_SLOT = -1;

    PageNum pageNum;
    SlotNum slotNum;

public: 
	RID (); // Default constructor 
	~RID (); // Destructor 

	RID (PageNum pageNum, SlotNum slotNum); // Construct RID from page and slot number 
	RC GetPageNum (PageNum &pageNum) const; // Return page number 
	RC GetSlotNum (SlotNum &slotNum) const; // Return slot number

    RID& operator= (const RID &rid) {
        return *this;
    }
};

#endif