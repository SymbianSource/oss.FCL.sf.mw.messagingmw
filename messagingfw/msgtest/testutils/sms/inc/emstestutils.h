// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef _EMS_UTILS_H
#define _EMS_UTILS_H

#include <e32std.h>
#include <gsmuelem.h>

// forward declarations
class CSmsMessage;
class CEmsInformationElement;
class CEmsPictureIE;
class CEmsAnimationIE;
class CFbsBitmap;

namespace EmsTestUtils
	{

	// function to compare two EMS information elements.
	IMPORT_C TBool CompareEmsIE(const CEmsInformationElement& left, const CEmsInformationElement& right);
	
	// function to compare two SMS messages with EMS objects
	IMPORT_C TBool CompareEmsMsgL(const CSmsMessage& left, const CSmsMessage& right);

	// function to find EMS element with given ID
	IMPORT_C const CEmsInformationElement* FindElementL(const CSmsMessage& msg, CSmsInformationElement::TSmsInformationElementIdentifier id);

	// Creates an EmsPicture object from the \ems\pics.mbm file. The
	// index into the MBM file is passed (0 is small, 1 is large, 
	// 2 is variable)
	IMPORT_C CEmsPictureIE* CreatePictureL(TInt index);

	// Creates an EmsAnimationIE object from the \ems\anims.mbm file. The
	// index into the MBM file is passed (0 is small, 1 is large)
	IMPORT_C CEmsAnimationIE* CreateAnimationL(TInt index);

	// Returns an array of adjusted EMS objects. These ems objects must be 
	// destroyed when the array is destroyed. The adjusted array 
	// fixes up formatting to remerge concatenated messages
	void AdjustedEmsObjectsL(const CSmsMessage& msg, RPointerArray<CEmsInformationElement>& adjusted);

	// function to extract the text portion of an SMS message into
	// an HBufC. This newly created HBufC is put onto the cleanup stack
	HBufC* ExtractSmsTextLC(const CSmsMessage& msg);

	// Creates an object from loaded bitmap - used by CreatePictureLC and 
	// CreateAnimationLC. T is an object type which can be constructed
	// from a bitmap. filename is the file to load the bitmap from
	// (assumed to be in C: or Z:) and index is the index into the bitmap
	template <class T>
	T* CreateEmsObjFromBitmapL(const TDesC& filename, TInt32 index);
	}

// This class is really only used in CompareEmsMsg. It enhances RPointerArray
// such that on close, all the elements in the array will be
// destroyed. This is required for proper handling in the CleanupStack
class RSafeEmsArray : public RPointerArray<CEmsInformationElement>
	{
public:
	RSafeEmsArray() : RPointerArray<CEmsInformationElement>(10) {}
	~RSafeEmsArray() { Close(); }
	void Close() 
		{ 
		ResetAndDestroy(); 
		RPointerArray<CEmsInformationElement>::Close(); 
		}
	};


#endif
