// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/


// epoc include
#include <e32base.h>
#include <imutdll.h>
#include <imcvtext.h>
#include "Local_imcvrecv.h"
#include <imcvcodc.h>	// TImConvert
#include <imcvutil.h>

#include <miuthdr.h>	// CImHeader
#include <miutatch.h>	// TImAttachmentFile
#include <txtrich.h>
#include <imcm.rsg>		// resource definition for IMCM

#include <pop3set.h>	// For including KUidMsgTypePOP3
#include <barsread.h>	// TResourceReader

#include "cmsvbodytext.h"

#ifdef __WINS__
#include <e32wins.h>    // for maxfilename lengths
#include <msvapi.h>
#endif

#ifndef __WINS__
#include <msvapi.h>
#endif
#include <cmsvattachment.h> 
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#ifndef _MSG_NO_LOGGING
#define __IMUT_LOGGING


// literals used
_LIT(KLogFilePath, "c:\\logs\\mailtext\\in.txt");
_LIT8(KNewLogHeader, "\r\n------ New Receive Session ------\r\n");
_LIT8(KFound, "found: ");
_LIT8(KPartLine, "unknown or part line");
_LIT8(KEndOFHeader, "End of header");
_LIT8(KSkippingData, "Skipping data");
_LIT8(KCollectingData7, "Collecting 7/8-bit data");
_LIT8(KCollectingDataQP, "Collecting QP data");
_LIT8(KCollectingBase64, "Collecting BASE64 data");
_LIT8(KCollectingUU, "Collecting UU data");
_LIT8(KDecodingBase64, "Decoding BASE64 data");
_LIT8(KDecodedBase64, "Decoded BASE64 data");
_LIT8(KDecodingUU, "Decoding UU data");
_LIT8(KDecodedUU, "Decoded UU data");
_LIT8(KDecodingQP, "Decoding QP data");
_LIT8(KDecodedQP, "Decoded QP data");
_LIT8(KUUEDataCorrupt, "UU Data is corrupt");
_LIT8(KFoundUUEStartBoundary, "UUE start boundary");
_LIT8(KFoundUUEEndBoundary, "UUE end boundary");
_LIT8(KClosingAttachFile, "Closing attachment file");
_LIT8(KDeletingAttachFile, "Deleting attachment file");
_LIT8(KDeletedAttachFile, "Deleted attachment file");
_LIT8(KFailedToWriteToFile, "Failed to write to attachment file");
_LIT8(KSectionHeader, "Section header");
_LIT8(KFoundMIMEBoundary, "MIME boundary");
_LIT8(KWritingToBody, "writing body");
_LIT8(KWroteToBody, "wrote body");
_LIT8(KWritingToFile, "writing attachment");
_LIT8(KWroteToFile, "wrote attachment");
_LIT8(KReset, "Resetting CLocalImRecvConvert");
_LIT8(KReseted, "Resetting CLocalImRecvConvert");
_LIT8(KDeleted, "Deleting CLocalImRecvConvert");
_LIT8(KLineHasLineBreak, "Linebreak");
_LIT8(KLastToken, "That was the last token");
_LIT8(KBlankLine, "Blank line");
_LIT8(KRemoveBoundary, "Remove boundary");
_LIT8(KRemovedBoundary, "Removed boundary");
_LIT8(KResetForNewEntry, "Reset for a New Entry");
_LIT8(KResetedForNewEntry, "Reseted for a New Entry");
_LIT8(KResetForNonMimeEntry, "Reset for a New Non-Mime Entry");
_LIT8(KResetedForNonMimeEntry, "Reseted for a New Non-Mime Entry");
_LIT8(KCreatingEntry, "Creating Entry");
_LIT8(KCreatedEntry, "Created Entry");
_LIT8(KUpdatingEntry, "Updating Entry");
_LIT8(KUpdatedEntry, "Updated Entry");
_LIT8(KMoveToParentEntry, "Moved to parent entry");
_LIT8(KMovedToParentEntry, "End of Moved to parent entry");
_LIT8(KStoringHeader, "Storing CImHeader");
_LIT8(KStoredHeader, "Stored CImHeader");
_LIT8(KStoringBody, "Storing CRichText");
_LIT8(KStoring8BitBody, "Storing CMsvBodyText");
_LIT8(KStoredBody, "Stored CRichText");
_LIT8(KStored8BitBody, "Stored CMsvBodyText");
_LIT8(KStoringMIMEHeader, "Storing CImMimeHeader");
_LIT8(KStoredMIMEHeader, "Stored CImMimeHeader");
_LIT8(KIgnoringStreams, "Discarding streams");
_LIT8(KStartMessageComplete, "Start Message Complete");
_LIT8(KMessageComplete, "Message Complete");
_LIT8(KHeaderComplete, "Message Header complete");
_LIT8(KReturnReceiptTo, "return-receipt-To is set");
_LIT8(KStartStoringEntryStream, "Starting Storing Entry Stream");
_LIT8(KStoringEntryStream, "Storing Entry Stream");
_LIT8(KDoneStoringEntryStream, "Done Storing Entry Stream");
#endif // _MSG_NO_LOGGING

// uncomment the line below if logging is to be enabled

#if defined(__IMUT_LOGGING)
#define RECVLOG(text) (iImcvLog?iImcvLog->AppendComment(text):void(0));
#define RECVLOG_OUT(text) (iImcvLog?iImcvLog->AppendOut(text):void(0));
#define LOGGING(string1, string2) (Logging(string1, string2));
#else
#define RECVLOG(text) (void(0));
#define RECVLOG_OUT(text) (void(0));
#define LOGGING(string1, string2) (void(0));
#endif

#define UNUSED_VAR( var ) var = var  // fix for armv5 build warnings
const TInt KBodyTextChunkSizeBytes = 512;

_LIT(KIntegerKey,"%d");
const TInt KSpaceToAddNumber=20;

// added this to handle DRM files
_LIT8(KTImcvDrm, "testagent.drm");


/**
IsIllegalChar()
Checks if the character is illegal in EPOC32 file system

@param aChar
@return
*/
LOCAL_C TBool IsIllegalChar(const TUint aChar)
	{
	// EPOC32 filenames must not contain any of the following chars
    return (aChar == '*' || aChar == '\\' || aChar == '<' || aChar == '>' ||
			aChar == ':'  || aChar == '"' ||  aChar    == '/' || aChar == '|' ||
			aChar < ' ' || aChar == '?');
	}

/**
RemoveSurroundingCharacters()

@param aLeft
@param aRight
@param aString
*/
LOCAL_C void RemoveSurroundingCharacters( const TUint8& aLeft, const TUint8& aRight, HBufC8& aString)
	{
	TPtr8 des = aString.Des();
	TInt len  = des.Length();
	
	if( len > 2 && des[0] == aLeft && des[len - 1] == aRight )
		{
		TPtrC8 mid = des.Mid(1, len - 1);
		des.Copy(mid);
		des.SetLength(len - 2);
		}
	}


//****************************************************************************************
//              Class CLocalRfc822Token Functions
//****************************************************************************************


