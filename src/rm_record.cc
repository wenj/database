#include "rm.h"

RM_Record::RM_Record() : pdata(nullptr), rid(RID()) {}

RM_Record::~RM_Record () {
    pdata = nullptr;
}

RC RM_Record::GetData (char *&pData) const {
    if (pdata == nullptr) {
        return RM_NORECORD;
    }
    pData = pdata;
    return 0;
}

RC RM_Record::GetRid (RID &rid) const {
	if (pdata == nullptr) {
        return RM_NORECORD;
    }
    rid = this -> rid;
    return 0;
}

RC RM_Record::LoadData(char *&pData, int recordSize, RID rid) {
    pdata = pData;
    this -> recordSize = recordSize;
    this -> rid = rid;
}