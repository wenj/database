#include <cstring>
#include "rm.h"
#include "redbase.h"

RM_Manager::RM_Manager(PF_Manager &pfm) : pfm(pfm) {}

RM_Manager:: ~RM_Manager() {}

RC RM_Manager::CreateFile (const char *fileName, int recordSize) {
	if (recordSize > PF_PAGE_SIZE - sizeof(RM_PageHeader)) {
		RM_PrintError(RM_SIZELIMITEXCEED);
		return RM_SIZELIMITEXCEED;
	}

	if (recordSize < 0) {
		RM_PrintError(RM_SIZEERROR);
		return RM_SIZEERROR;
	}

	RC rc;
	rc = pfm.CreateFile(fileName);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	PF_FileHandle pffh;
	rc = pfm.OpenFile(fileName, pffh);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	PF_PageHandle pfph;
	rc = pffh.AllocatePage(pfph);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	char *pData;
	rc = pfph.GetData(pData);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	RM_FileHeader rmfh;
	memcpy(pData, &rmfh, sizeof(rmfh));

	PageNum pageNum;
	rc = pfph.GetPageNum(pageNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	rc = pffh.MarkDirty(pageNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	rc = pffh.UnpinPage(pageNum);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	rc = pfm.CloseFile(pffh);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	this -> recordSize = recordSize;
	return 0;
}

RC RM_Manager::DestroyFile (const char *fileName) {
	RC rc = pfm.DestroyFile(fileName);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	return 0;
}

RC RM_Manager::OpenFile (const char *fileName, RM_FileHandle &fileHandle) {
	PF_FileHandle pffh;
	RC rc = pfm.OpenFile(fileName, pffh);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	rc = fileHandle.AttachFile(pffh, recordSize);
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}

	return 0;
}

RC RM_Manager::CloseFile (RM_FileHandle &fileHandle) {
	RC rc = fileHandle.UnattachFile();
	if (rc < 0) {
		RM_PrintError(rc);
		return rc;
	}
}