/**
NewLC()
Static factory constructor. Uses two phase 
construction.

@leave KErrNoMemory
@return
A pointer to the newly created CLocalRfc822Token object.
*/
CLocalRfc822Token* CLocalRfc822Token::NewLC()
	{
	CLocalRfc822Token* self = new (ELeave) CLocalRfc822Token;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/**
ConstructL()

*/
void CLocalRfc822Token::ConstructL()
	{
	iOutputLine = HBufC8::NewL(KMaxIMailHeaderReadLineLength + 1);
	Reset();
	}


/**
CLocalRfc822Token()
Constructor
*/
CLocalRfc822Token::CLocalRfc822Token()
	{
	__DECLARE_NAME(_S("CLocalRfc822Token"));
	}


/**
~CLocalRfc822Token()
Destructor
*/
CLocalRfc822Token::~CLocalRfc822Token()
	{
	delete iOutputLine;
	iOutputLine=NULL;
	}


/**
Reset()

*/
void CLocalRfc822Token::Reset()
	{
	iOutputLine->Des().FillZ();
	iBufferedInput.SetLength(0);
	iBufferedInput.FillZ();

	iLastToken = EFalse;
	iFirstLine = ETrue;
	i822FieldsExist=EFalse;
	}


/**
ParseNextLineL()

@param aSourceLine
*/
void CLocalRfc822Token::ParseNextLineL(const TDesC8& aSourceLine)
	{
	TBool hitEof = EFalse;
	TBool endOfField = EFalse;
	// Always read one more packet than we need so we know we've read the entire field.

	// Consequently, we need to check whether there is any buffered input from the
	// previous rfc822 field:*******

	if(iBufferedInput.Length())
		{
		// the buffered input is never more than a line of text...
		*iOutputLine = iBufferedInput;
		
 		iBufferedInput.FillZ();
		iBufferedInput.SetLength(0);
		}
	iInputLine = aSourceLine;

	// discard any LF character from the begining of the line (this is abit strange?)
	while(iInputLine.Length() > 0 && (iInputLine[0] == KImcvLF || iInputLine[0] == KImcvCR))
		iInputLine.Delete(0, 1);

	// remove the CR at the end but only if there's stuff before it
	while(iInputLine.Length() > 0 && (iInputLine[iInputLine.Length() - 1] == KImcvLF || iInputLine[iInputLine.Length() - 1] == KImcvCR))
		iInputLine.Delete(iInputLine.Length() - 1, 1);

	// if the newly read line is now empty, then don't read any more through this tokeniser
	iLastToken = !iInputLine.Length();

	// only bother with the rest of the parsing if there's any data left
	if(iInputLine.Length() > 0 || iOutputLine->Length() > 0)
		{
		// check whether we're on the next field or still on the current one
		if(!iFirstLine && iInputLine.Length() > 0 && iInputLine[0] != KImcvSP && iInputLine[0] != KImcvTab)
			{
			// we've read the first line of the next field
			iBufferedInput = iInputLine;
			endOfField = ETrue; // flag the fact that the current field is ready for parsing
			}
		else // the line belongs in the current field, but without any leading whitespace
			{
			// find the first non-whitespace character
			const TUint8* ptr = iInputLine.Ptr();
			while((*ptr == KImcvSP || *ptr == KImcvTab) && (ptr - iInputLine.Ptr() < iInputLine.Length())) ptr++;
		
			// make sure we're not about to exceed the buffer
			if((iOutputLine->Length() + iInputLine.Length()) > iOutputLine->Des().MaxLength())
				iOutputLine = iOutputLine->ReAlloc(iOutputLine->Length() + iInputLine.Length());

			// now copy the remaining data into the buffer
			iOutputLine->Des().Append(iInputLine.Right(iInputLine.Length() - (ptr - iInputLine.Ptr())));
			}
		}
	else
		hitEof = ETrue;		//this means that EEndOfHeader is set if a line of zero length is parsed - ie. (clean CRLF - CRLF) = 0.
	iFirstLine = EFalse;

	if(endOfField || iLastToken)
		{ 
		// only parse the input when we have a whole field, 
		// or we just found the blank line that
		// separates the body from the header
		if(MatchAndRemoveL(KImcvFromPrompt))
			iHeaderPart = EFrom;
		else if(MatchAndRemoveL(KImcvReplyToPrompt))
			iHeaderPart = EReplyTo;
		else if(MatchAndRemoveL(KImcvToPrompt))
			iHeaderPart = ETo;
		else if(MatchAndRemoveL(KImcvCcPrompt))
			iHeaderPart = ECc;
		else if(MatchAndRemoveL(KImcvBccPrompt))
			iHeaderPart = EBcc;
		else if(MatchAndRemoveL(KImcvSubjectPrompt))
			iHeaderPart = ESubject;
		else if(MatchAndRemoveL(KImcvDatePrompt))
			iHeaderPart = EDate;
		else if(MatchAndRemoveL(KImcvMessageIdPrompt))
			{
			iHeaderPart = EMessageId;
			RemoveSurroundingCharacters(KImcvLeftChevron, KImcvRightChevron, *iOutputLine);
			}
		else if (PriorityAndReceiptsMatchAndRemoveL(EPriority))
			iHeaderPart=EPriority;
		else if (PriorityAndReceiptsMatchAndRemoveL(EReturnReceiptTo))
			iHeaderPart=EReturnReceiptTo;
		else if(hitEof)
			iHeaderPart = EEndOfHeader;
		else
			{
			iHeaderPart = EUnknown;
			iImRecvConvert->iMimeParser->ParseLineL(*iOutputLine);
			}
		}
	else 
		iHeaderPart = ENotFinished;
	}


/**
PriorityAndReceiptsMatchAndRemoveL()

@param aPriority
*/
TBool CLocalRfc822Token::PriorityAndReceiptsMatchAndRemoveL(THeaderPart aPriority)
	{
	CDesC8Array* fields=new (ELeave)CDesC8ArrayFlat(6);
	CleanupStack::PushL(fields);
	if (aPriority==EPriority)
		CImcvUtils::PriorityFieldsL(*fields);
	else if (aPriority==EReturnReceiptTo)
		CImcvUtils::ReceiptFieldsL(*fields);
	else
		{
		CleanupStack::PopAndDestroy(); //fields
		return EFalse;
		}
	TInt count=fields->Count();
	TBool matched=EFalse;
	for (TInt i=0;i<count;i++)
		{
		if (MatchAndRemoveL((*fields)[i]))
			{
			matched=ETrue;
			break;
			}
		}
	CleanupStack::PopAndDestroy(); //fields
	return matched;
	}


/**
MatchAndRemoveL()

@param aString
@return
TBool
*/
TBool CLocalRfc822Token::MatchAndRemoveL(const TDesC8& aString)
	{
	TInt comparison;
	TInt stringLength = aString.Length();
	TInt desLength = (*iOutputLine).Length();
	TInt compareLength = stringLength > desLength ? desLength : stringLength;
	TPtrC8 left((*iOutputLine).Left(compareLength));
	
	// now see whether the current line contains the search string
	comparison = left.CompareF(aString);
	if(!comparison)
		{
		// found the match string at the start of the output line, so remove it
		// get rid of any whitespace betweebn the tag and the data while we have a chance
		if (!i822FieldsExist)
			i822FieldsExist=ETrue;
		TInt whitespaceLength=0;
		TInt maxLength=desLength-stringLength;
		const TUint8* ptr = (*iOutputLine).Ptr();
		while(whitespaceLength <= maxLength && (ptr[stringLength+whitespaceLength] == KImcvSP || ptr[stringLength+whitespaceLength] == KImcvTab))
			whitespaceLength++;
		(iOutputLine->Des()).Delete(0, stringLength+whitespaceLength);
		}
	return (comparison==0);
	}

//****************************************************************************************
//             Class CLocalImRecvConvert
//****************************************************************************************


/**
NewLC()
Static factory constructor. Uses two phase construction 
and pushes the newly created object into the Cleanup stack.

@param anFs
@param aServerEntry
@param aMsgType
@param aEmailServiceId
@leave KErrNoMemory
@return
A pointer to the newly created CLocalImRecvConvert object.
*/
CLocalImRecvConvert* CLocalImRecvConvert::NewLC(RFs& anFs, CMsvServerEntry* aServerEntry, 
												TUid aMsgType, TMsvId aEmailServiceId)
	{
	CLocalImRecvConvert* self = new (ELeave) CLocalImRecvConvert(anFs, aServerEntry, aMsgType, 
																		aEmailServiceId);
	CleanupStack::PushL(self);
	self->ConstructL(anFs);
	return self;
	}


/**
NewL()
Static factory constructor. Uses two phase 
construction.

@param anFs
@param aServerEntry
@param aMsgType
@param aEmailServiceId
@leave KErrNoMemory
@return
A pointer to the newly created CLocalImRecvConvert object.
*/
CLocalImRecvConvert* CLocalImRecvConvert::NewL(RFs& anFs, CMsvServerEntry* aServerEntry, 
												TUid aMsgType, TMsvId aEmailServiceId)
	{
	CLocalImRecvConvert* self = CLocalImRecvConvert::NewLC(anFs, aServerEntry, aMsgType, 
																aEmailServiceId);
	CleanupStack::Pop();
	return self;
	}

  
/**
ConstructL()

@param anFs
*/
void CLocalImRecvConvert::ConstructL(RFs& anFs)
	{
	iFsSession = &anFs;  

	RResourceFile resFile;
	OpenResourceFileL(resFile, anFs);	// NB leaves if file not found

	// make sure the resource file will be closed if anything goes wrong
	// CloseResourceFile is declared in IMUTDLL.H and defined in IMUTDLL.CPP
	TCleanupItem close( CloseResourceFile, &resFile );
	CleanupStack::PushL( close );
	
	// Read iStore8BitData flag.
	HBufC8* buf = resFile.AllocReadLC( STORE_8BIT_BODY_TEXT );
	TResourceReader reader;
	reader.SetBuffer(buf);
	iStore8BitData = reader.ReadInt8();
	CleanupStack::PopAndDestroy(buf);

	buf = resFile.AllocReadLC( REMOVED_ATTACHMENT_TAG );
	reader.SetBuffer(buf);
	iRemovedAttachmentTag = reader.ReadTPtrC().AllocL();
	CleanupStack::PopAndDestroy(buf);
	
	buf = resFile.AllocReadLC( DEFAULT_ATTACHMENT_NAME );
	reader.SetBuffer(buf);
	iDefaultAttachmentName = reader.ReadTPtrC().AllocL();
	CleanupStack::PopAndDestroy(buf);
	buf = resFile.AllocReadLC(PARTIAL_DOWNLOAD_FOOTER_MESSAGE);
	reader.SetBuffer(buf);
	iPartialEmailFooter = (reader.ReadTPtrC()).AllocL();
	CleanupStack::PopAndDestroy(buf);
	buf = NULL;
	CleanupStack::PopAndDestroy(&resFile); // resFile (Close resfile)

	// create CImHeader object to store Rfc822 header info...
	iOutputHeader = CImHeader::NewLC();	// PushL(iHeader)
	CleanupStack::Pop();				// iHeader

	if (iStore8BitData)
		{
		//Create body text storage helper.
		iBodyText = CMsvBodyText::NewL();
		}
	else
		{
		// create CRichText object to store body...
		iParaLayer = CParaFormatLayer::NewL();
		iCharLayer = CCharFormatLayer::NewL();
		iOutputBody = CRichText::NewL(iParaLayer, iCharLayer);
		}
	
	// Create Rfc822 filter object...
	iRfc822Token = CLocalRfc822Token::NewLC(); // PushL(iRfc822Token)
	CleanupStack::Pop();				  // iRfc822Token
	iRfc822Token->SetImRecvConvert(this);

	// Create converter objects...
	iCharacterConverter = CCnvCharacterSetConverter::NewL();
	iCharConv = CImConvertCharconv::NewL(*iCharacterConverter, anFs);
	iHeaderConverter = CImConvertHeader::NewL(*iCharConv); 

	
	// Create MIME filter object...
	iMimeParser = CLocalMimeParser::NewL(*this);
	
	
	// logfile stuff
	iImcvLog=NULL;

#ifdef __IMUT_LOGGING	
	TRAPD(err,iImcvLog=CImLog::NewL(KLogFilePath, EAppend));
	UNUSED_VAR( err );
#endif

	iRootEntryId = EntryId();

	iNotFinishedRfc822Header = ETrue;
	
	iEmailEntry = new (ELeave) TMsvEmailEntry;
	iReceivingHeadersOnly=EFalse;	

	iParent = new (ELeave) CArrayFixFlat<TParentDetails>(3);

	iFirstBoundaryReached=EFalse;

	ResetL();
	
	RECVLOG( KNewLogHeader )
	}


/**
CLocalImRecvConvert()

@param anFs
@param aServerEntry
@param aMsgType
@param aEmailServiceId
*/
CLocalImRecvConvert::CLocalImRecvConvert(RFs& anFs, CMsvServerEntry* aServerEntry, 
							   TUid aMsgType, TMsvId aEmailServiceId)
				: iServerEntry(aServerEntry), iNewMsgType(aMsgType), iDefaultEntryType(ETextEntry), 
			  	  iEmailServiceId(aEmailServiceId), iAttachmentFile(anFs)
	{
	__DECLARE_NAME(_S("CLocalImRecvConvert"));
	}

  
/**
~CLocalImRecvConvert()
Destructor
*/
CLocalImRecvConvert::~CLocalImRecvConvert()
	{
	delete iEmailEntry;
	iEmailEntry=NULL;
	delete iOutputHeader;
	iOutputHeader=NULL;
	delete iBodyText;
	iBodyText=NULL;
	delete iBodyBuf;
	iBodyBuf=NULL;
	delete iParaLayer;
	iParaLayer=NULL;
	delete iCharLayer;
	iCharLayer=NULL;
	delete iOutputBody;
	iOutputBody=NULL;
	delete iRemovedAttachmentTag;
	iRemovedAttachmentTag=NULL;
	delete iDefaultAttachmentName;
	iDefaultAttachmentName=NULL;
	delete iRfc822Token;
	iRfc822Token=NULL;
	delete iMimeParser;
	iMimeParser=NULL;
	delete iHeaderConverter;
	iHeaderConverter=NULL;
	delete iCharConv;
	iCharConv=NULL;
	delete iCharacterConverter;
	iCharacterConverter=NULL;
	delete iAttachmentFullPath;
	iAttachmentFullPath=NULL;
	delete iImcvUtils;
	iImcvUtils=NULL;
	delete iParent;
	iParent=NULL;
	RECVLOG(KDeleted)
	delete iImcvLog;
	iImcvLog=NULL;
	delete iPartialEmailFooter;
	iPartialEmailFooter=NULL;
	}


/**
ResetForHeadersL()
This function is used in preference to ResetL() when the owner
only wishes to create  email entries in the remote mailbox - 
they do not wish to create any stores associated with the message
entry

*/
void CLocalImRecvConvert::ResetForHeadersL()
	{
	ResetL();
	iReceivingHeadersOnly=ETrue;
	}

/**
ResetL()

*/
void CLocalImRecvConvert::ResetL()
	{
	RECVLOG(KReset) 
	iBodyId=KMsvNullIndexEntryId;
	iSizeOfAttachmentsRemoved=0;

	User::LeaveIfError(iServerEntry->SetEntry(iRootEntryId));
	
	iReceivingHeadersOnly=EFalse;
	iMessageEntryCalled=EFalse;
	iLeaveError=KErrNone; 		
	iCurrentMultipartFolderEntryId=0;

	TParentDetails parent;
	parent.iAttachment=parent.iMHTML=EFalse;
	parent.iSize=0;
	iParent->InsertL(0,parent);

	delete iEmailEntry;
	iEmailEntry=NULL;
	iEmailEntry = new (ELeave) TMsvEmailEntry;

	// "iSavingAttachments" commands this code to store attachments in files.
	// Currently this is always set to ETrue as we intend to save all attachments
	// However, if the user wishes to discard (ie do not save) any 
	// attachments in incoming email messages - then this member variable
	// may be set to EFalse by the inline accessor function "SaveAllAttachments(TBool)"
	iSavingAttachments=ETrue;
	iAttachmentFileState=EFileIsClosed;

	// iLeaveError contains the error value of any functions that leaves (ie CRichText::InsertL)
	// or any returned error value that generates a leave from within CLocalImRecvConvert (ie RFile::Create)

	iGlobalIndent=0;
	iLongestLine=50;
	
	iImPriority=EMsvMediumPriority;
	delete iImcvUtils;
	iImcvUtils=NULL;
	iImcvUtils=CImcvUtils::NewL();

	// reset internal date
	iTimeDate.HomeTime();
	
	iMimeParser->Reset();
	
	iNewEntry=EFalse; //EFalse if the entry was moved to. ETrue if the entry was just created
	iTopMessagePart=KMsvNullIndexEntryId; //A value of NULL indicates the next entry created is a main message entry
	ResetForNewEntryL(iDefaultEntryType);
	iMIMEPart_822Header = EFalse;
	iPartialEmail=EFalse;

	RECVLOG(KReseted) 
	}


/**
ResetForNewEntryL()

@param entryType
*/
void CLocalImRecvConvert::ResetForNewEntryL(TValidEntryType entryType)
	{
	RECVLOG(KResetForNewEntry) 

	iFinishedHeader = EFalse;

	iPreviousLineLength = 0;
	iPreviousTrailingWhitespace = 0;
	iLastChar = 0;

	// Reset the new entry state
	ResetForNonMimeEntryL();

	// Clear the storage classes
	iMimeParser->ResetForNewEntry();
	iRfc822Token->Reset();

	iEmailPart = KNoPart;
	iEntryType=entryType;

	if (iEntryType==EMessageEntry)
		{
		iEmailPart = KParentPart;
		iMimeParser->SetMessageFolderType(EFolderTypeRFC822);
		}
	}


/**
ResetForNonMimeEntryL()

*/
void CLocalImRecvConvert::ResetForNonMimeEntryL()
	{
	iEntryType = ETextEntry;
	RECVLOG(KResetForNonMimeEntry) 

	iSkipData = EFalse;
	iPrepared = EFalse;
	iNewNonMIMEBodyPart=ETrue;
	iEncounteredLineEndingInCarriageReturn=EFalse;

	iAlgorithm = ENoAlgorithm;
	iCurrentPartIsRichText = ETrue;	
	CloseAttachmentFileL();
	iEntryDataSize = 0;
	
	if (iStore8BitData)
		{
		//Create a buffer to hold the body text as it is down loaded.
		delete iBodyBuf;
		iBodyBuf = NULL;
		iBodyBuf = CBufSeg::NewL(KBodyTextChunkSizeBytes);
		}
	else
		{
		iOutputBody->Reset();
		}
	
	iAttachmentName.Zero();
	iFinalLine = EFalse;

	iOutputHeader->Reset();
	iEmptyHeaderSize=iOutputHeader->DataSize();
	RECVLOG(KResetedForNonMimeEntry) 
	}


/**
ParseNextField()

@param aSourceLine
*/
TInt CLocalImRecvConvert::ParseNextField(const TDesC8& aSourceLine) 
	{
	__ASSERT_DEBUG(iRootEntryId!=KMsvNullIndexEntryId, gPanic(KPanicServiceIdNotValid));
	RECVLOG_OUT(aSourceLine);

	// If we are temporarily on the null entry then move back to the entry we are working on
	if ((iLeaveError == KErrNone) && (iServerEntry->Entry().Id() == KMsvNullIndexEntryId))
		iLeaveError = iServerEntry->SetEntry(iSavedEntryId);

	if(iLeaveError==KErrNone)
		TRAP(iLeaveError, ParseNextLineL(aSourceLine));		

	// Save the current entry id for later.
	if (iLeaveError==KErrNone)
		iSavedEntryId = iServerEntry->Entry().Id();

	TUid type = iServerEntry->Entry().iType;
	if( type != KUidMsvMessageEntry    &&
		type != KUidMsvEmailTextEntry  &&
		type != KUidMsvEmailHtmlEntry  &&
		type != KUidMsvAttachmentEntry)
		{
	// Set the current id to null so that we aren't locking any folders
	iServerEntry->SetEntry(KMsvNullIndexEntryId);
		}

	return iLeaveError;
	}


/**
ParseNextLineL()

@param aSourceLine
*/
void CLocalImRecvConvert::ParseNextLineL(const TDesC8& aSourceLine)
	{
	iParsedMimeBoundaryLast=0;
	if(!iFinishedHeader)
		{
		// start by getting the next token from the header
		iRfc822Token->ParseNextLineL(aSourceLine);

		switch(iRfc822Token->iHeaderPart)
			{
			case CLocalRfc822Token::EUnknown:
			case CLocalRfc822Token::ENotFinished:
				RECVLOG(KPartLine)
				break;
			case CLocalRfc822Token::EFrom:
				iOutputHeader->SetFromL(*iRfc822Token->OutputLine());
				LOGGING(KFound,KImcvFromPrompt);
				break;
			case CLocalRfc822Token::EReplyTo:
				iOutputHeader->SetReplyToL(*iRfc822Token->OutputLine());
				LOGGING(KFound,KImcvReplyToPrompt);
				break;
			case CLocalRfc822Token::ETo:
				ParseRecipientListL(iOutputHeader->ToRecipients());
				LOGGING(KFound,KImcvToPrompt);
				break;
			case CLocalRfc822Token::ECc: 
				ParseRecipientListL(iOutputHeader->CcRecipients());
				LOGGING(KFound,KImcvCcPrompt);
				break;
			case CLocalRfc822Token::EBcc: 
				ParseRecipientListL(iOutputHeader->BccRecipients());
				LOGGING(KFound,KImcvBccPrompt);
				break;
			case CLocalRfc822Token::ESubject:
				iOutputHeader->SetSubjectL(*iRfc822Token->OutputLine());
				LOGGING(KFound,KImcvSubjectPrompt);
				break;
			case CLocalRfc822Token::EDate:
				iRfc822Date.ParseDateField(*iRfc822Token->OutputLine() , iTimeDate); 
				if (iEmailEntry->Id() != iTopMessagePart)
					{
					iEmailEntry->iDate = iTimeDate;
					}
				LOGGING(KFound,KImcvDatePrompt);
				break;
			case CLocalRfc822Token::EMessageId:
				iOutputHeader->SetImMsgIdL(*iRfc822Token->OutputLine());
				LOGGING(KFound,KImcvMessageIdPrompt);
				break;
			case CLocalRfc822Token::EPriority:
				iImPriority=iImcvUtils->EvaluatePriorityText(*iRfc822Token->OutputLine());
				LOGGING(KFound,KImcvPriorityPrompt);
				break;
			case CLocalRfc822Token::EReturnReceiptTo:
				iOutputHeader->SetReceiptAddressL(*iRfc822Token->OutputLine());
				LOGGING(KFound,KReturnReceiptTo);
				break;
			case CLocalRfc822Token::EEndOfHeader:
				// the next line goes in the body part
				iFinishedHeader = ETrue;
				RECVLOG(KEndOFHeader)
				break;
			default:
				RECVLOG(KEndOFHeader)
				break;
			}


		if(iRfc822Token->iHeaderPart != CLocalRfc822Token::ENotFinished)
			{
			// Now that we've used the data, we also need to clear the output line from the tokeniser....
			iRfc822Token->OutputLine()->Des() = KNullDesC8;

			// whatever part we just read, we may also have read the empty line that separates
			// the header from the body
			if((iFinishedHeader = iRfc822Token->LastToken()) != EFalse)	//iFinishedHeader set to 1 on CRLF-
				{
				RECVLOG(KLastToken);
				iNotFinishedRfc822Header = EFalse;

				iHeaderConverter->SetMessageType(iMimeParser->MessageIsMime());
				iHeaderConverter->DecodeAllHeaderFieldsL(*iOutputHeader);

				if (iMimeParser->MessageIsMime())
					EndOfHeaderMIMEProcessingL();
				else
					EndOfHeaderProcessingL();

				if (iStore8BitData)
					{
					iBodyText->SetDefaultCharacterSet(iCharConv->SystemDefaultCharset());
					if ( iMimeParser->MessageIsMime() && iMimeParser->ContentType()==EMimeText )
						iBodyText->SetCharacterSet(iMimeParser->CurrentCharsetL());
					else
						iBodyText->SetCharacterSet(0);
					}
				else
					{
					// Get charset for decoding.
					if ( iMimeParser->MessageIsMime() && iMimeParser->ContentType()==EMimeText )
						iCharConv->PrepareToConvertToFromOurCharsetL(iMimeParser->CurrentCharsetL());
					else
						iCharConv->PrepareToConvertToFromOurCharsetL(iCharConv->SystemDefaultCharset());
					}
				}
			}
		}
	else if (iReceivingHeadersOnly == EFalse)
		{	
		//Set to EFalse when a single line is to be skipped (ie boundary line)
		iCommitLine=ETrue; 

		// read one line of the message body if I am processing a whole email message
		if(iMimeParser->MessageIsMime())
			ParseMimeLineL(aSourceLine);
		else
			ParseBodyLineL(aSourceLine);	

		if((iCommitLine) && (!iSkipData))
			DecodeAndStoreLineL(aSourceLine);
		}
	}


/**
MessageEntryDetailsL()

@return
*/
TMsvEmailEntry CLocalImRecvConvert::MessageEntryDetailsL()
	{
	iMessageEntryCalled=ETrue;
	CloseAttachmentFileL();

	// A message requiring manual termination and not part way through a MIME part header
	if( iTopMessagePart==EntryId() && iFinishedHeader) 
		StoreEntryStreamsL(KStore822Header|KStoreMIMEHeader);
	else
		{
		User::LeaveIfError(iServerEntry->SetEntry(iTopMessagePart));         
		delete iEmailEntry;
		iEmailEntry=NULL;
		iEmailEntry = new (ELeave) TMsvEmailEntry(iServerEntry->Entry());
		}
		
	return *iEmailEntry;
	}


/**
MessageCompleteL()

@param aEmailEntry
*/
void CLocalImRecvConvert::MessageCompleteL(TMsvEmailEntry aEmailEntry)
	{
	// Restore the entry context
	if (iServerEntry->Entry().Id() == KMsvNullIndexEntryId)
		User::LeaveIfError(iServerEntry->SetEntry(iSavedEntryId));

	__ASSERT_DEBUG(iMessageEntryCalled, gPanic(KPanicMessageEntryNotCalled));
	__ASSERT_DEBUG(aEmailEntry.Id()==iTopMessagePart, gPanic(KPanicMessageEntryIdHasChanged)); // Id should be set to iTopMessagePart

	// Called ResetL() to reset object instead of ResetForHeadersL()
	__ASSERT_DEBUG(iReceivingHeadersOnly, gPanic(KPanicIncorrectResetState)); 

	if(iEmailEntry)
		{
		if (iEmailEntry->Id())
			{
			if(iLeaveError==KErrNone)
				{
				// a remote email header cannot have any attachments or be marked as complete...
				aEmailEntry.SetAttachment(EFalse);
				aEmailEntry.SetMHTMLEmail(EFalse);
				aEmailEntry.SetComplete(EFalse);
				User::LeaveIfError(iServerEntry->ChangeEntry(aEmailEntry));
				RECVLOG(KHeaderComplete)
				}
			else
				{
				if (!iPopulateMessage)
					{
					TMsvId currentId = EntryId();
					User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().Parent()));			
					User::LeaveIfError(iServerEntry->DeleteEntry(currentId));
					}
				User::Leave(iLeaveError);
				}
			}
		}

	// Save the entry context and move it to null so that we're not locking any folders
	iSavedEntryId = iServerEntry->Entry().Id();
	User::LeaveIfError(iServerEntry->SetEntry(KMsvNullIndexEntryId));
	}


