#ifndef RM_H
#define RM_H

#include <cerrno>
#include <iostream>
#include "pm.h"

class RM_Manager {

private:
	PF_Manager &pfm;

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

public: 
	RM_FileHandle (); // Constructor 
	~RM_FileHandle (); // Destructor 

	RC AttachFile(PF_FileHandle pffh);
	RC UnattachFile(PF_FileHandle pffh);

	RC GetRec (const RID &rid, RM_Record &rec) const; // Get a record 
	RC InsertRec (const char *pData, RID &rid); // Insert a new record, return record id 
	RC DeleteRec (const RID &rid); // Delete a record 
	RC UpdateRec (const RM_Record &rec); // Update a record 
	RC ForcePages (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s) to disk 
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

public: 
	RM_Record (); // Constructor 
	~RM_Record (); // Destructor 

	RC GetData (char *&pData) const; // Set pData to point to the record's contents 
	RC GetRid (RID &rid) const; // Get the record id 
};


struct RM_FileHeader {
	PageNum freePageHead;
};

struct RM_PageHeader {

};

#endif
