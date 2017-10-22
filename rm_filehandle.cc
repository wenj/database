#include "rm.h"

RM_FileHandle::RM_FileHandle () : attachedFile(false), fileHandle(null) {}

RM_FileHandle::~RM_FileHandle () {
	attachedFile = true;
}

RC RM_FileHandle::AttachFile(PF_FileHandle &pffh) {
	if (attachedFile) {
		return RM_HANDLEHASOPEN;
	}

	RC rc;
	PF_PageHandle pfph;
	rc = pffh.GetFirstPage(pfph);
	if (rc < 0) {
		return rc;
	}

	char *pData;
	rc = pfph.getData(pData);
	if (rc < 0) {
		return rc;
	}

	memcpy(&fileHeader, pData, sizeof(fileHeader));
	PageNum pageNum;
	rc = pfph.GetPageNum(pageNum);
	if (rc < 0) {
		return rc;
	}
	rc = pffh.UnpinPage(pageNum);
	if (rc < 0) {
		return rc;
	}

	attachedFile = true;
	fileHandle = &pffh;
	return 0;
}

RC RM_FileHandle::GetRec (const RID &rid, RM_Record &rec) const {
	if (!attachedFile) {
		RM_PrintError(rc);
		return RM_HANDLENOTOPEN;
	}

	RC rc;
	PageNum pageNum;
	rc = rid.GetPageNum(pageNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	SlotNum slotNum;
	rc = rid.GetSlotNum(slotNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	PF_PageHandle pfph;
	rc = fileHandle.GetThisPage(pageNum, pfph);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	char *pData;
	rc = pfph.GetDate(pData);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	RM_PageHeader pageHeader;
	memcpy(&pageHeader, pData, sizeof(pageHeader));
	SlotNum slotSum;
	slotSum = pageHeader.slotNum;
	if (slotSum <= slotNum) {
		RM_PrintError(RM_RIDNOTEXIST);
		return rc;
	}

	pData += sizeof(pageHeader) + (slotNum * recordSize);
	rec.LoadData(pData, recordSize, rid);
	return 0;
}

RC RM_FileHandle::InsertRec (const char *pData, RID &rid) {
	if (!attachedFile) {
		RM_PrintError(RM_HANDLENOTOPEN);
		return RM_HANDLENOTOPEN;
	}

	RC rc;
	PageNum pageNum = fileHeader.freePageHead;
	if (pageNum == RM_NULL) {
		PF_PageHandle pfph;
		rc = fileHandle.AllocatePage(pfph);
		if (rc < 0) {
			RM_PrintError(rc);
			return rc;
		}

		rc = pfph.GetPageNum(pageNum);
		if (rc < 0) {
			RM_PrintError(rc);
			return rc;
		}

		RM_PageHeader pageHeader;
		pageHeader.freeSlotNum = (PM_PAGE_SIZE - sizeof(pageHeader)) / recordSize;
		pageHeader.slotNum = 0;
		pageHeader.nextPage = RM_NULL;
		fileHeader.freePageHead = pageNum;
		pfph.UnpinPage(pageNum); 
		UpdateFileHeader(fileHandle, fileHeader);
	}

	PF_PageHandle pfph;
	rc = fileHandle.GetThisPage(pageNum, pfph);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}
	char *data;
	rc = pfph.GetData(data);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}
	RM_PageHeader pageHeader;
	memcpy(&pageHeader, data, sizeof(pageHeader));
	++pageHeader.slotNum;
	--pageHeader.freeSlotNum;
	if (pageHeader.slotNum == 0) {
		fileHeader.freePageHead = pageHeader.nextPage;
		pageHeader.nextPage = RM_NULL;
	}
	memcpy(data, &pageHeader, sizeof(pageHeader));
	pfph.MarkDirty(pageNum);

	data += sizeof(pageHeader) + (pageHeader.slotNum - 1) * recordSize;
	memcpy(data, pData, recordSize);
	pfph.UnpinPage(pageNum);

	UpdateFileHeader(fileHandle, fileHeader);
	return 0;
}

RC RM_FileHandle::DeleteRec (const RID &rid) {
	if (!attachedFile) {
		RM_PrintError(RM_HANDLENOTOPEN);
		return RM_HANDLENOTOPEN;
	}

	RC rc;
	PageNum pageNum;
	rc = rid.GetPageNum(pageNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	SlotNum slotNum;
	rc = rid.GetSlotNum(slotNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	PF_PageHandle pfph;
	rc = fileHandle.GetThisPage(pageNum, pfph);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	char *pdata;
	rc = pfph.GetData(pdata);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	RM_PageHeader pageHeader;
	memcpy(&pageHeader, pdata, sizeof(pageHeader));
	if (pageHeader.slotNum <= slotNum) {
		RM_PrintError(RM_RIDNOTEXIST);
		return RM_RIDNOTEXIST;
	}

	pdata += sizeof(pageHeader);
	char *buffer;
	memcpy(buffer, pdata + (slotNum + 1) * recordSize, sizeof(pageHeader.slotNum - slotNum - 1) * recordSize);
	memcpy(pdata + slotNum * recordSize, buffer, sizeof(pageHeader.slotNum - slotNum - 1) * recordSize);
	pageHeader.slotNum--;
	pageHeader.freeSlotNum++;
	if (pageHeader.freeSlotNum == 1) {
		pageHeader.nextPage = fileHeader.freePageHead;
		fileHeader.freePageHead = pageNum;
		UpdateFileHeader(fileHandle, fileHeader);
	}

	memcpy(pdata, &pageHeader, sizeof(pageHeader));
	fileHandle.MarkDirty(pageNum);
	fileHandle.UnpinPage(pageNum);
	return 0;
}

RC RM_FileHandle::UpdateRec (const RM_Record &rec) {
	if (attachedFile) {
		return RM_HANDLENOTOPEN;
	}
}

RC RM_FileHandle::ForcePages (PageNum pageNum = ALL_PAGES) const {
	if (attachedFile) {
		return RM_HANDLENOTOPEN;
	}
} 