/**
MessageCompleteL()

@param aPartialDownload
*/
void CLocalImRecvConvert::MessageCompleteL(TBool aPartialDownload)
	{
	iPartialEmail=aPartialDownload;
	MessageCompleteL();
	}
  

/**
MessageCompleteL()
*/
void CLocalImRecvConvert::MessageCompleteL()
	{
	// Restore the entry context
	RECVLOG(KStartMessageComplete)
	if (iServerEntry->Entry().Id() == KMsvNullIndexEntryId)
		User::LeaveIfError(iServerEntry->SetEntry(iSavedEntryId));

	if (iParsedMimeBoundaryLast==EFalse && iAttachmentFileState==EFileIsOpen && iPartialEmail!=EFalse)
		{
		iAttachmentFileState=EFileTopIncomplete;
		}

	CloseAttachmentFileL();

	if(!iEmailEntry->Id())
		return;

	if(iLeaveError!=KErrNone)
		{
		User::Leave(iLeaveError);
		}

	if (EntryId()!=iTopMessagePart)
		{
		iEmailEntry->SetComplete(ETrue);
		StoreEntryStreamsL(KStoreBodyText);
		MoveToParentEntryL();

		if( iTopMessagePart==EntryId() && iFinishedHeader) 
			{
			iEmailEntry->SetVisible(ETrue);
			iEmailEntry->SetInPreparation(EFalse);
			iEmailEntry->SetBodyTextComplete(ETrue);
			iEmailEntry->SetComplete(ETrue);

			// Main message.
			StoreMessageEntryDetailsL();
			return;
			}
		else if(iEmailEntry->iType == KUidMsvMessageEntry)
			{
			StoreMessageEntryDetailsL();
			MoveToParentEntryL();
			StoreMessageEntryDetailsL();
			}
		}

	// A message requiring manual termination and not part way through 
	// a MIME part header.

	User::LeaveIfError(iServerEntry->SetEntry(iTopMessagePart));         
	if(NULL!=iEmailEntry)
		{
		delete iEmailEntry;
		iEmailEntry=NULL;
		}
	iEmailEntry = new (ELeave) TMsvEmailEntry(iServerEntry->Entry());
	iEmailEntry->SetVisible(ETrue);
	iEmailEntry->SetInPreparation(EFalse);
	iEmailEntry->SetComplete(ETrue);
	iEmailEntry->SetBodyTextComplete(ETrue);

	iEmailEntry->SetAttachment(iParent->At(0).iAttachment);
	iEmailEntry->SetMHTMLEmail(iParent->At(0).iMHTML);

	if(iEmailEntry->MHTMLEmail() == EFalse && iEmailEntry->Attachment() == EFalse && iRelatedAttachments !=EFalse)
		{
		iEmailEntry->SetAttachment(ETrue);
		}
	iRelatedAttachments=EFalse;
	
	iEmailEntry->iSize=iParent->At(0).iSize;
	iEmailEntry->SetMessageFolderType(iParent->At(0).iFolder);

	User::LeaveIfError(iServerEntry->ChangeEntry(*iEmailEntry));

	// Save the entry context and move it to null so that we're not locking any folders
	iSavedEntryId = iServerEntry->Entry().Id();
	User::LeaveIfError(iServerEntry->SetEntry(KMsvNullIndexEntryId));
	RECVLOG(KMessageComplete)
	}


/**
PrepareDecoder()

*/
void CLocalImRecvConvert::PrepareDecoder()
	{
	switch(iMimeParser->ContentEncoding())
		{
		case EEncodingTypeNone:
		case EEncodingType7Bit:
		case EEncodingType8Bit:
			iAlgorithm = ENoAlgorithm;
			RECVLOG(KCollectingData7)
			break;
		case EEncodingTypeQP:
			iAlgorithm = EQPDecode;
			RECVLOG(KCollectingDataQP)
			break;
		case EEncodingTypeBASE64:
			iAlgorithm = EBase64Decode;
			RECVLOG(KCollectingBase64)
			break;	
		case EEncodingTypeUU:
			iAlgorithm = EUUDecode;
			RECVLOG(KCollectingUU)
			break;	
		default:	// EEncodingTypeUnknown, EEncodingTypeNone, EEncodingTypeBinary	
			iAlgorithm = ENoAlgorithm;
			break;
		} // switch
	iPrepared = ETrue;
	}

  
/**
DecodeAndStoreLineL()

@param aSourceLine
*/
void CLocalImRecvConvert::DecodeAndStoreLineL(const TDesC8& aSourceLine)
	{
	TInt sourceLineLength=aSourceLine.Length();
	TBool blankLine = EFalse;

	// create a temporary buffer to write the decoded data into. 
	// This will always be as long as or shorter than the original.
	
	HBufC8* convertedLine = HBufC8::NewLC(sourceLineLength+KConversionRemainderLength);
	TPtr8 des(convertedLine->Des());

	if(iFinalLine)						
		{
		// We've got to the end of encoded section, so set in order to skip all
		// trailing empty lines & postamble until we reach the next MIME/UUE boundary

		RECVLOG(KSkippingData)
		iSkipData = ETrue;
		}
	else
		{
		switch(iAlgorithm)
			{
			case EBase64Decode:
				RECVLOG(KDecodingBase64);

				iB64Codec.Decode(aSourceLine, des); 
				RECVLOG(KDecodedBase64);
				break;
			case EUUDecode:
				RECVLOG(KDecodingUU);
				//used to end a Mime encoded UU section else no purpose
				iFinalLine=(aSourceLine.CompareF(KImcvUueLastLine)==0);	
				if(iFinalLine)
					iCommitLine=EFalse;
				else
					{
					//returns True on invalid data
					if((!iFinalLine)&&(iUUCodec.Decode(aSourceLine, des))) 
						{
						RECVLOG(KUUEDataCorrupt)
						if(iAttachmentFileState==EFileIsOpen)
							{
							iAttachmentFileState=EFileIsCorrupt;
							iLeaveError=KErrCorrupt;
							CloseAttachmentFileL();
							}
						else
							User::Leave(KErrCorrupt);
						}
					}
				RECVLOG(KDecodedUU);
				break;
			case EQPDecode:
				RECVLOG(KDecodingQP);
				iQPCodec.Decode(aSourceLine, des);
				RECVLOG(KDecodedQP);
				break;
			case ENoAlgorithm:
				{				
				// If the data is to be stored in CRichText clean it up before copying it to the buffer
				TLex8 lex(aSourceLine);
				blankLine=ETrue;
				while(blankLine && !lex.Eos())
					{
					blankLine = (lex.Peek()==KImcvSP || lex.Peek()==KImcvTab 
						|| lex.Peek()==KImcvCR || lex.Peek()==KImcvLF);
					lex.Inc();
					}
				des.Copy(aSourceLine);
				}				
			default:
				break;
			} // end switch
	
			// Commits the decoded data to the appropriate store or trashes the line 
			if(iCommitLine) 
				{
				if (iLeftOver.Length())
					{
					des.Insert(0, iLeftOver);
					iLeftOver.SetLength(0);
					}

				if(iCurrentPartIsRichText)
					{
					if (iStore8BitData)
						iBodyBuf->InsertL(iBodyBuf->Size(), des);
					else
						WriteToBodyL(des, blankLine);
					}
				else
					WriteToAttachmentL(des);
				}
		} // end else

	CleanupStack::PopAndDestroy(); // convertedLine
	}


/**
ParseMimeLineL()

@param aSourceLine
*/
void CLocalImRecvConvert::ParseMimeLineL(const TDesC8& aSourceLine)
	{

	if(!iMimeParser->IsBoundary(aSourceLine))
		{
		if(!iPrepared) // first line of the body
			{		
			if (iMIMEPart_822Header)
				{
				// missing 822 part header. Revert to default.
				EndOfHeaderMIMEProcessingL();
				iMIMEPart_822Header=EFalse;
				}

			PrepareDecoder();
			}

		if (CheckUUEStartL(aSourceLine))
			{
			iAlgorithm=EUUDecode;
			iCommitLine=EFalse;
			}
		}
	else
		{
		// found a MIME boundary so store the current parts data and update its entry.

		RECVLOG(KFoundMIMEBoundary)
		iCommitLine=EFalse; //Dont store this line as its a boundary.

		if(!iFirstBoundaryReached && iEntryType==EFolderEntry)
			{
			iFirstBoundaryReached=ETrue;
			if (!iCurrentMultipartFolderEntryId)
				iCurrentMultipartFolderEntryId = EntryId();
			MoveToParentEntryL();
			ResetForNewEntryL(iDefaultEntryType);
			return; // First boundary encountered.
			}

		CloseAttachmentFileL();
		if(iNewEntry)
			{
			iEmailEntry->SetComplete(ETrue);
			iEmailEntry->SetBodyTextComplete(ETrue);
			StoreEntryStreamsL(KStoreMIMEHeader | KStoreBodyText);
			if (iBodyId==KMsvNullIndexEntryId)
				{
				iBodyId=iEmailEntry->Id();
				}
			}

		iSkipData = EFalse;

		// check whether we just found the terminating boundary...
		if(iMimeParser->IsTerminatingBoundary())
			{
			RECVLOG(KRemoveBoundary);
			iMimeParser->RemoveBoundary();
			RECVLOG(KRemovedBoundary);
			iMIMEPart_822Header = EFalse;

			if (EntryId()!=iTopMessagePart)
				{
				if(iEmailPart==KParentPart)
					{
					// rfc822 message which is not  not multipart.
					iEmailPart=KNoPart;
					MoveToParentEntryL();				

					// Embedded message
					iEmailEntry->SetComplete(ETrue);
					iEmailEntry->SetBodyTextComplete(ETrue);
					StoreMessageEntryDetailsL();
					}
				else if (iEmailEntry->iType==KUidMsvMessageEntry)
					{
					// Moving up from a multi embedded rfc822 message.
					iEmailEntry->SetComplete(ETrue);
					iEmailEntry->SetBodyTextComplete(ETrue);
					StoreMessageEntryDetailsL();
					}
				}

			MoveToParentEntryL();
			iEntryDataSize = iEmailEntry->iSize;

			if(iServerEntry->Entry().iType == KUidMsvFolderEntry)
				{
				iCurrentMultipartFolderEntryId = EntryId();
				}
			else if (EntryId()!=iTopMessagePart)
				{
				if(iEmailEntry->iType == KUidMsvMessageEntry)
					{
					iEmailEntry->SetComplete(ETrue);
					iEmailEntry->SetBodyTextComplete(ETrue);
					StoreMessageEntryDetailsL();
					}
				MoveToParentEntryL();
				iEntryDataSize += iEmailEntry->iSize;
				}

			if(!iNewEntry)
				ResetForNonMimeEntryL();

			RECVLOG(KSkippingData)
			iSkipData = ETrue;
			iDefaultEntryType=ETextEntry;
			}
		else // if regular boundary
			{ 
			RECVLOG(KSectionHeader)
			if(iEmailPart==KParentPart && EntryId()!=iTopMessagePart)
				{
				// rfc822 message which is not  not multipart.
				iEmailPart=KNoPart;
				MoveToParentEntryL();
				// Embedded message
				iEmailEntry->SetComplete(ETrue);
				iEmailEntry->SetBodyTextComplete(ETrue);
				StoreMessageEntryDetailsL();
				}

			if (!iCurrentMultipartFolderEntryId && iEmailPart==KNoPart)
				MoveToParentEntryL();

			ResetForNewEntryL(iDefaultEntryType);
			}
		}
	}

  
/**
EndOfHeaderProcessingL()

*/
void CLocalImRecvConvert::EndOfHeaderProcessingL()
	{	
	CreateEntryL();
	StoreEntryStreamsL(KStore822Header);
	iEntryDataSize = 0;
	}


/**
EndOfHeaderMIMEProcessingL()
Have just finished processing header, what next.. ?
All MIME entry entry creation takes place here.

*/
void CLocalImRecvConvert::EndOfHeaderMIMEProcessingL()
	{	
	if (iMimeParser->IsMessageDigest())
		iDefaultEntryType=EMessageEntry;

	if (iMimeParser->VCard() || iMimeParser->VCalendar())
		{
		iCurrentPartIsRichText = EFalse;	
		iEntryType = EAttachmentEntry;
		}

	// Don't create entry if an embedded message header. 
	if (!iMIMEPart_822Header || iTopMessagePart==EntryId() )
		CreateEntryL(); 

	if (!iMIMEPart_822Header && !iMimeParser->MimeFieldsExist() && iDefaultEntryType==EMessageEntry)
		{
		// MIME header not present. So expecting embedded 822 header
		iEmailPart = KParentPart;
		iMIMEPart_822Header=ETrue;
		iEntryType=ETextEntry;
		}
	else if (iMimeParser->ContentType()==EMimeMessage)
		{
		// Having received A MIME header of type message/..., store and continue.

		StoreEntryStreamsL(KStore822Header|KStoreMIMEHeader);
		iMimeParser->ResetForNewEntry();
		iMIMEPart_822Header=ETrue;

		// Non-multipart embedded message.
		if (iTopMessagePart==EntryId())
			{
			iEntryType = EMessageEntry;
			ResetForNonMimeEntryL();
			}
		else
			iEntryType=ETextEntry;
		}
	else if ( (iTopMessagePart==EntryId() || iMIMEPart_822Header)&&(!iReceivingHeadersOnly) )
		{
		// Main rfc822 header or embedded header.

		TImEmailFolderType folderType=iMimeParser->MessageFolderType();
		if (iMIMEPart_822Header)
			{
			iEmailEntry->iDetails.Set(iOutputHeader->From());
			iEmailEntry->iDescription.Set(iOutputHeader->Subject());
			iMIMEPart_822Header=EFalse;
			}

		if (iRfc822Token->i822FieldsExist)
			StoreEntryStreamsL(KStore822Header|KStoreMIMEHeader);
		else
			StoreEntryStreamsL(KStoreMIMEHeader);


		if (iMimeParser->ContentType()==EMimeMultipart)
			{	
			RECVLOG(KSkippingData)
			ResetForNewEntryL(EFolderEntry);
			iMimeParser->SetMessageFolderType(folderType);
			iSkipData = ETrue;
			iEmailPart = KMultiPart;
			iCurrentMultipartFolderEntryId=0;
			}
		else 
			{
			// Not multipart but some header data to store.
			iEntryDataSize = 0;
			}

		CreateEntryL();
		}

	iRfc822Token->i822FieldsExist=EFalse;
	iMimeParser->ResetMimeFieldsExist();
	
	iFinishedHeader = iMIMEPart_822Header ? EFalse:ETrue;
	iSkipData = iMimeParser->ContentType()==EMimeMultipart ? ETrue:EFalse;
	}


