#include "rm_rid.h"

RID::RID () : pageNum(NO_PAGE), slotNum(NO_SLOT) {}

RID::~RID () {}

RID::RID (PageNum pageNum, SlotNum slotNum): pageNum(pageNum), slotNum(slotNum) {}

RC RID::GetPageNum (PageNum &pageNum) const {
	if (this -> pageNum == NO_PAGE) {
		return RM_RIDNOTEXIST;
	}
	pageNum = this -> pageNum;
	return 0;
}

RC RID::GetSlotNum (SlotNum &slotNum) const {
	if (this -> slotNum == NO_SLOT) {
		return RM_RIDNOTEXIST;
	}
	slotNum = this -> slotNum;
	return 0;
}
