#include "rid.h"

RID::RID () : pageNum(NULL_PAGE_NUM), slotNum(NULL_SLOT_NUM) {}

RID::~RID () {}

RID::RID (PageNum pageNum, SlotNum slotNum): pageNum(pageNum), slotNum(slotNum) {}

RC RID::GetPageNum (PageNum &pageNum) const {
	if (this.pageNum == NULL_PAGE_NUM) {
		return RM_RIDNONEXIST;
	}
	pageNum = this.pageNum;
	return 0;
}

RC RID::GetSlotNum (SlotNum &slotNum) const {
	if (this.slotNum == NULL_SLOT_NUM) {
		return RM_RIDNONEXIST;
	}
	slotNum = this.slotNum;
	return 0;
}