/**
ParseBodyLineL()
Non Mime body parsing

@param aSourceLine
*/
void CLocalImRecvConvert::ParseBodyLineL(const TDesC8& aSourceLine)
	{
	TInt len=aSourceLine.Length();
	iTopPartialDownloadCounter+=len; // added for TOP. increment the number of bytes of the attachment downloaded so far
	TMsvId id=0;
	if (iBodyId==KMsvNullIndexEntryId)
		{
		iBodyId=iEmailEntry->Id();
		}

	// first check whether this line is a UUEncode start boundary
	if(CheckUUEStartL(aSourceLine)) 
		{
		RECVLOG(KFoundUUEStartBoundary)
		TFileName tempStore = iAttachmentName;
		id = EntryId();
		if (!iNewNonMIMEBodyPart || id!=iTopMessagePart) // main message entry
			{
			iEmailEntry->SetComplete(ETrue);
			iEmailEntry->SetBodyTextComplete(ETrue);
			StoreEntryStreamsL(KStoreBodyText);
			}
		MoveToParentEntryL();
		if ( !CreateNonMIMEFolderEntryL(id))
			ResetForNonMimeEntryL();
			
		iEntryType = EAttachmentEntry;
		CreateEntryL();
		SetAttachmentName(tempStore);
		
		iCurrentPartIsRichText = EFalse;
		iAlgorithm=EUUDecode;
		iCommitLine=EFalse;
		if(!iSavingAttachments)
			{
			RECVLOG(KSkippingData)
			iSkipData=ETrue;
			}
		iNewNonMIMEBodyPart=EFalse;
		}
	else if(aSourceLine.CompareF(KImcvUueEnd)==0) // Checks for the UUEncode end boundary
		{
		RECVLOG(KFoundUUEEndBoundary)
		CloseAttachmentFileL();
		StoreEntryDataL();
		MoveToParentEntryL();
		iSkipData = EFalse;
		iCommitLine=EFalse;
		iNewNonMIMEBodyPart=ETrue; 
		}
	else if (iNewNonMIMEBodyPart && !( len==2 && aSourceLine[0]==KImcvCR && aSourceLine[1]==KImcvLF ))
		{
		id = EntryId();
		if (!iNewNonMIMEBodyPart || id!=iTopMessagePart)
			{
			iEmailEntry->SetComplete(ETrue);
			iEmailEntry->SetBodyTextComplete(ETrue);
			StoreEntryStreamsL(KStoreBodyText);
			}
		MoveToParentEntryL();
		if ( !CreateNonMIMEFolderEntryL(id))
			ResetForNonMimeEntryL();
		iAlgorithm=ENoAlgorithm;
		iEntryType = ETextEntry;
		CreateEntryL();
		iNewNonMIMEBodyPart=EFalse;
		}	
	}


/**
CreateNonMIMEFolderEntryL()

@param aCurrentId
@return
*/
TBool CLocalImRecvConvert::CreateNonMIMEFolderEntryL(TMsvId aCurrentId)
	{
	if ( aCurrentId==iTopMessagePart || iCurrentMultipartFolderEntryId )
		return EFalse;

	// Create Folder.
	iServerEntry->SetEntry(iTopMessagePart); 
	iEmailPart = KMultiPart;
	iEntryType = EFolderEntry;
	CreateEntryL();

	// Move existing child entry under folder.
	TMsvId destId = EntryId();
	iServerEntry->SetEntry(iTopMessagePart); 
	iServerEntry->MoveEntryWithinService(aCurrentId, destId);
	User::LeaveIfError(iServerEntry->SetEntry(aCurrentId));
	User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().Parent())); 

	// Create MimeHeader.
	iEmailEntry->SetMessageFolderType(iMimeParser->MessageFolderType());

	RECVLOG(KResetForNewEntry) 

	iMimeParser->ResetForNewEntry();
	
	if (iStore8BitData)
		{
		//Create a buffer to hold the body text as it is down loaded.
		delete iBodyBuf;
		iBodyBuf = NULL;
		iBodyBuf = CBufFlat::NewL(KBodyTextChunkSizeBytes);
		}
	else
		{
		iOutputBody->Reset();
		}
		
	ResetForNonMimeEntryL(); 
	RECVLOG(KResetedForNewEntry) 
	return ETrue;
	}


/**
CreateAttachmentL()

@return
*/
TBool CLocalImRecvConvert::CreateAttachmentL()
	{
	// Get and set Attachment File path
	TFileName filepath;
	// added to support TOP command. Reset the download counter each time we have a new
	// attachment
	iTopPartialDownloadCounter = 0; 

	// Need to check that the complete filename: iAttachmentFullPath & iAttachmentName	
	// does not exceed 256 characters. Greater than this and it cannot be saved as a file.

	TBool addExtension=ETrue;
	if(iAttachmentName.Length() == 0)	//i.e. problem with Attachment name
	    {
		// No filename present. Generate one.
		if(iMimeParser->ContentDescription().Length()!=0)
			{
			// Use ContentDescription() as default name 
			// - as this is more informative than the default

			TLex sourceLineLex = iMimeParser->ContentDescription();
			ExtractFilename(sourceLineLex, iAttachmentName);
			}
		else
			iAttachmentName = *iDefaultAttachmentName;
		}
	else
		{
		// Filename present. Check it is valid.
		ReplaceInvalidCharacters(iAttachmentName);
		if (iAttachmentName.Locate(KImcvFullStop)!=KErrNotFound)
			addExtension=EFalse;
		}
	if (addExtension)
		AddFileExtension();

	CMsvStore* store = iServerEntry->EditStoreL();	
	CleanupStack::PushL(store);
	CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	RFile file; 
	store->AttachmentManagerExtensionsL().CreateAttachmentL(iAttachmentName,file,attachment);
	
	iAttachmentFile.SetFileHandle(file,TImAttachmentFile::EImFileWrite);
	store->CommitL();
	CleanupStack::PopAndDestroy(store); // store

	if(KErrNone!=iLeaveError)
		{
		iAttachmentFileState=EFileFailedToOpen;
		CloseAttachmentFileL();
		return EFalse;
		}

	iAttachmentFileState=EFileIsOpen;
	return ETrue;
	}


/**
WriteToAttachmentL()

@param text
*/
void CLocalImRecvConvert::WriteToAttachmentL(const TDesC8& text)
	{
	if ( (iAttachmentFileState==EFileIsClosed || iAttachmentFileState==EFileNotOpen) 
		&& CreateAttachmentL() && iEntryType!=EHtmlEntry)
			iEmailEntry->SetAttachment(ETrue);
	
	if(iAttachmentFileState!=EFileIsOpen || !text.Length())
		{
		RECVLOG(KSkippingData)
		iSkipData = ETrue;
		}

	// write decoded data into a file if there is any data there to write.

	RECVLOG(KWritingToFile)

	// Convert text before writing to attachment.
	
	// Store as Binary files..
	iLeaveError=iAttachmentFile.WriteFile(text);

	if(KErrNone==iLeaveError)
		iEntryDataSize += text.Length();
	else	
		{
		// the file write failed (eg.there is no space left), set new file state and skip 
		RECVLOG(KFailedToWriteToFile)
		iAttachmentFileState=EFileIsIncomplete;
		CloseAttachmentFileL();
		}
	RECVLOG(KWroteToFile)
	}


/**
CloseAttachmentFileL()

*/
void CLocalImRecvConvert::CloseAttachmentFileL()
	{
	// If anything bad happened a message is sent to the parts CRichText
	switch(iAttachmentFileState)
		{
		case EFileNotOpen:
			iAttachmentFileState=EFileIsClosed;
		case EFileIsClosed: //do nothing - this shouldn't happen	
			break;
		case EFileIsOpen:		//successful attachment decode
			RECVLOG(KClosingAttachFile)
			iAttachmentFile.CloseFile();
			iAttachmentFileState=EFileIsClosed;
			iEmailEntry->SetComplete(ETrue);
			break;
		case EFileIsIncomplete:		// file write failed
		case EFileFailedToOpen:		// can't open attach file
		case EFileIsCorrupt:		// UU data being decoded is corrupt
			RECVLOG(KClosingAttachFile)
			iAttachmentFile.CloseFile();	//file has to be closed before it can be deleted
			RECVLOG(KDeletingAttachFile)
			{ 
			// NOTE - need the braces to stop error for re-definition of store

			CMsvStore* store = iServerEntry->EditStoreL();	
			CleanupStack::PushL(store);
		
			// Remove the attachment and commit the store.	
			store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
			store->CommitL();
			CleanupStack::PopAndDestroy(store);
			}
			iEmailEntry->SetAttachment(EFalse);
			iAttachmentFileState=EFileIsClosed;
			RECVLOG(KDeletedAttachFile)

			if(iSavingAttachments && !iStore8BitData)
				{
				WriteToBodyL(KImcvParagraph);
				WriteToBodyL(*iRemovedAttachmentTag);	//lost attachment - notify user
				TBuf8<KMaxFileName> name;
				name.Copy(iAttachmentName); //16 to 8
				WriteToBodyL(name);
				WriteToBodyL(KImcvParagraph);
				}

			User::Leave(iLeaveError);
			// Skip any remaining encoded data in message
			break;
		case EFileTopIncomplete:
			RECVLOG(KClosingAttachFile)
			iAttachmentFile.CloseFile();	//file has to be closed before it can be deleted

			// added for TOP command. Ensure we know correct amount of data for later redownload
			iSizeOfAttachmentsRemoved+=iTopPartialDownloadCounter;


			RECVLOG(KDeletingAttachFile)
			{ 
			// NOTE - need the braces to stop error for re-definition of store
			CMsvStore* store = iServerEntry->EditStoreL();	
			CleanupStack::PushL(store);
		
			// Remove the attachment and commit the store.	
			store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
			store->CommitL();
			CleanupStack::PopAndDestroy(store);
			}
			iEmailEntry->SetAttachment(EFalse);
			iAttachmentFileState=EFileIsClosed;
			RECVLOG(KDeletedAttachFile);
			TMsvId id = EntryId();
			TMsvId parent = iServerEntry->Entry().Parent();
			MoveToParentEntryL();
			TMsvId setTo=iServerEntry->Entry().Id();
			if(setTo!=parent)
				{
				iServerEntry->SetEntry(parent);
				}
			User::LeaveIfError(iServerEntry->DeleteEntry(id));			
			iServerEntry->SetEntry(setTo);
			break;
		}
	}


/**
LineIsAllWhitespace()

@return
*/
TBool CLocalImRecvConvert::LineIsAllWhitespace()
	{// returns 1 if all elements of the current line are whitespace
	TBool	spaceFound = 1;
	TLex8	aLex = iLineLex;

	while (spaceFound && aLex.Peek()!=KImcvCR)
		{
		if (aLex.Peek()==KImcvSP)
			{
			spaceFound = 1;
			aLex.Inc();
			}	
		else spaceFound = 0;
		}
	return (spaceFound);
	}

  
/**
CheckUUEStartL()

@param aSourceLine
@return
*/
TBool CLocalImRecvConvert::CheckUUEStartL(const TDesC8& aSourceLine)
	{
	// Checks if the descriptor contains the UUE begin header
	// Extracts the file name if it is
	
	TInt sourceLength = aSourceLine.Length();
	if(sourceLength < KImcvUueStart().Length()+3) // can't be it, it's not long enough
		return EFalse;

	if(!aSourceLine.Left(KImcvUueStart().Length()).CompareF(KImcvUueStart)) // start of line might be UUE boundary
		{
		// we also need to check that the next three chars are numbers - Unix file access code
		const TUint8* _ptr = aSourceLine.Ptr();
		TInt length=KImcvUueStart().Length();// this defines length as 6 ie. "b e g i n  " 
		if(TChar(_ptr[length]).IsDigit() && TChar(_ptr[length+1]).IsDigit() && TChar(_ptr[length+2]).IsDigit())
			{
			// we've found 'begin ###' at the start of a line -
			// that's about as good as we can do 
			// now grab the file name and paste it into the document
			// Extract filename from string, removing any surrounding quote marks

			HBufC16* pBuf16 = HBufC16::NewLC(aSourceLine.Length());
			pBuf16->Des().Copy(aSourceLine);
			TLex sourceLineLex = pBuf16->Ptr();
			//parse until start of filename and mark
			length+=3;					// length (init'd to 6 above) now equals 9 ie. "begin ###"
			sourceLineLex.Inc(length);	// skips "begin ###"
			sourceLineLex.SkipSpace();	// skips any leading whitespace

			ExtractFilename(sourceLineLex, iAttachmentName);
			CleanupStack::PopAndDestroy(); // pBuf8
			return ETrue;
			}
		}
		
	return EFalse;
	}

  
/**
AddFileExtension()
*/
void CLocalImRecvConvert::AddFileExtension()
	{
	switch (iMimeParser->ContentType())
		{
		case EMimeText:
			// Add on extension to make opening file from email editor possible.

			if ( iMimeParser->ContentSubType()==KImcvHtml )
				iAttachmentName.Append(KHtmlExtension);
			else if (iMimeParser->VCard() || iMimeParser->VCalendar())
				iAttachmentName.Append(KVCardExtension);
			else //if ( iMimeParser->ContentSubType()==KImcvPlain)
				iAttachmentName.Append(KTextExtension);
			break;
		case EMimeImage:
		case EMimeAudio:
		case EMimeVideo:
			if ( (iMimeParser->ContentSubType()==KImcvBmp) 
					|| (iMimeParser->ContentSubType()==KImcvGif)
					|| (iMimeParser->ContentSubType()==KImcvJpeg)
					|| (iMimeParser->ContentSubType()==KImcvTiff)
					|| (iMimeParser->ContentSubType()==KImcvWav) )
				{
				TBuf<KMaxExtensionLength> buf;
				buf.Copy(iMimeParser->ContentSubType());
				iAttachmentName.Append(KImcvFullStop);
				iAttachmentName.Append(buf);
				}
			break;
		default:
			break;
		} // End switch
	}

  
