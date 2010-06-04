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
// BIODB.H
// 
/**
 * @file 
 * BIO information file (BIF) database.
 *
 * @publishedAll
 * @released
 */

#ifndef __BIODB_H__
#define __BIODB_H__

#include <bif.h>				// the bif reader
#include <f32file.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include <bifchangeobserver.h>
#endif

class CBifEntry;

/** BIO information file (BIF) database.

This class reads the installed BIF files and allows information from them 
to be easily extracted.

@see BIO_INFO_FILE 
@publishedAll
@released
*/
class CBIODatabase : public CBase
	{
public:
	/** Search methods. */
	enum TSearchList
		{
		/** Begin search from the start. */
		EStart, 
		/** Begin search from the last index position. */
		ENext
		};
public:
	IMPORT_C static CBIODatabase* NewLC(RFs& afileSystem);
	IMPORT_C static CBIODatabase* NewL(RFs& afileSystem);
	IMPORT_C ~CBIODatabase();

public:
	// BIF files contain all the information that needs to be registered
	// for each BIO Message type

	// Completely refresh the database will all the BIF Files in the
	// default directory
	IMPORT_C void AddAllBifsL(RFs& afileSystem);

	// Add one bifFile using the file name, then using Neil's handy 
	// class to read it
	// This will add the Parser
	IMPORT_C void AddBifL(CBioInfoFileReader* aBifReader);
	IMPORT_C void RemoveBifL(TUid aMsgID);
	IMPORT_C void RemoveBifL(TInt aIndex);

	// BIO Messages are determined to of a BIO type if we have the BIO
	// Parser identified by a WAP/NBS port, IANA MIME type or a Pattern
	IMPORT_C TInt BIOCount();

	// Get the BioEntry at this index
	// Return Index if found, NULL if not
	IMPORT_C const CArrayFix<TBioMsgId>* BIOEntryLC(TInt index);
	

	// Get the whole bif file class
	IMPORT_C const CBioInfoFileReader& BifReader(TInt aIndex) const;
     
	// Get the BIO Entry based on what type it is, pos indicates where to start looking 
	// after, and will get updated to current pos
	IMPORT_C const CArrayFix<TBioMsgId>* BioEntryByTypeLC(
								TSearchList aSearchType,
								TBioMsgIdType portType, 
								TInt& rIndex);

	IMPORT_C void GetBioIndexWithMsgIDL(TUid aMsgID, TInt& rIndex);

	// Get the BioMessageID for the Index
	IMPORT_C void GetBioMsgID(TInt aIndex, TUid& rMsgID);

	// Get the BioParserName for the Index
	IMPORT_C const TPtrC GetBioParserName(TInt aIndex);
	
	// Get the BioParserName for the BioMessageID
	IMPORT_C const TPtrC GetBioParserNameL(TUid aMsgID);
	
	// Get the ControlID for the Index
	IMPORT_C void GetBioControlID(TInt aIndex, TUid& rControlID);
	
	// Get the ControlID for the BioMessageID
	IMPORT_C void GetBioControlIDL(TUid aMsgID, TUid& rControlID);
	
	IMPORT_C const TPtrC GetBioControlName(TInt aIndex);
	IMPORT_C const TPtrC GetBioControlNameL(TUid aMsgID);
	
	// Get the String Extension for the BioMessageID
	IMPORT_C const TPtrC GetFileExtL(TUid aMsgID);

	IMPORT_C void GetDefaultSendBearerL(TUid aBioUID, TBioMsgId& rBioMsgIdentifier);
	IMPORT_C void GetDefaultSendBearerTypeL(TUid aBioUID, TBioMsgIdType& rPortType);
	IMPORT_C void GetDefaultSendBearerByTypeL(TUid aBioUID, TBioMsgIdType aPortType, TBioMsgId& rBioMsgIdentifier);

	// BIO Messages are determined to of a BIO type if we have the BIO
	// Parser identified by a WAP/NBS port, IANA MIME type or a Pattern
	// Return an Index

	// Get the Port# or Identifying string for sending
	IMPORT_C void GetPortNumberL(TUid aMsgID, TBioMsgIdType aPortType, TInt& aPortNumber);
	IMPORT_C void GetIdentifierTextL(TUid aMsgID, TBioMsgIdType aPortType, TBioMsgIdText& aText);

	// Test to see if this is a BioMessage
	// Pass in the type ... if its NBS or IANA pass in the string pattern
	//						if its WAP or SecureWap, pass in the port number
	// return kErrNone if success, kErrNotFound if it fails
	IMPORT_C TInt IsBioMessageL(TBioMsgIdType aPortType, const TDesC& aPattern, TUint16 aPort, TUid& rBioMsgUID);

	IMPORT_C TInt IsBioMessageL(TBioMsgId bioMessageData, TUid& rBioMsgUID);

private:
	CBIODatabase();
	void ConstructL(RFs& afileSystem);

private:
	//Utilities
	void GetTransportIDL(TInt aIndex, TBioMsgIdType aPortType, TBioMsgId& aBioMsgID);
	TBool IsLanguageFileL(const TDesC& aFileName, TInt& aExtLength) const;
	// If Persistence is required...
	// void InternalizeL(RReadStream& aStream);		
	// void ExternalizeL(RWriteStream& aStream) const;	
	
private:
	CArrayPtrFlat<CBioInfoFileReader>* iBifReaders;

};

#endif	// __BIODB_H__
