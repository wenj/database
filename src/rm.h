#ifndef RM_H
#define RM_H

#include <cerrno>
#include <iostream>
#include "pf.h"
#include "rm_rid.h"

class RM_Manager;
class RM_FileHandle;
class RM_FileScan;
class RM_Record;

struct RM_FileHeader {
    PageNum freePageHead;
};

struct RM_PageHeader {
    SlotNum slotNum;
    SlotNum freeSlotNum;
    PageNum nextPage;
};

class RM_Manager {

private:
	PF_Manager &pfm;
    int recordSize;

public:
	RM_Manager (PF_Manager &_pfm); // Constructor 
	~RM_Manager (); // Destructor 
	
	RC CreateFile (const char *fileName, int recordSize); // Create a new file 
	RC DestroyFile (const char *fileName); // Destroy a file 
	RC OpenFile (const char *fileName, RM_FileHandle &fileHandle); // Open a file 
	RC CloseFile (RM_FileHandle &fileHandle); // Close a file
};

class RM_FileHandle { 

private:
	bool attachedFile;
	PF_FileHandle *fileHandle;
	RM_FileHeader fileHeader;
    int recordSize;

public: 
	RM_FileHandle (); // Constructor 
	~RM_FileHandle (); // Destructor 

	RC AttachFile(PF_FileHandle &pffh, int recordSize);
	RC UnattachFile();
    RC UpdateFileHeader();

	RC GetRec (const RID &rid, RM_Record &rec) const; // Get a record 
	RC InsertRec (const char *pData, RID &rid); // Insert a new record, return record id 
	RC DeleteRec (const RID &rid); // Delete a record 
	RC UpdateRec (const RM_Record &rec); // Update a record 
	RC ForcePages (PageNum pageNum) const; // Write dirty page(s) to disk
};

class RM_FileScan { 

public: 
	RM_FileScan (); // Constructor 
	~RM_FileScan (); // Destructor 

	RC OpenScan (const RM_FileHandle &fileHandle, // Initialize file scan 
				 AttrType attrType, 
				 int attrLength, 
				 int attrOffset, 
				 CompOp compOp, 
				 void *value, 
				 ClientHint pinHint = NO_HINT); 
	RC GetNextRec (RM_Record &rec); // Get next matching record 
	RC CloseScan (); // Terminate file scan 
};

class RM_Record { 

	RID rid;
	char *pdata;
    int recordSize;

public: 
	RM_Record (); // Constructor 
	~RM_Record (); // Destructor 

	RC GetData (char *&pData) const; // Set pData to point to the record's contents 
	RC GetRid (RID &rid) const; // Get the record id
    RC LoadData(char *&pData, int recordSize, RID rid);
};

#define RM_NULL -1

void RM_PrintError(RC rc);

#define RM_HANDLEHASOPEN           (START_RM_ERR - 0)  //
#define RM_SIZELIMITEXCEED         (START_RM_ERR - 1)
#define RM_SIZEERROR               (START_RM_ERR - 2)
#define RM_HANDLENOTOPEN           (START_RM_ERR - 3)
#define RM_NORECORD                (START_RM_ERR - 4)
//#define RM_RIDNOTEXIST             (START_RM_ERR - 5)
#define RM_EOF                     (START_RM_ERR - 6)

#endif