/**
WriteToBodyL()

@param aText
@param aBlankLine
*/
void CLocalImRecvConvert::WriteToBodyL(const TDesC8& aText, TBool aBlankLine)
	{
	RECVLOG(KWritingToBody)

	if(aText.Length() && aText[aText.Length()-1]==CEditableText::ELineBreak )
		RECVLOG(KLineHasLineBreak)
	TInt pos = iOutputBody->DocumentLength();

	// Add bits of body text, converting along the way, till no characters left
	// .. to convert.

	if(aBlankLine)
		{
		RECVLOG(KBlankLine);
		iOutputBody->InsertL(pos, CEditableText::EParagraphDelimiter);
		pos++;
		return;
		}

	// Convert text before writing to body.
	TInt rem = 0;
	HBufC16* text16=HBufC16::NewLC(aText.Length());
	TPtr16 ptr16(text16->Des());
	TInt unconvertedChars, firstPos; // not used 
	rem = iCharConv->ConvertToOurCharsetL(aText, ptr16, unconvertedChars, firstPos);
	if (rem < 0) // error
		{
		// Copy unconverted characters.
		Append(ptr16, aText);
		iOutputBody->InsertL(pos, ptr16);
		CleanupStack::PopAndDestroy(); // text16
		return;
		}
	else if (rem && rem < KConversionRemainderLength)
		iLeftOver.Copy(aText.Right(rem));	
	
	// Make sure that the line is not CRLF terminated 
	// - replace with a line break if necessary.

	TInt length = ptr16.Length();
	switch(iAlgorithm)
		{
		case EBase64Decode:
		case EUUDecode:
			{
			// Check for CRLF throughout the string.

			if (!length)
				break; // String length zero.

			if (iEncounteredLineEndingInCarriageReturn)
				{
				pos--; // overwrite the stored CR.
				ptr16[0] = CEditableText::ELineBreak;
				}
			iEncounteredLineEndingInCarriageReturn = ptr16[length-1]==KImcvCR ? ETrue:EFalse;

			TInt start = 0;
			TInt offset = ptr16.Find(KImcvCRLF16);

			while (offset != KErrNotFound)
				{
				ptr16[offset] = CEditableText::ELineBreak;
				const TDesC& buf = ptr16.Mid(start, offset-start+1);
				iOutputBody->InsertL(pos, buf);
				pos += buf.Length();
				start=offset+2; // Skip the LF char.
				offset = ptr16.Find(KImcvCRLF16);
				} 

			if (start<length)
				{
				const TDesC& buf = ptr16.Right(length-start);
				iOutputBody->InsertL(pos, buf);
				pos += buf.Length();
				}
			}
			break;

		case EQPDecode:
		case ENoAlgorithm:
		default:
			// Check for CRLF at end of line.
			if(length>=2 && ptr16[length-2]==KImcvCR && ptr16[length-1]==KImcvLF)
				{
				ptr16[length-2] = CEditableText::ELineBreak;
				ptr16.SetLength(length-1);
				}	

			const TDesC& buf = text16->Des();
			iOutputBody->InsertL(pos, buf);
			pos += buf.Length();
		}	

	CleanupStack::PopAndDestroy(text16);
	
	RECVLOG(KWroteToBody)
	}


/**
WriteToBodyL()

@param aText
*/
void CLocalImRecvConvert::WriteToBodyL(const TDesC16& aText)  
	{
	RECVLOG(KWritingToBody)
	if (aText.Length() && aText[aText.Length()-1]==CEditableText::ELineBreak)
		RECVLOG(KLineHasLineBreak)

	TInt pos = iOutputBody->Read(0).Length();
	// get the text in before the paragraph marker that's always there
	pos = pos-1 < 0 ? 0 : pos-1;
	iOutputBody->InsertL(pos,aText);
	
	RECVLOG(KWroteToBody)
	}

  
/**
ParseRecipientListL()

@param aList
*/
void CLocalImRecvConvert::ParseRecipientListL(CDesCArray& aList)
	{
	HBufC8* pBuf = HBufC8::NewLC(KMaxIMailHeaderReadLineLength);
	TPtrC8 source(iRfc822Token->OutputLine()->Ptr(), iRfc822Token->OutputLine()->Length());
	const TUint8* ptr = source.Ptr();
	TUint8 lookFor = 0;
	TInt count = 0;
	TBool finishedEntry = EFalse;

	// get past white space
	while(*ptr&&((*ptr==KImcvSP)||(*ptr==KImcvSemiColon))) ptr++;

	while(*ptr&&((ptr-source.Ptr())<(source.Length())))
		{
		if(!pBuf->Length())
			{
			finishedEntry = EFalse;
			}

		switch(*ptr)
			{
			case KImcvLeftBracket:
				if(lookFor==KImcvRightBracket)
					count++;
				else
					{
					lookFor = KImcvRightBracket;
					count = 1;
					}
				break;
			case KImcvLeftChevron:
				if(lookFor==KImcvRightChevron)
					count++;
				else
					{
					lookFor = KImcvRightChevron;
					count = 1;
					}
				break;
			case KImcvDoubleQuote:
 				lookFor = (TUint8)(lookFor==KImcvDoubleQuote ? 0 : KImcvDoubleQuote);
				count = (lookFor ? 1 : 0);
				break;
			case KImcvRightBracket:
			case KImcvRightChevron:
				count -= (*ptr == lookFor ? 1 : 0);
				lookFor = (TUint8)((*ptr == lookFor)&&(count == 0) ? 0 : lookFor);
				break;
			case KImcvComma:
			case KImcvSemiColon:
				finishedEntry = !lookFor;
				break;
			}

		if(!finishedEntry)
			{
			// check we're not about to blow the buffer
			if(pBuf->Length() >= pBuf->Des().MaxLength())
				{
				pBuf = pBuf->ReAlloc(pBuf->Length() + 64); // arbitrary extension
				}
			pBuf->Des().Append((TChar)*ptr);
			// move to the next character
			ptr++;
			}
		else
			{
			// that's it! store the address away
			HBufC16* pBuf16 = HBufC16::NewLC(pBuf->Des().Length());
			pBuf16->Des().Copy(pBuf->Des());
			aList.AppendL( (HBufC16&) *pBuf16 );
			CleanupStack::PopAndDestroy(); // pBuf16
			pBuf->Des().SetLength(0);

			// get past the separator
			ptr++;

			// get past white space (& any other separators)
			while(*ptr && (*ptr==KImcvSP || *ptr==KImcvTab || *ptr==KImcvComma || *ptr==KImcvSemiColon)) ptr++;
			}
		}
		// catch the last name in the list
		if (pBuf)
			{
			TInt recipientLength = pBuf->Length();
			if (recipientLength)
				{
				HBufC16* pBuf16 = HBufC16::NewLC(recipientLength);
				pBuf16->Des().Copy(*pBuf);
				aList.AppendL(*pBuf16);
				CleanupStack::PopAndDestroy(); // pBuf16
				}
			}

		CleanupStack::PopAndDestroy(); // pBuf
	}


/**
ExtractFilename()

@param aLex
@param rFileName
*/
void CLocalImRecvConvert::ExtractFilename(TLex& aLex, TDes& rFileName)
	{
	// This function steps through the filename extracting the bare name and checking 
	//  the length is less than the max of 256 for EPOC ;checks that all chars are legal for EPOC32
	
	TChar endChar = KImcvSemiColon;

	aLex.SkipSpace();
	
	if (aLex.Peek()==KImcvDoubleQuote)
		{
		aLex.Inc();	// step over the " character
		endChar = KImcvDoubleQuote;
		}

	aLex.Mark();	// marks where we are as this is the first char of the filename
	
	TInt fileNameLength = 0;
	TInt maxFileNameLength = rFileName.MaxLength();

	while(!aLex.Eos() && aLex.Peek()!=endChar && aLex.Peek()!=KImcvCR && fileNameLength < maxFileNameLength)	
		//spools through the string until the end and marks char before quote (such that 
		//  it extracts only the filename), EOS or before the maximum buffer length is exceeded 
		{
		fileNameLength++;
		aLex.Inc();
		}

	TPtrC marked = aLex.MarkedToken(); 
	rFileName.Copy(marked);
	
	ReplaceInvalidCharacters(rFileName);
	}


/**
SetAttachmentName()

@param aFileName
*/
void CLocalImRecvConvert::SetAttachmentName(TDes& aFileName)
	{
	ReplaceInvalidCharacters(aFileName);
	iAttachmentName.Zero();

	TUint delimiter = '.';
	TInt  maxLength = iAttachmentName.MaxLength();
	
	__ASSERT_DEBUG(
		maxLength >= aFileName.Length(), gPanic(KPanicReadLengthTooLarge)
		);

	iAttachmentName.Copy(aFileName);
	TInt attachmentLen = iAttachmentName.Length();
	if (attachmentLen == 0)
		iAttachmentName = *iDefaultAttachmentName;
	else if (iAttachmentName[0] == delimiter && maxLength >= attachmentLen + iDefaultAttachmentName->Length())
		iAttachmentName.Insert(0, *iDefaultAttachmentName);
	}

	
/**
ReplaceInvalidCharacters()

@param rFileName
*/
void CLocalImRecvConvert::ReplaceInvalidCharacters(TDes& rFileName)
	{
	TInt length = rFileName.Length();
	for(TInt index=0; index < length; index++)
		{
		//parse extracted filename and replace any illegal chars with a default.

		if(IsIllegalChar((TUint)rFileName[index]))
			rFileName[index] = KImcvDefaultChar;
		}
	}


/**
StoreEntryStreamsL()
*/
void CLocalImRecvConvert::StoreEntryStreamsL()	
	{
	StoreEntryStreamsL(KStoreBodyText|KStore822Header|KStoreMIMEHeader);
	}


/**
StoreEntryStreamsL()

@param aSettings
*/
void CLocalImRecvConvert::StoreEntryStreamsL(TInt aSettings)	
	{
	RECVLOG(KStartStoringEntryStream);
	if (iReceivingHeadersOnly==EFalse)
		{
		CMsvStore* entryStore = NULL;
		TBool commitStore = EFalse;
		TRAPD(error, entryStore = iServerEntry->EditStoreL());	
		if(error==KErrNone) // if store does not exist then the entry is the wrong type
			{
			CleanupStack::PushL(entryStore);

			if (aSettings & KStore822Header)
				Store822HeaderL(*entryStore, commitStore);
			
			if (aSettings & KStoreMIMEHeader)
				StoreMIMEHeaderL(*entryStore, commitStore);

			if (aSettings & KStoreBodyText)
				StoreBodyTextL(*entryStore, commitStore);

			// only commit to the store if I wrote something into it
			if (commitStore)
				{
				RECVLOG(KStoringEntryStream);
				entryStore->CommitL();
				}

			StoreEntryDataL();
			CleanupStack::PopAndDestroy(); //entryStore	
			}
		
		}
	RECVLOG(KDoneStoringEntryStream);
	}


/**
Store822HeaderL()

@param aStore
@param aCommit 
*/
void CLocalImRecvConvert::Store822HeaderL(CMsvStore& aStore, TBool& aCommit)	
	{
	if(iEmptyHeaderSize<(iOutputHeader->DataSize()))
		{
		iEntryDataSize += iOutputHeader->DataSize()-iEmptyHeaderSize;
		RECVLOG(KStoringHeader);
		iOutputHeader->StoreL(aStore);	
		RECVLOG(KStoredHeader);
		aCommit = ETrue;
		}
	}


/**
StoreMIMEHeaderL()

@param aStore
@param aCommit
*/
void CLocalImRecvConvert::StoreMIMEHeaderL(CMsvStore& aStore, TBool& aCommit)	
	{
	if(iMimeParser->MimeHeaderSize())
		{
		RECVLOG(KStoringMIMEHeader);
 		iMimeParser->StoreMimeHeaderWithoutCommitL(aStore);
		aCommit = ETrue;
		RECVLOG(KStoredMIMEHeader);
		}
	}


/**
StoreBodyTextL()

@param aStore
@param aCommit
*/
void CLocalImRecvConvert::StoreBodyTextL(CMsvStore& aStore, TBool& aCommit)	
	{
	if (iStore8BitData)
		{
		if(iBodyBuf->Size())
			{
			iEntryDataSize += iBodyBuf->Size();
			RECVLOG(KStoring8BitBody);
			iBodyText->StoreL(aStore, *iBodyBuf);
			aCommit = ETrue;
			RECVLOG(KStored8BitBody);
			}
		}
	else
		{
		if(iOutputBody->DocumentLength())
			{
			iEntryDataSize += iOutputBody->DocumentLength();
			RECVLOG(KStoringBody);
			aStore.StoreBodyTextL(*iOutputBody);
			aCommit = ETrue;
			RECVLOG(KStoredBody);
			}
		}
	}


/**
StoreEntryDataL()

@return
*/
TBool CLocalImRecvConvert::StoreEntryDataL()
	{
	// NB function should only be called if a whole email is being processed
	TBool commit=EFalse;
	RECVLOG(KUpdatingEntry)

	if (iEmailEntry->iType==KUidMsvMessageEntry)
		iParent->At(0).iSize += iEntryDataSize;
	else
		{
		iEmailEntry->iSize += iEntryDataSize;
		if (iEntryType==EAttachmentEntry || iEntryType==EHtmlEntry)
			iEmailEntry->iDetails.Set(iAttachmentName);
		}
	User::LeaveIfError(iServerEntry->ChangeEntry(*iEmailEntry));

	RECVLOG(KUpdatedEntry)
	return commit;	// if I wrote data into the store, tell owner	
	}


/**
MoveToParentEntryL()
*/
void CLocalImRecvConvert::MoveToParentEntryL()
	{
	// This function changes the context to the current entry's parent entry.
	RECVLOG(KMoveToParentEntry)

	// Change context to the parent entry	
	if (EntryId()==iTopMessagePart)
		return; // Already there.

	User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().Parent())); 

	iNewEntry = EFalse;
	
	// only read and write to store if this is a real email message; headers stored in
	// the remote mailbox do not require any store information.
	if (iReceivingHeadersOnly)
		{
		RECVLOG(KIgnoringStreams)
		return;
		}

	TBool allowAttachmentFlag=ETrue;
	if(iServerEntry->Entry().iType == KUidMsvFolderEntry)
		{
		// Current entry is a folder entry signifying a MIME multipart.
		// Change context to the parent entry
		
		TMsvEmailEntry entry = (TMsvEmailEntry) iServerEntry->Entry();
		iCurrentMultipartFolderEntryId = EntryId();
		allowAttachmentFlag = !(entry.MessageFolderType()==EFolderTypeRelated || 
								entry.MessageFolderType()==EFolderTypeAlternative);
	

		if (EntryId()!=iTopMessagePart)
			User::LeaveIfError(iServerEntry->SetEntry(iServerEntry->Entry().Parent())); 
		}

	TBool childIsAttachment = (iEmailEntry->Attachment() || iEmailEntry->iType == KUidMsvMessageEntry) ? ETrue:EFalse;
	TBool childIsMHTML=EFalse;
	// Dont want the flag propogated 'up' past a message entry.
	if(iEmailEntry->iType != KUidMsvMessageEntry)
		childIsMHTML = iEmailEntry->MHTMLEmail() ? ETrue:EFalse;

	//Make the parent entry the current entry 
	if (NULL!=iEmailEntry)
		{
		delete iEmailEntry;
		iEmailEntry=NULL;
		}
	iEmailEntry = new (ELeave) TMsvEmailEntry(iServerEntry->Entry());

	if (!iParent->Count())
		{
		TParentDetails parentDetails;
		iParent->InsertL(0,parentDetails);
		}

	if (! iParent->At(0).iAttachment)
		{
		iParent->At(0).iAttachment=(childIsAttachment && allowAttachmentFlag)? ETrue:EFalse;

		// if we aren't allowing attachments because of the folder type
		// remember there where attachments and check at the end whether
		// it was an MHTML message or not.
		if(childIsAttachment && !allowAttachmentFlag)
			{
			iRelatedAttachments=ETrue;
			}
		}
	if (!iParent->At(0).iMHTML)
		iParent->At(0).iMHTML = childIsMHTML ? ETrue:EFalse;
	iParent->At(0).iSize += iEntryDataSize;

	iOutputHeader->Reset();
	iEmptyHeaderSize=iOutputHeader->DataSize();
	iMimeParser->ResetForNewEntry();

	iEntryDataSize=0;

	RECVLOG(KMovedToParentEntry)
	}

