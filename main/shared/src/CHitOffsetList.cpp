// ===========================================================================
//	CHitOffsetList.cp					 1994 Metrowerks Inc. All rights reserved.
// ===========================================================================

#include "NeoException.h"
#include "CHitOffsetList.h"


//==================================================================================

CHitOffsetList::CHitOffsetList() {
	size = 4;
	array = new HitOffsetData[size];
    if (!array) {
       throw CFatalError("CHitOffsetList::GrowList");
    }
	count = 0;
}

//==================================================================================

void CHitOffsetList::GrowList() {
	HitOffsetData *oldArray = array;
	int oldSize = size;
	
	size = oldSize * 2 - 1;
	array = new HitOffsetData[size];
    if (!array) {
       throw CFatalError("CHitOffsetList::GrowList");
    }
	for (int j = 0; j < oldSize; j++)
		array[j] = oldArray[j];
	delete[] oldArray;
}

//==================================================================================

void CHitOffsetList::AppendHit(short wordNumber) {
	if (count == size)
		GrowList();
	array[count++].wordNumber = wordNumber;
}

//==================================================================================
short CHitOffsetList::GetIndex(short wordNum) {
	for (int j = 0; j < count; j++)
		if (array[j].wordNumber == wordNum)
			return j;
	return -1;	// change this!!!
}
//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================



