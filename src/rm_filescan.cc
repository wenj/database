#include <cstring>
#include <cassert>
#include "rm.h"

/**
 * 基于某一条件对RM文件中的记录进行扫描
 */

RM_FileScan::Predicate::Predicate(AttrType attrType, int attrLength, int attrOffset, CompOp compOp, void *value,
                                  ClientHint clientHint) {
    this->attrType = attrType;
    this->attrLength = attrLength;
    this->attrOffset = attrOffset;
    this->compOp = compOp;
    this->value = value;
    this->clientHint = clientHint;
}

/**
 * 判断clientValue的值是否符合谓词条件
 * @param clientValue 需要判断的record中的value
 * @return 是否符合谓词条件
 */
bool RM_FileScan::Predicate::eval(void *clientValue) {

    if (value == nullptr)
        return true;

    switch (attrType)
    {
        case AttrType::INT: {
            void* myValue = value + sizeof(char) * attrOffset;
            clientValue += sizeof(char) * attrOffset;
            int myInt = *reinterpret_cast<int *>(myValue);
            int clientInt = *reinterpret_cast<int *>(clientValue);
            switch (compOp) {
                case CompOp::EQ_OP:
                    return clientInt == myInt;
                case CompOp::LT_OP:
                    return clientInt < myInt;
                case CompOp::GT_OP:
                    return clientInt > myInt;
                case CompOp::LE_OP:
                    return clientInt <= myInt;
                case CompOp::GE_OP:
                    return clientInt >= myInt;
                case CompOp::NE_OP:
                    return clientInt != myInt;
                case CompOp::NO_OP:
                    return true;
            }
            break;
        }

        case AttrType::FLOAT: {
            static_assert(sizeof(float) == 4, "The size of float is not 4.");
            void* myValue = value + sizeof(char) * attrOffset;
            clientValue += sizeof(char) * attrOffset;
            float myFloat = *reinterpret_cast<float *>(myValue);
            float clientFloat = *reinterpret_cast<float *>(clientValue);
            switch (compOp) {
                case CompOp::EQ_OP:
                    return clientFloat == myFloat;
                case CompOp::LT_OP:
                    return clientFloat < myFloat;
                case CompOp::GT_OP:
                    return clientFloat > myFloat;
                case CompOp::LE_OP:
                    return clientFloat <= myFloat;
                case CompOp::GE_OP:
                    return clientFloat >= myFloat;
                case CompOp::NE_OP:
                    return clientFloat != myFloat;
                case CompOp::NO_OP:
                    return true;
            }
            break;
        }

        case AttrType::STRING: {
            char myStr[attrLength + 1];
            char clientStr[attrLength + 1];
            memcpy(myStr, value + sizeof(char) * attrOffset, sizeof(char) * attrLength);
            memcpy(clientStr, clientValue + sizeof(char) * attrOffset, sizeof(char) * attrLength);
            myStr[attrLength] = '\0';
            clientStr[attrLength] = '\0';
            int cmp = strcmp(clientStr, myStr);
            switch (compOp) {
                case CompOp::EQ_OP:
                    return cmp == 0;
                case CompOp::LT_OP:
                    return cmp < 0;
                case CompOp::GT_OP:
                    return cmp > 0;
                case CompOp::LE_OP:
                    return cmp <= 0;
                case CompOp::GE_OP:
                    return cmp >= 0;
                case CompOp::NE_OP:
                    return cmp != 0;
                case CompOp::NO_OP:
                    return true;
            }
            break;
        }
    }
}

RM_FileScan::RM_FileScan() : isOpen(false), cur(0, -1), fileHandle(NULL), next(NULL) {

}

RM_FileScan::~RM_FileScan() {

}

/**
 * 初始化对fileHandle指定的文件中记录的扫描，只有满足条件的才会被取出
 * AttrType定义在redbase.h里，包括INT、FLOAT和STRING
 * 需要自行把value cast成所需类型
 * CompOp定义在redbase.h里，包括EQ_OP,LT_OP,GT_OP,LE_OP,GE_OP,NE_OP,NO_OP
 *
 * @param fileHandle 打开的RM文件
 * @param attrType 用于比较的属性的类型：4字节整数，4字节浮点数或1-255长度的字符串
 * @param attrLength 用于比较的属性的长度
 * @param attrOffset 属性在每个记录的什么位置
 * @param compOp 比较方法
 * @param value 如果value是空指针，则取出全部记录；如果非空，则是需要用于比较的
 * @param pinHint 可以在高层决定使用什么策略，如果需要用到的话，要自己定义常量
 * @return
 */
RC RM_FileScan::OpenScan(const RM_FileHandle &fileHandle, AttrType attrType, int attrLength, int attrOffset,
                         CompOp compOp, void *value, ClientHint pinHint) {
    if (isOpen) {
        return RM_SCANHASOPEN;
    }
    isOpen = true;
    this->fileHandle = const_cast<RM_FileHandle*>(&fileHandle);
    if (this->fileHandle == nullptr)
        return RM_INVALIDHANDLE;

    // 如果value是null，就不再判断下面这些是否合法
    if (value == nullptr) {
        // attrType是否合法
        if (attrType < AttrType::INT || attrType > AttrType::STRING)
            return RM_INVALIDATTR;

        if (compOp < CompOp::NO_OP || compOp > CompOp::GE_OP)
            return RM_INVALIDOP;

        // attr的位置+长度是否超过了fileHandle的记录长度
        if (attrLength + attrOffset >= fileHandle.getRecordSize())
            return RM_INVALIDATTR;

        // 数值类型的长度不正确
        if (attrType != AttrType::STRING && attrLength != 4)
            return RM_INVALIDATTR;

        if (attrType == AttrType::STRING && (attrLength < 1 || attrLength > 255))
            return RM_INVALIDATTR;
    }

    predicate = Predicate(attrType, attrLength, attrOffset, compOp, value, pinHint);

    return 0;
}

/**
 *
 * @param rec
 * @return
 */
RC RM_FileScan::GetNextRec(RM_Record &rec) {
    if (!isOpen)
        return RM_SCANNOTOPEN;
    assert(fileHandle != nullptr);

    for (int i = cur.; i < fileHandle->)
}

/**
 *
 * @return
 */
RC RM_FileScan::CloseScan() {

}