/**
WritePartialFooterL()
Helper function to add the partial footer to the email if it exists

@param aAmountLeft
*/
void CLocalImRecvConvert::WritePartialFooterL(TInt aAmountLeft)
	{
	TMsvId msgId=iBodyId;
	if (msgId==KMsvNullIndexEntryId)
		return;
	TMsvId id = iServerEntry->Entry().Id();
	if (iServerEntry->SetEntry(msgId)==KErrNone)
		{
		TBool storePresent = iServerEntry->HasStoreL();
		if (storePresent && iPartialEmailFooter->Length()>0 && aAmountLeft>0)
			{
			CMsvStore* store = iServerEntry->ReadStoreL();
			CleanupStack::PushL(store);
			if (store->HasBodyTextL())
				{
				iOutputBody->Reset();
				store->RestoreBodyTextL(*iOutputBody);
				CleanupStack::PopAndDestroy(store);
				HBufC* msg=NULL;
				if (iPartialEmailFooter->Find(KIntegerKey)!=KErrNotFound)
					{
					// display k left on the server, rounded up if between 1 and 1023 bytes
					TInt kBytesLeft = aAmountLeft / 1024;
					if(kBytesLeft == 0)
						kBytesLeft = 1;
					msg = HBufC::NewLC(iPartialEmailFooter->Length()+KSpaceToAddNumber);
					msg->Des().Format(*iPartialEmailFooter,kBytesLeft);
					}
				else
					{
					msg = iPartialEmailFooter->AllocLC();
					}
				iOutputBody->AppendParagraphL();
				TInt length = iOutputBody->DocumentLength();
				iOutputBody->InsertL(length,*msg);
				CleanupStack::PopAndDestroy(msg);
				store=NULL;
				store = iServerEntry->EditStoreL();	
				
				CleanupStack::PushL(store);
				store->StoreBodyTextL(*iOutputBody);
				store->Commit();
				CleanupStack::PopAndDestroy(store);
				}
			else
				{
				CleanupStack::PopAndDestroy(store);
				}
			}
		}
	iServerEntry->SetEntry(id);
	}


/**
CreateEntryL()
*/
void CLocalImRecvConvert::CreateEntryL()
	{
	RECVLOG(KCreatingEntry);
	if (NULL!=iEmailEntry)
		{
		delete iEmailEntry;
		iEmailEntry=NULL;
		}

	if (iCurrentMultipartFolderEntryId)
		{
		User::LeaveIfError(iServerEntry->SetEntry(iCurrentMultipartFolderEntryId));
		iCurrentMultipartFolderEntryId=0;
		}
		
	iEmailEntry = new (ELeave) TMsvEmailEntry;

	TValidEntryType previousEntryType = iEntryType;
	if (!iTopMessagePart || iMIMEPart_822Header)
		{
		// At the main header, want to create a message entry.
		// The stored iEntryType will indicate ( from header info) the next entry to be created.
		// Save temporarily.
		// Also applies to the special case where a message contains ony 1 embedded message.

		previousEntryType=iEntryType;
		iEntryType=EMessageEntry;
		}

	if ((iPopulateMessage) && (!iTopMessagePart))
	// If this is the root of a message that is being populated then do not create it.
		{
		User::LeaveIfError(iServerEntry->SetEntry(iRootEntryId));
		*iEmailEntry = iServerEntry->Entry();
		iTopMessagePart=iRootEntryId;

		// Delete all the children of the message entry.  This is needed because if the
		// message has been purged, the entries will still exist.  When the message is populated,
		// new entries are created.  If the original entries are not removed, then duplicate 
		// entries will exist.
		CMsvEntrySelection*	children = new(ELeave) CMsvEntrySelection;
		CleanupStack::PushL(children);
		User::LeaveIfError(iServerEntry->GetChildren(*children));
		if (children->Count())
			iServerEntry->DeleteEntries(*children);
		CleanupStack::PopAndDestroy(children);
		}
	else
		{
		iEmailEntry->iMtm=iNewMsgType;
		iEmailEntry->iServiceId = iEmailServiceId;
		iEmailEntry->SetComplete(EFalse);
		iEmailEntry->iSize = 0;
		iEmailEntry->SetVisible(ETrue);
		iEmailEntry->SetInPreparation(EFalse);
		iEmailEntry->SetReceipt(EFalse);

		iEmailEntry->SetVCard(iMimeParser->VCard());
		iEmailEntry->SetVCalendar(iMimeParser->VCalendar());
		iEmailEntry->SetMessageFolderType(iMimeParser->MessageFolderType());
		iEmailEntry->SetPriority(iImPriority);
		iEmailEntry->SetNew(EFalse);

		if(iOutputHeader->ReceiptAddress().Length()>0)
			iEmailEntry->SetReceipt(ETrue);

		iEmailEntry->iDate=iTimeDate;
		switch(iEntryType)
			{
			case EMessageEntry:
				if(!iTopMessagePart)
					{
					iEmailEntry->SetUnread(ETrue);
					iEmailEntry->SetNew(ETrue);
					iEmailEntry->SetVisible(EFalse);
					iEmailEntry->SetInPreparation(ETrue);
					iEmailEntry->SetSendingState(KMsvSendStateNotApplicable);
					}
				else
					{
					TParentDetails parentDetails;
					parentDetails.iMHTML=EFalse;
			        parentDetails.iAttachment=EFalse;
					parentDetails.iSize=0;
					iParent->InsertL(0,parentDetails);
					}
				iEmailEntry->iType=KUidMsvMessageEntry;
				iEmailEntry->iDetails.Set(iOutputHeader->From());
				iEmailEntry->iDescription.Set(iOutputHeader->Subject());
				break;
			case EFolderEntry:
				iEmailEntry->iType=KUidMsvFolderEntry;
				if (iMimeParser->MessageFolderType()==EFolderTypeUnknown)
					{
					// Get folder type of parent (the message)
					TMsvEmailEntry entry=iServerEntry->Entry();
					iEmailEntry->SetMessageFolderType(entry.MessageFolderType());
					}
				break;
			case EAttachmentEntry:
				iEmailEntry->iType=KUidMsvAttachmentEntry;
				iEmailEntry->iDetails.Set(iAttachmentName);
				iEmailEntry->iDescription.Set(iMimeParser->ContentDescription());
				break; 
			case ETextEntry:
				if ( iMimeParser->ContentDisposition()!=KImcvAttachment)
					iEmailEntry->iType=KUidMsvEmailTextEntry;
				else 
					{
					iEmailEntry->iType=KUidMsvAttachmentEntry;
					iEmailEntry->iDetails.Set(iAttachmentName);
					iEmailEntry->iDescription.Set(iMimeParser->ContentDescription());
					}
				break;
			case EHtmlEntry:
				iEmailEntry->iType=KUidMsvEmailHtmlEntry;
				// If disposition not set or is inline..
				if ( iMimeParser->ContentDisposition()==KImcvAttachment)
					iEmailEntry->iType=KUidMsvAttachmentEntry;
				else
					iEmailEntry->SetMHTMLEmail(ETrue);
				iEmailEntry->iDetails.Set(iAttachmentName);
				iEmailEntry->iDescription.Set(iMimeParser->ContentDescription());
				break;
			default:
				iEmailEntry->iType=KUidMsvAttachmentEntry;
				iEmailEntry->iDetails.Set(iAttachmentName);
				iEmailEntry->iDescription.Set(iMimeParser->ContentDescription());
			}

		User::LeaveIfError(iServerEntry->CreateEntry(*iEmailEntry));
		User::LeaveIfError(iServerEntry->SetEntry(iEmailEntry->Id()));
		if(!iTopMessagePart)
			iTopMessagePart=iEmailEntry->Id();

		//if (iEntryType==EHtmlEntry && iAttachmentFileState!=EFileIsOpen)
		//	CreateAttachmentL();
		}

	iEntryType=previousEntryType;
	iNewEntry = ETrue;
	RECVLOG(KCreatedEntry);
	}


/**
Logging()

@param aString1
@param aString2
*/
void CLocalImRecvConvert::Logging(const TDesC8& aString1, const TDesC8& aString2)  
	{
	TBuf8<1024> aBuf(aString1);

	aBuf.Append(aString2);
	RECVLOG(aBuf);
	}


/**
StoreMessageEntryDetailsL()
*/
 void CLocalImRecvConvert::StoreMessageEntryDetailsL()  
	{
	iEmailEntry->SetAttachment(iParent->At(0).iAttachment);
	iEmailEntry->SetMHTMLEmail(iParent->At(0).iMHTML);

	if(iEmailEntry->MHTMLEmail() == EFalse && iEmailEntry->Attachment() == EFalse && iRelatedAttachments !=EFalse)
		{
		iEmailEntry->SetAttachment(ETrue);
		}
	iRelatedAttachments=EFalse;
	
	iEmailEntry->iSize=iParent->At(0).iSize;
	iEmailEntry->SetMessageFolderType(iParent->At(0).iFolder);
	StoreEntryDataL();

	if (iParent->Count()>1)
		{
		iParent->At(1).iSize+=iEmailEntry->iSize;
		iParent->Delete(0);
		}
	else
		{
		iParent->At(0).iAttachment=iParent->At(0).iMHTML=EFalse;
		iParent->At(0).iSize=0;
		}
	}


/**
DeletedAttachmentSize()

@return
*/ 
 TInt CLocalImRecvConvert::DeletedAttachmentSize()  
	{
	return iSizeOfAttachmentsRemoved;
	}

/****************************************************************************
	Class CLocalMimeParser functions
*****************************************************************************/

/**
NewLC()
Static factory constructor. Uses two phase construction 
and pushes the newly created object into the Cleanup stack.

@param aImRecvConvert
@return
*/    
CLocalMimeParser* CLocalMimeParser::NewLC(CLocalImRecvConvert& aImRecvConvert)  
	{
	CLocalMimeParser* self = new (ELeave) CLocalMimeParser(aImRecvConvert);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/**
NewL()
Static factory constructor. Uses two phase construction.

@param aImRecvConvert
@return
*/
 CLocalMimeParser* CLocalMimeParser::NewL(CLocalImRecvConvert& aImRecvConvert)  
	{
	CLocalMimeParser* self = CLocalMimeParser::NewLC(aImRecvConvert);
	CleanupStack::Pop();
	return self;
	}


/**
CLocalMimeParser()

@param aImRecvConvert
@return
*/
CLocalMimeParser::CLocalMimeParser(CLocalImRecvConvert& aImRecvConvert): iImRecvConvert(aImRecvConvert), 
											iStartId(NULL)

	{
	__DECLARE_NAME(_S("CLocalMimeParser"));
	}


/**
ConstructL()
*/
void CLocalMimeParser::ConstructL() 
	{
	iMimeHeader = CImMimeHeader::NewL();
	// Create a Desc array to store the boundary strings of a Mime message
	iBoundaryText = new (ELeave) CDesC8ArrayFlat(3);
	
	// Set charset default value
	iDefaultCharset=iImRecvConvert.CharacterConverter().DefaultCharset();
	iCharset = iDefaultCharset;
	ResetMimeFieldsExist();

	Reset();
	}


/**
~CLocalMimeParser()
*/
CLocalMimeParser::~CLocalMimeParser() 
	{
	delete iMimeHeader;
	iMimeHeader=NULL;
	delete iBoundaryText;
	iBoundaryText=NULL;
	delete iMimeHeaderLine;
	iMimeHeaderLine=NULL;
	delete iStartId;
	iStartId=NULL;
	}


/**
Reset() 
*/
void CLocalMimeParser::Reset() 
	{
	iBoundaryText->Reset();
	iBoundaryIndex = 0;
	iBoundaryLength = 0;
	isMime = EFalse;
	
	ResetForNewEntry();
	}


/**
ResetForNewEntry()
*/
void CLocalMimeParser::ResetForNewEntry()
	{
	iMimeHeader->Reset();
	iEmptyMimeHeaderSize=iMimeHeader->Size();
		
	iContentType = EMimeUnknownContent;
	iContentEncoding = EEncodingTypeNone;
	iContentDescription.Zero();
	iVCard = EFalse;
	iVCalendar = EFalse;
	iStartPart=EFalse;
	iMessageFolderType = EFolderTypeUnknown;
	iTerminatingBoundary = EFalse;
	iBoundaryFound = EFalse;
	}


/**
RestoreMimeParserL()

@param entryStore
*/
void CLocalMimeParser::RestoreMimeParserL(CMsvStore& entryStore)
	{
	iMimeHeader->RestoreL(entryStore);
	if(iMimeHeader->ContentType().Compare(KImcvText)==0)
		iContentType=EMimeText; 
	else
	if(iMimeHeader->ContentType().Compare(KImcvMessage)==0)
		iContentType=EMimeMessage;
	else
	if(iMimeHeader->ContentType().Compare(KImcvMultipart)==0)
		iContentType=EMimeMultipart; 
	else
	if(iMimeHeader->ContentType().Compare(KImcvImage)==0)
		iContentType=EMimeImage; 
	else
	if(iMimeHeader->ContentType().Compare(KImcvApplication)==0)
		iContentType=EMimeApplication; 
	else
	if(iMimeHeader->ContentType().Compare(KImcvAudio)==0)
		iContentType=EMimeAudio; 
	else
	if(iMimeHeader->ContentType().Compare(KImcvVideo)==0)
		iContentType=EMimeVideo; 
	else
		iContentType=EMimeUnknownContent; 

	}


/**
ParseLineL()

@param aSourceLine
*/
void CLocalMimeParser::ParseLineL(const TDesC8& aSourceLine)
	{
	if(iMimeHeaderLine==NULL)
		{
		iMimeHeaderLine = HBufC8::NewL(aSourceLine.Length());
		*iMimeHeaderLine = aSourceLine;
		}
	
	iLex = *iMimeHeaderLine;

	// find out whether the current line has anything to do with currently understood MIME Content tokens
	if(!iMimeHeaderLine->MatchF(KImcvMime) || !iMimeHeaderLine->MatchF(KImcvContent))
		{ 
		if(MatchAndRemoveToken(KImcvMimePrompt))
			DoMimeVersion();
		else if(MatchAndRemoveToken(KImcvContentType))
			DoContentTypeL();
		else if(MatchAndRemoveToken(KImcvContentLocation))
			DoContentLocationL();
		else if(MatchAndRemoveToken(KImcvContentTransferEncoding))
			DoEncodingL();
		else if(MatchAndRemoveToken(KImcvContentId))
			{
			RemoveSurroundingCharacters(KImcvLeftChevron, KImcvRightChevron, *iMimeHeaderLine);
			DoContentIdL();
			}
		else if(MatchAndRemoveToken(KImcvContentDescription))
			DoDescriptionL();
		else if(MatchAndRemoveToken(KImcvContentDisposition))
			DoDispositionL();
		else if(MatchAndRemoveToken(KImcvContentBase))
			DoContentBaseL();
		}
		
	delete iMimeHeaderLine;	// clean up and null pointer iff CompleteMimeHeader and no foldover append req'd
	iMimeHeaderLine=NULL;
	}


/**
DoMimeVersion()
*/
void CLocalMimeParser::DoMimeVersion()
	{
	// extract the MIME version from a header line which we already know
	// has 'MIME-Version' start of it.
	if(MatchAndRemoveToken(KImcvMimeVersion))
		iCorrectMimeVersion = ETrue;
	}


/**
DoContentIdL()
*/
void CLocalMimeParser::DoContentIdL()
	{
	iMimeHeader->SetContentIDL(iMimeHeaderLine->Des());
	if(iStartId && iStartId->CompareF(iMimeHeaderLine->Des())==KErrNone)
		iStartPart=ETrue;
	}


/**
DoContentLocationL()
*/
void CLocalMimeParser::DoContentLocationL()
	{
	TInt len = (*iMimeHeaderLine).Length();
	if (len == 0)
		return;
			
	RemoveSurroundingCharacters(KImcvQuote, KImcvQuote, *iMimeHeaderLine);

	HBufC16* locationBuf = HBufC16::NewL( len );
	CleanupStack::PushL(locationBuf);
	TPtr locationPtr(locationBuf->Des());
	iImRecvConvert.iHeaderConverter->DecodeHeaderFieldL( iMimeHeaderLine->Des(), locationPtr);
	iMimeHeader->SetContentLocationL(locationPtr);
	CleanupStack::PopAndDestroy(); // locationBuf
	}


/**
DoContentBaseL()
*/
void CLocalMimeParser::DoContentBaseL()
	{
	RemoveSurroundingCharacters(KImcvQuote, KImcvQuote, *iMimeHeaderLine);
	iMimeHeader->SetContentBaseL(iMimeHeaderLine->Des());
	}


/**
DoAttachmentTypeL()
*/
void CLocalMimeParser::DoAttachmentTypeL()
	{
	iImRecvConvert.iEntryType = CLocalImRecvConvert::EAttachmentEntry;
	iImRecvConvert.iCurrentPartIsRichText = EFalse;	

	if(MatchAndRemoveToken(KImcvForwardSlash))
		{
		if(MatchAndRemoveToken(KImcvBmp))
			{
			iMimeHeader->SetContentSubTypeL(KImcvBmp);
			}
		else
		if(MatchAndRemoveToken(KImcvGif))
			{
			iMimeHeader->SetContentSubTypeL(KImcvGif);
			}
		else
		if(MatchAndRemoveToken(KImcvJpeg))
			{
			iMimeHeader->SetContentSubTypeL(KImcvJpeg);
			}
		else
		if(MatchAndRemoveToken(KImcvTiff))
			{
			iMimeHeader->SetContentSubTypeL(KImcvTiff);
			}
		else
		if(MatchAndRemoveToken(KImcvWav))
			{
			iMimeHeader->SetContentSubTypeL(KImcvWav);
			}
		else
		if(MatchAndRemoveToken(KImcvZip))
			{
			iMimeHeader->SetContentSubTypeL(KImcvZip);
			}
		else
		if(MatchAndRemoveToken(KImcvOctetStream))
			{
			iMimeHeader->SetContentSubTypeL(KImcvOctetStream);
			}
		else
		if(MatchAndRemoveToken(KImcvExe))
			{
			iMimeHeader->SetContentSubTypeL(KImcvExe);
			}
		else
		if(MatchAndRemoveToken(KImcvCmd))
			{
			iMimeHeader->SetContentSubTypeL(KImcvCmd);
			}
		else
		if(MatchAndRemoveToken(KTImcvDrm))
			{ // added this to handle DRM files
			iMimeHeader->SetContentSubTypeL(KTImcvDrm);
			}
		}
	}


/**
DoMessageTypeL()
*/
void CLocalMimeParser::DoMessageTypeL()
	{
	iImRecvConvert.iEntryType = CLocalImRecvConvert::EMessageEntry;
	iImRecvConvert.iEmailPart = CLocalImRecvConvert::KParentPart;
	
	iContentType=EMimeMessage; 
	iMimeHeader->SetContentTypeL(KImcvMessage);

	if(MatchAndRemoveToken(KImcvForwardSlash))
		{
		if(MatchAndRemoveToken(KImcvRfc822))
			{
			//iMessageFolderType=EFolderTypeRFC822;
			iMimeHeader->SetContentSubTypeL(KImcvRfc822);
			}
		else if(MatchAndRemoveToken(KImcvExternal))
			{
			iMessageFolderType=EFolderTypeExternal;
			iMimeHeader->SetContentSubTypeL(KImcvExternal);
			}
		else if(MatchAndRemoveToken(KImcvPartial))
			{
			iMessageFolderType=EFolderTypePartial;
			iMimeHeader->SetContentSubTypeL(KImcvPartial);
			}
		else if(MatchAndRemoveToken(KImcvDeliveryStatus))
			{
			// We do not process this part. So store as text.
			iMimeHeader->SetContentSubTypeL(KImcvDeliveryStatus);
			iImRecvConvert.iEntryType = CLocalImRecvConvert::EAttachmentEntry;
			iImRecvConvert.iEmailPart = CLocalImRecvConvert::KNoPart;
			iContentType=EMimeUnknownContent; 
			iImRecvConvert.iCurrentPartIsRichText=EFalse;
			iImRecvConvert.iAttachmentName.Copy(KImcvDeliveryStatus);
			iImRecvConvert.iAttachmentName.Append(KTextExtension);
			}
		else
			{
			iMessageFolderType=EFolderTypeUnknown;
			iMimeHeader->SetContentSubTypeL(KImcvUnknown);
			}
		}	
	}


/**
DoMultipartTypeForNonMIMEL()
*/
void CLocalMimeParser::DoMultipartTypeForNonMIMEL()
	{
	ResetForNewEntry();
	iMessageFolderType=EFolderTypeMixed;
	iMimeHeader->SetContentTypeL(KImcvMultipart);
	iMimeHeader->SetContentSubTypeL(KImcvMixed);
	}


/**
DoMultipartTypeL()
*/
void CLocalMimeParser::DoMultipartTypeL()
	{
	iImRecvConvert.iEntryType = CLocalImRecvConvert::EFolderEntry;
	iImRecvConvert.iEmailPart = CLocalImRecvConvert::KMultiPart;
	
	iContentType=EMimeMultipart;
	iMimeHeader->SetContentTypeL(KImcvMultipart);

	if(MatchAndRemoveToken(KImcvForwardSlash))
		{
		if(MatchAndRemoveToken(KImcvMixed))
			{
			iMessageFolderType=EFolderTypeMixed;
			iMimeHeader->SetContentSubTypeL(KImcvMixed);
			}
		else if(MatchAndRemoveToken(KImcvRelated))
			{
			iMessageFolderType=EFolderTypeRelated;
			iMimeHeader->SetContentSubTypeL(KImcvRelated);
			}
		else if(MatchAndRemoveToken(KImcvAlternative))
			{
			iMessageFolderType=EFolderTypeAlternative;
			iMimeHeader->SetContentSubTypeL(KImcvAlternative);
			}
		else if(MatchAndRemoveToken(KImcvEncrypted))
			{
//	Add this when Encryption is handled iMessageFolderType=EFolderTypeEncrypted;
			iMimeHeader->SetContentSubTypeL(KImcvEncrypted);
			}
		else if(MatchAndRemoveToken(KImcvParallel))
			{
			iMessageFolderType=EFolderTypeParallel;
			iMimeHeader->SetContentSubTypeL(KImcvParallel);
			}
		else if(MatchAndRemoveToken(KImcvDigest))
			{
			iMessageFolderType=EFolderTypeDigest;
			iMimeHeader->SetContentSubTypeL(KImcvDigest);
			isMessageDigest=ETrue;
			}
		else if(MatchAndRemoveToken(KImcvSigned))
			{
//	Add this when Signed is handled	iMessageFolderType=EFolderTypeSigned;
			iMimeHeader->SetContentSubTypeL(KImcvSigned);
			}
		else if(MatchAndRemoveToken(KImcvReport))
			{
			iMimeHeader->SetContentSubTypeL(KImcvReport);
			}
		else
			{
			iMessageFolderType=EFolderTypeUnknown;
			iMimeHeader->SetContentSubTypeL(KImcvUnknown);
			}
		}

	if (iMessageFolderType==EFolderTypeRelated)
		iImRecvConvert.iParent->At(0).iFolder=iMessageFolderType; 

	// Find any parameters specific to a Multipart content type	

	HBufC8* paramValue = NULL;

	// Extracts the boundary string

	ExtractParameterInfoL(KImcvBoundary, paramValue);
	if( paramValue!=NULL ) 
		{
		CleanupStack::PushL(paramValue);
		iBoundaryFound = ETrue;
		iImRecvConvert.iEmailPart = CLocalImRecvConvert::KMultiPart;
		SetBoundaryL(*paramValue);
		CleanupStack::PopAndDestroy(paramValue);
		}

	// Extracts start ID if it has been defined;

	ExtractParameterInfoL(KImcvStartPart, paramValue);
	if( paramValue!=NULL )
		{
		delete iStartId;
		iStartId=NULL;
		iStartId = paramValue;
		}
	}


/**
DoTextTypeL()
*/
void CLocalMimeParser::DoTextTypeL()
	{
	HBufC8* paramValue = NULL;
	
	iImRecvConvert.iEntryType = CLocalImRecvConvert::ETextEntry;
	iContentType=EMimeText;		
	iMimeHeader->SetContentTypeL(KImcvText);

	if(MatchAndRemoveToken(KImcvForwardSlash))
		{
		if(MatchAndRemoveToken(KImcvPlain))
			iMimeHeader->SetContentSubTypeL(KImcvPlain);
		else
		if(MatchAndRemoveToken(KImcvHtml))
			{
			iMimeHeader->SetContentSubTypeL(KImcvHtml);
			iImRecvConvert.iEntryType = CLocalImRecvConvert::EHtmlEntry;
			iImRecvConvert.iCurrentPartIsRichText = EFalse;	
			}
		else
		if(MatchAndRemoveToken(KImcvDirectory))
			{
			iMimeHeader->SetContentSubTypeL(KImcvDirectory);
			iMimeHeader->ContentTypeParams().AppendL(KImcvProfile);
			ExtractParameterInfoL(KImcvProfile, paramValue);
			// Assume at right context, the email message, not attachment.
			
			iMessageFolderType=EFolderTypeDirectory;
			if( paramValue!=NULL )
				{
				CleanupStack::PushL(paramValue);
				iMimeHeader->ContentTypeParams().AppendL(*paramValue);
				if(paramValue->MatchF(KImcvVCard) == 0)
					{
					iVCard=ETrue;
					}
				CleanupStack::PopAndDestroy(paramValue);
				}
			}
		if(MatchAndRemoveToken(KImcvVCalender))
			iVCalendar=ETrue;

		// Extract the charset value, 

		ExtractParameterInfoL(KImcvCharset, paramValue);

		if(paramValue!=NULL)
			{
			CleanupStack::PushL(paramValue);
			// check if at top level header or Mime part header

			TUint charsetUid = iImRecvConvert.CharacterConverter().GetMimeCharsetUidL(*paramValue); 
			if(iImRecvConvert.NotFinishedRfc822Header() == EFalse)
				iCharset=charsetUid;

			// Store in CMimeHeader::iContentTypeParams
			iMimeHeader->ContentTypeParams().AppendL(KImcvCharset);
			iMimeHeader->ContentTypeParams().AppendL(*paramValue);
			CleanupStack::PopAndDestroy(paramValue);

			if (!iImRecvConvert.CharacterConverter().PrepareToConvertToFromOurCharsetL(charsetUid))
				charsetUid=KUidMsvCharsetNone;
			iMimeHeader->SetMimeCharset(charsetUid);
			}
		else
			iMimeHeader->SetMimeCharset(iDefaultCharset);
		}
	}


/**
DoContentTypeL()
*/
void CLocalMimeParser::DoContentTypeL()
	{
	RemoveSurroundingCharacters(KImcvLeftChevron, KImcvRightChevron, *iMimeHeaderLine);

	if(MatchAndRemoveToken(KImcvText))
		DoTextTypeL();
	else
	if(MatchAndRemoveToken(KImcvMultipart))
		DoMultipartTypeL();
	else
	if(MatchAndRemoveToken(KImcvMessage))
		DoMessageTypeL();
	else
	if(MatchAndRemoveToken(KImcvImage))
		{	
		iContentType=EMimeImage; 
		iMimeHeader->SetContentTypeL(KImcvImage);
		DoAttachmentTypeL();
		}
	else
	if(MatchAndRemoveToken(KImcvApplication))
		{
		iContentType=EMimeApplication; 
		iMimeHeader->SetContentTypeL(KImcvApplication);
		DoAttachmentTypeL();
		}
	else
	if(MatchAndRemoveToken(KImcvAudio))
		{
		iContentType=EMimeAudio; 
		iMimeHeader->SetContentTypeL(KImcvAudio);
		DoAttachmentTypeL();
		}
	else
	if(MatchAndRemoveToken(KImcvVideo))
		{
		iContentType=EMimeVideo; 
		iMimeHeader->SetContentTypeL(KImcvVideo);
		DoAttachmentTypeL();
		}
	else
		{
		iContentType=EMimeUnknownContent; 
		iMimeHeader->SetContentTypeL(KImcvUnknown);
		}

	// Extract the filename if it exists
	HBufC* paramStore = HBufC::NewLC(MaxMimeParameterValueLength);
	HBufC8* paramStore8 = HBufC8::NewLC(KMimeEncodedParameterSize);
	TPtr paramValue(paramStore->Des());
	TPtr8 paramValue8(paramStore8->Des());

	ExtractParameterInfoL(KImcvMimeTypeName, paramValue,paramValue8);
	if(paramValue.Length())
		{
		iMimeHeader->ContentTypeParams().AppendL(KImcvMimeTypeName);
		iMimeHeader->ContentTypeParams().AppendL(paramValue8);
		iImRecvConvert.SetAttachmentName(paramValue);
		}
	
	CleanupStack::PopAndDestroy(2,paramStore); 
	}


/**
DoEncodingL()
*/
void CLocalMimeParser::DoEncodingL()
	{
	iMimeHeader->SetContentTransferEncodingL(*iMimeHeaderLine);

	switch ((*iMimeHeaderLine)[0])
		{
		case '7': // 7bit
			iContentEncoding = EEncodingType7Bit;
			break;
		case '8': // 8bit
			iContentEncoding = EEncodingType8Bit;
			break;
		case 'q': // quoted-printable
		case 'Q': 
			iContentEncoding = EEncodingTypeQP;
			break;
		case 'b': // binary or base64
		case 'B':
			iContentEncoding = ((*iMimeHeaderLine)[1] == 'i' || (*iMimeHeaderLine)[1] == 'I' ? EEncodingTypeBinary : EEncodingTypeBASE64);
			break;
		case 'x': //in order to support UU encoded within a MIME message
		case 'X':
			iContentEncoding = ((*iMimeHeaderLine).FindF(KImcvXUUString)==KErrNotFound ? EEncodingTypeUnknown : EEncodingTypeUU);
			break;
		default:
			iContentEncoding = EEncodingTypeUnknown;
			break;
		}
	}


/**
DoDescriptionL()
*/
void CLocalMimeParser::DoDescriptionL()
	{
	TPtrC8 marked(iMimeHeaderLine->Des());
	if (marked.Length()>KMaxFileName)
		marked.Set(marked.Left(KMaxFileName));

	iContentDescription.Copy(marked);

	// remove the CRLF

	TInt length = iContentDescription.Length();
	if (length>2)
		{
		if (iContentDescription[length-2]==KImcvCR && iContentDescription[length-1]==KImcvLF)
			iContentDescription.SetLength(length-2);
		}
	iMimeHeader->SetContentDescriptionL(marked);
	}


/**
DoDispositionL()
*/
void CLocalMimeParser::DoDispositionL()
	{
	TBool inLine = EFalse;
	if(MatchAndRemoveToken(KImcvAttachment))
		{
		iMimeHeader->SetContentDispositionL(KImcvAttachment);
		if (iMimeHeader->ContentSubType()!=KImcvRfc822)
			{
			iImRecvConvert.iCurrentPartIsRichText = EFalse;	
			}
		if (iImRecvConvert.iEntryType!=CLocalImRecvConvert::EMessageEntry)
			{
			iImRecvConvert.iEntryType = CLocalImRecvConvert::EAttachmentEntry;
			}
		}
	else if(MatchAndRemoveToken(KImcvInline))
		inLine = ETrue;

	// Extract the filename if it exists, unless we already have a name for it	
	if(!iImRecvConvert.iAttachmentName.Length())
		{
		HBufC* paramStore = HBufC::NewLC(KHeaderBufferLength);
		TPtr paramValue(paramStore->Des());
		HBufC8* paramStore8 = HBufC8::NewLC(KMimeEncodedParameterSize);
		TPtr8 paramValue8(paramStore8->Des());

		ExtractParameterInfoL(KImcvMimeDispositionFilename, paramValue,paramValue8);
		if(paramValue.Length())
			{
			iMimeHeader->ContentTypeParams().AppendL(KImcvMimeDispositionFilename);
			iMimeHeader->ContentTypeParams().AppendL(paramValue8);
			iImRecvConvert.SetAttachmentName(paramValue);
			}

		CleanupStack::PopAndDestroy(2,paramStore); // paramStore, paramStore8
		}

		if(inLine)
 		{
 		// Content Disposition set to inline
 		if (iImRecvConvert.iAttachmentName.Length())
 			// filename exists
 			{
 			iMimeHeader->SetContentDispositionL(KImcvAttachment);
 			iImRecvConvert.iCurrentPartIsRichText = EFalse;	
			if (iImRecvConvert.iEntryType!=CLocalImRecvConvert::EMessageEntry)
 				{
 				iImRecvConvert.iEntryType = CLocalImRecvConvert::EAttachmentEntry;
 				}
 			}
 		else
 			iMimeHeader->SetContentDispositionL(KImcvInline);
 		}
	}


/**
ContentSubType()

@return
*/  
TPtrC8 CLocalMimeParser::ContentSubType() const  
	{
	return iMimeHeader->ContentSubType();;
	}


/**
VCard()

@return
*/  
TBool CLocalMimeParser::VCard() const  
	{
	return iVCard;
	}


/**
VCalendar()

@return
*/  
TBool CLocalMimeParser::VCalendar() const  	
	{
	return iVCalendar;
	}


/**
StartPart()

@return
*/  
TBool CLocalMimeParser::StartPart() const  
	{
	return iStartPart;
	}


/**
MessageFolderType()

@return
*/  
TImEmailFolderType CLocalMimeParser::MessageFolderType() const  
	{
	return iMessageFolderType;
	}


/**
SetMessageFolderType()

@param aFolderType
*/  
void CLocalMimeParser::SetMessageFolderType(TImEmailFolderType aFolderType)  
	{
	iMessageFolderType=aFolderType;
	}


/**
SetBoundaryL()

@param aBoundaryText
*/  
void CLocalMimeParser::SetBoundaryL(const TDesC8& aBoundaryText)  
	{
	TBuf8<KMaxBoundaryTextLength+2> tempBoundary(KImcvMimeBoundaryStartEnd);
	// From RFC 1521, (Boundaries) must be no longer than 70 characters.
	// Including beginning and end "--" 
	if (aBoundaryText.Length()>70)
		tempBoundary.Append(aBoundaryText.Left(70));
	else
		tempBoundary.Append(aBoundaryText);
	iBoundaryText->AppendL(tempBoundary);

	iBoundaryIndex = iBoundaryText->MdcaCount(); //iBoundaryIndex stores a count value not the index here
	iBoundaryIndex = (iBoundaryIndex)? --iBoundaryIndex: 0;
	iBoundaryLength = iBoundaryText->MdcaPoint(iBoundaryIndex).Length();
	}


/**
IsBoundary()

@param aSourceLine
 @return
*/
TBool CLocalMimeParser::IsBoundary(const TDesC8& aSourceLine)
	{
	if(iBoundaryText->MdcaCount())
		{
		TInt found = 0;
		TInt aLineLength = aSourceLine.Length();
		TInt compareLength = aLineLength > iBoundaryLength ? iBoundaryLength : aLineLength;
		TPtrC8 tempSourceLine(aSourceLine.Ptr(), compareLength);
	
		TInt error = iBoundaryText->Find(tempSourceLine, found, ECmpNormal);

		if(error||(!iBoundaryLength))
			return KBoundaryNotFound;
	
		// The following code is executed only if aSourceLine is a boundary
		if(found != iBoundaryIndex)
			{
			iReceiveError = (iReceiveError)? iReceiveError: KBoundaryError;
			iBoundaryIndex = found;
			iBoundaryText->Delete(found++);
			}
	
		if(aLineLength >= iBoundaryLength+4) // allow for CRLF & then check for the double hyphen
			{
			if((aSourceLine[iBoundaryLength] == KImcvHyphen) && (aSourceLine[iBoundaryLength+1] == KImcvHyphen))
				iTerminatingBoundary = ETrue; // this is a terminating boundary
			}
		else
			iTerminatingBoundary = EFalse;
	
		return KBoundaryFound;
		}
	
	return KBoundaryNotFound;
	}


/**
RemoveBoundary()
*/
void CLocalMimeParser::RemoveBoundary()
	{
	iBoundaryText->Delete(iBoundaryIndex);
	if(iBoundaryText->MdcaCount())
		{
		iBoundaryIndex = iBoundaryText->MdcaCount(); //iBoundaryIndex stores a count value not the index here
	
		iBoundaryIndex = (iBoundaryIndex)? --iBoundaryIndex: 0;
		iBoundaryLength = iBoundaryText->MdcaPoint(iBoundaryIndex).Length();
		}
	}


/**
MatchAndRemoveToken()

@param aToken
@return
*/
TBool CLocalMimeParser::MatchAndRemoveToken( const TDesC8& aToken )
	{
	TInt comparison;
	TInt tokenLength = aToken.Length();
	TInt desLength = (*iMimeHeaderLine).Length();
	TInt compareLength = tokenLength > desLength ? desLength : tokenLength;
	TPtrC8 left((*iMimeHeaderLine).Left(compareLength));
	
	// now see whether the current line contains the search string
	comparison = left.CompareF(aToken);
	if (!comparison)
		{
		// found the match string at the start of the output line, so remove it
		iMimeFieldsExist=ETrue;
		isMime=ETrue;
		// get rid of any whitespace betweebn the tag and the data while we have a chance
		TInt whitespaceLength=0;
		TInt maxLength=desLength-tokenLength;
		const TUint8* ptr = (*iMimeHeaderLine).Ptr();
		while ( whitespaceLength <= maxLength && (ptr[tokenLength+whitespaceLength] == KImcvSP || ptr[tokenLength+whitespaceLength] == KImcvTab) )
			whitespaceLength++;
		iMimeHeaderLine->Des().Delete(0, tokenLength+whitespaceLength);

		// Reset iLex, so its length is updated.
		iLex = *iMimeHeaderLine;
		}
	return (comparison==0);
	}


/**
IsSpecialChar()

@param aChar
@return
*/
TBool CLocalMimeParser::IsSpecialChar( const TUint8 aChar )
	{
	return (aChar == '(' || aChar == ')' || aChar == '<' || aChar == '>' || aChar == '@' 
		 || aChar == ',' || aChar == ';' || aChar == ':' || aChar == '\\'|| aChar == '"' 
		 || aChar == '/' || aChar == '[' || aChar == ']' || aChar == '?' || aChar == '=');
	}


/**
ExtractParameterInfoL()
Implicitly the parameter ASSUMED TO BE is not encoded as return parameter, rBuffer,
is 8 bit. iLex should currently be pointing at the space after content-type description.

@param aTag
@param rBuffer
*/
void CLocalMimeParser::ExtractParameterInfoL(const TDesC8& aTag, HBufC8*& rBuffer)
	{	
	rBuffer = NULL;

	TLexMark8 mark;
	TInt offset;

	if( (offset=iLex.Remainder().FindF(aTag)) != KErrNotFound )
		{
		// move past the tag	
		iLex.Inc(offset+aTag.Length());

		// Default : no charset info or folding
		// move forward to the start of the boundary string itself.

		while (iLex.Peek() != KImcvEquals  && !iLex.Eos())
			iLex.Inc(); 

		TPtrC8 paramBuffer(ExtractParameterString(mark));
		if( paramBuffer.Length() > 0 )
			{
			rBuffer = paramBuffer.AllocL();
			}
		}
	}


/**
ExtractParameterInfoL()
iLex should currently be pointing at the space after content-type description.

@param aTag
@param rBuffer
@param rBuffer8
*/
void CLocalMimeParser::ExtractParameterInfoL(const TDesC8& aTag, TDes16& rBuffer, TDes8& rBuffer8)
	{
	TInt offset;

	rBuffer.Copy(KNullDesC);

	// we need to extract the <aTag> text from the same line
	// iLex should currently be pointing at the space after content-type description

	if( (offset=iLex.Remainder().FindF(aTag))!=KErrNotFound )
		{
		// Check which type of encoding present.

		TLexMark8 initMark;
		iLex.Mark(initMark);
		TLexMark8 mark;
		iLex.Inc(offset+aTag.Length()); 	// move past the tag

		const TPtrC8 param = ExtractParameterString(mark);

		rBuffer8.Copy(param);

		if ( ParseRfc2047ParameterInfoL(param, rBuffer) == EFalse )
			{
			iLex.UnGetToMark(initMark);
			ParseRfc2231ParameterInfoL(aTag, rBuffer, offset );
			}
		}
	}


/**
ParseRfc2047ParameterInfoL()
Detect encoding of form =?charset?Q?" Text "?=

@param aParam
@param rBuffer
@return
*/
TBool CLocalMimeParser::ParseRfc2047ParameterInfoL(const TDesC8& aParam, TDes& rBuffer)
	{
	TBool parameterPresent = EFalse;

	// Check for =? somewhere in text.

	if ( aParam.Find(KImcvEncodedWordStart) != KErrNotFound )
		{
		// Make assumption that '=?' appearing in parameter means 
		//it is part of encoding
	
		parameterPresent = ETrue;
		iImRecvConvert.iHeaderConverter->DecodeHeaderFieldL(aParam, rBuffer);
		}
	return parameterPresent;
	}


/**
ParseRfc2231ParameterInfoL()
For extracting parameter data following Mime header fields, in the format
*(;tag=data). As specified in rfc2231
Assumes parameter data seperated by ';'
Takes tag(name) of parameter as input, returning a descriptor with the data

@param aTag
@param rBuffer
@param aOffset
@return
*/
TBool CLocalMimeParser::ParseRfc2231ParameterInfoL(const TDesC8& aTag, TDes& rBuffer, TInt aOffset)
	{
	TBool parameterPresent = ETrue;

	// For storing extracted parameter information

	HBufC8* info = HBufC8::NewLC(KHeaderBufferLength);

	// For storing information relating to parameter extraction/conversion

	TInt count = 0;

	TPtrC8 charset8;
	TPtrC8 language8;
	
	// Following rfc 2231, parameter may be encoded & folded in the following way
	//
	// <name>*0*=us-ascii'en'<encoded data>
	// <name>*1*=< more encoded data>
	// <name>*2=<unencoded info>

	do 
		{
		// move past the tag	
		iLex.Inc(aOffset+aTag.Length());

		if (iLex.Peek() != KImcvAny) 
			{
			// Default : no charset info or folding
			// move forward to the start of the boundary string itself

			while (iLex.Peek() != KImcvEquals  && !iLex.Eos())
				iLex.Inc();
			}
		else	// *, Parameter is encoded
			{
			iLex.Inc(); // Past '*'

			// If parameter folded :- need to get several bits of data and join together.
		
			if ( iLex.Peek()!=KImcvEquals) // Get Number
				{
				iLex.Mark(); // Beginnig of number
				iLex.Inc();
				while (iLex.Peek() != KImcvEquals && iLex.Peek() != KImcvAny && !iLex.Eos())
					iLex.Inc(); 
				TPtrC8 numPtr = iLex.MarkedToken();
				TLex8 lex(numPtr);
				lex.Val(count);  // Store number in count.

				if (iLex.Peek() == KImcvAny)
					iLex.Inc(); // Past *
				}


			// Must get charset & language information etc..

			if (!count)
				{

				iLex.Inc(); // Past '='

				if (iLex.Peek()==KImcvQuote)
					iLex.Inc(); 

				// Extract CHARSET token
				iLex.Mark();
				while (iLex.Peek() != KImcvSingleQuote && !iLex.Eos())
					iLex.Inc(); 
				TPtrC8 marked = iLex.MarkedToken();

				charset8.Set(marked );
				iLex.Inc(); // Past ' 

				// Extract LANGUAGE token
				iLex.Mark();
				while (iLex.Peek() != KImcvSingleQuote && !iLex.Eos())
					iLex.Inc(); 
				TPtrC8 marked1 = iLex.MarkedToken();
				language8.Set(marked1);
				}

			} // else, param encoded
				
		TLexMark8 mark;
		TPtrC8 param = ExtractParameterString(mark);

		// Save parameter data
		TInt maxlen=info->Des().MaxLength();
		if ((*info).Length() + param.Length() > maxlen )
			{
			info = info->ReAllocL(maxlen + param.Length() + MaxMimeParameterValueLength);
		 CleanupStack::Pop();
			CleanupStack::PushL(info); 
			}
		info->Des().Append( param );


		} while( ( aOffset=iLex.Remainder().FindF(aTag))!=KErrNotFound );

	TPtr8 infoPtr(info->Des());	
	
	DecodeRfc2231ParameterInfoL( infoPtr, rBuffer, charset8/*, language8*/ );

	CleanupStack::PopAndDestroy(info); 
	return parameterPresent;
	}


/**
DecodeRfc2231ParameterInfoL()

@param aInput
@param rBufOut
@param aCharset
@return
*/
TBool CLocalMimeParser::DecodeRfc2231ParameterInfoL(TDes8& aInput, TDes& rBufOut, TPtrC8 aCharset/*, TPtrC8 aLanguage*/)
	{
	HBufC8* QPdecodedbuf = HBufC8::NewLC( aInput.Length() );
	TPtr8 ptr(QPdecodedbuf->Des());

	iImRecvConvert.iQPCodec.SetQPChar(KImcvPercentSign);
	iImRecvConvert.iQPCodec.Decode( aInput, ptr);
	iImRecvConvert.iQPCodec.SetQPChar(KImcvEquals);

	// Convert parameter, based on charset supplied.
	CImConvertCharconv& charconv = iImRecvConvert.CharacterConverter();
	TUint id = charconv.GetMimeCharsetUidL( aCharset);

	if(id==KUidMsvCharsetNone)
		{
		id=charconv.SystemDefaultCharset();
		}

	if (!charconv.PrepareToConvertToFromOurCharsetL(id))
		{
		rBufOut.Copy(ptr);
		}
	else
		{
		// Charset found, so do conversion
		TInt unconverted;
		TInt indexOfFirst;
		TInt rem = charconv.ConvertToOurCharsetL(ptr, rBufOut, unconverted, indexOfFirst);
		if (rem < 0) // error
			Append(rBufOut, ptr);
		else if (rem && rem < KConversionRemainderLength)
			rBufOut.Copy(ptr);	

		}
	
	CleanupStack::PopAndDestroy(QPdecodedbuf);
	return ETrue;
	}


/**
ExtractParameterString()
Check for delimiter & mark parameter text string

@param rMark
@return
*/
TPtrC8 CLocalMimeParser::ExtractParameterString(TLexMark8& rMark)
	{
	// move on to the first char of the boundary ; this MIGHT be a double-quote

	TBool delimited = EFalse;
	iLex.Inc();
				
	if ( iLex.Peek() == KImcvDoubleQuote )
		{
		delimited = ETrue;
		iLex.Inc();
		}

	while ( iLex.Peek().IsSpace() )
		iLex.Inc();

	iLex.Mark( rMark );

	TBool finished = EFalse;
	while ( !finished  && !iLex.Eos() )
		{
		iLex.Inc();

		if ( delimited )
			finished = (iLex.Peek() == KImcvDoubleQuote);
		else
			finished = ( iLex.Peek().IsSpace() || IsSpecialChar((TUint8)iLex.Peek()) );
		} 

	return iLex.MarkedToken( rMark );
	}


/**
CurrentCharsetL()
Which charset to use .. ? If the body part charset set, use that.
else if the main header charset set, use that... else default to us-ascii.

@return
*/
TUint CLocalMimeParser::CurrentCharsetL() const
	{
	TPtrC8 paramCharset = GetContentTypeValue(KImcvCharset);

	TUint charsetId;
	if (paramCharset.Length())
		// charset parameter present.
		charsetId = iImRecvConvert.CharacterConverter().GetMimeCharsetUidL(paramCharset);
	else if (iCharset) 
		// Main Mime header has default charset value.
		charsetId = iCharset;
	else
		charsetId = iDefaultCharset;

	return charsetId;		
	}


/**
GetContentTypeValue()

@param aContentTypeParameter
@return
*/
const TPtrC8 CLocalMimeParser::GetContentTypeValue(const TDesC8& aContentTypeParameter) const
	{
	CDesC8Array& contentTypeParams = iMimeHeader->ContentTypeParams();
	__ASSERT_DEBUG(!(contentTypeParams.Count()&1), User::Invariant());

	TInt result;
	if (KErrNone == contentTypeParams.Find(aContentTypeParameter,result,ECmpFolded8))
		{
		result++;
		if ((result&1) && result <= contentTypeParams.Count())    
			{       
			// check result+1 is odd & entry exists
			return iMimeHeader->ContentTypeParams()[result];
			}
		}
	return TPtrC8();        // couldn't find match so return an empty descriptor
	}


/**s
EntryId()

@return
*/
TMsvId CLocalImRecvConvert::EntryId()
	{
	// If we are temporarily on the null entry then return the saved entry
	if (iServerEntry->Entry().Id() == KMsvNullIndexEntryId)
		{
		return iSavedEntryId;
		}
	else
		{
		return iServerEntry->Entry().Id();
		}
	}

