// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SisRegistry - server session implementation
// 
//

/**
 @file
 @test
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>
#include <caf/virtualpath.h>

#include "rtamanagerhandler.h"
#include "RTAsession.h"
#include "RTAserver.h"
#include "rightsobject.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfiles.h"
using namespace ReferenceTestAgent;



CRtaManagerHandler* CRtaManagerHandler::NewL(const CSession2& aSession)
	{
	CRtaManagerHandler* self = new (ELeave) CRtaManagerHandler(aSession);
	return self;
	}
	
CRtaManagerHandler::CRtaManagerHandler(const CSession2& aSession) : CRtaMessageHandler(aSession)
	{
	}
	
CRtaManagerHandler::~CRtaManagerHandler()
	{
	delete iArchive;
	iFile.Close();
	}

void CRtaManagerHandler::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{
	switch (aMessage.Function())
		{	
	// RRtaManager functions
	case EManagerDeleteFile:
	case ENoEnforceManagerDeleteFile:
		DeleteFileL(aMessage);
		break;
	case EManagerCopyFile:
	case ENoEnforceManagerCopyFile:
		CopyFileL(aMessage);
		break;
	case EManagerCopyFileHandle:
	case ENoEnforceManagerCopyFileHandle:
		CopyFileHandleL(aMessage);
		break;
	case EManagerRenameFile:
	case ENoEnforceManagerRenameFile:
		RenameFileL(aMessage);
		break;
	case EManagerMkDir:
	case ENoEnforceManagerMkDir:
		MkDirL(aMessage);
		break;
	case EManagerMkDirAll:
	case ENoEnforceManagerMkDirAll:
		MkDirAllL(aMessage);
		break;
	case EManagerRenameDir:
	case ENoEnforceManagerRenameDir:
		DeleteFileL(aMessage);
		break;
	case EManagerRemoveDir:
	case ENoEnforceManagerRemoveDir:
		RmDirL(aMessage);
		break;
	case EManagerGetDir1:
	case ENoEnforceManagerGetDir1:
		GetDir1L(aMessage);
		break;
	case EManagerGetDir2:
	case ENoEnforceManagerGetDir2:
		GetDir2L(aMessage);
		break;
	case EManagerGetDir3:
	case ENoEnforceManagerGetDir3:
		GetDir3L(aMessage);
		break;	
	case EManagerGetAttribute:
	case ENoEnforceManagerGetAttribute:
		GetAttributeL(aMessage);
		break;
	case EManagerGetAttributeSet:
	case ENoEnforceManagerGetAttributeSet:
		GetAttributeSetL(aMessage);
		break;
	case EManagerGetStringAttribute:
	case ENoEnforceManagerGetStringAttribute:
		GetStringAttributeL(aMessage);
		break;
	case EManagerGetStringAttributeSet:
	case ENoEnforceManagerGetStringAttributeSet:
		GetStringAttributeSetL(aMessage);
		break;
	default:
		CRtaSession::PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CRtaManagerHandler::DeleteFileL(const RMessage2& aMessage)
	{
	_LIT(KSystemDrive, "C:"); // not always c but close enough for test/ref
	const TInt KPrivatePathLen = 32;
	TBuf<KPrivatePathLen> myDataCage;
	User::LeaveIfError(Server().Fs().PrivatePath(myDataCage));
	myDataCage.Insert(0, KSystemDrive); 
	
	HBufC16* buffer = ReadDesC16LC(aMessage,0);

	if (buffer->FindF(myDataCage) == KErrNotFound)
		{
		User::Leave(KErrArgument);
		}
	User::LeaveIfError(Server().Fs().Delete(*buffer));
	CleanupStack::PopAndDestroy(buffer); 
	}

void CRtaManagerHandler::CopyFileL(const RMessage2& aMessage)
	{
	// Read source and destination
	HBufC16* source = ReadDesC16LC(aMessage,0);
	HBufC16* destination= ReadDesC16LC(aMessage,1);
	
	// Attempt to copy the files
	CFileMan* fileMan = CFileMan::NewL(Server().Fs());
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(*source, *destination, EFalse));
	CleanupStack::PopAndDestroy(3, source); // fileman, destination, source
	}

void CRtaManagerHandler::CopyFileHandleL(const RMessage2& aMessage)
	{
	RFile file;

	// Read source and destination
	User::LeaveIfError(file.AdoptFromClient(aMessage,0,1));
	CleanupClosePushL(file);
	HBufC16* destination= ReadDesC16LC(aMessage,2);

	// Attempt to copy the files
	CFileMan* fileMan = CFileMan::NewL(Server().Fs());
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(file, *destination));
	CleanupStack::PopAndDestroy(3); // fileman, destination, file
	}
	
void CRtaManagerHandler::RenameFileL(const RMessage2& aMessage)
	{
	// Read source and destination
	HBufC16* source = ReadDesC16LC(aMessage,0);
	HBufC16* destination= ReadDesC16LC(aMessage,1);
	
	// Attempt to rename the file
	User::LeaveIfError(Server().Fs().Rename(*source, *destination));
	CleanupStack::PopAndDestroy(2, source); // destination, source
	}

void CRtaManagerHandler::MkDirL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to create the directory
	User::LeaveIfError(Server().Fs().MkDir(*path));
	CleanupStack::PopAndDestroy(path);
	}

void CRtaManagerHandler::MkDirAllL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to create the directory
	User::LeaveIfError(Server().Fs().MkDirAll(*path));
	CleanupStack::PopAndDestroy(path);
	}
	
void CRtaManagerHandler::RenameDirL(const RMessage2& aMessage)
	{
	// Read oldPath, newPath
	HBufC16* oldPath = ReadDesC16LC(aMessage,0);
	HBufC16* newPath = ReadDesC16LC(aMessage,1);
	
	// Attempt to rename the directory
	User::LeaveIfError(Server().Fs().Rename(*oldPath, *newPath));
	CleanupStack::PopAndDestroy(newPath); // newPath
	CleanupStack::PopAndDestroy(oldPath); // oldPath
	}
	
void CRtaManagerHandler::RmDirL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to remove the directory
	User::LeaveIfError(Server().Fs().RmDir(*path));
	CleanupStack::PopAndDestroy(path);
	}
	
void CRtaManagerHandler::GetDir1L(const RMessage2& aMessage)
	{
	// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry attribute mask
	TUint entryAttMask;
	TPckg<TUint> entryAttMaskPckg(entryAttMask);
	aMessage.Read(1, entryAttMaskPckg);
	
	// read entry sort key
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(2, entrySortKeyPckg);

	// Get Dir	
	CDir* entryList = NULL;
	User::LeaveIfError(Server().Fs().GetDir(*name, entryAttMask, entrySortKey, entryList));
	CleanupStack::PushL(entryList);
	
	// Write results to the client
	WriteCDirToMessageL(aMessage, 3, *entryList);
	
	CleanupStack::PopAndDestroy(2, name); // entryList, name
	}
	
void CRtaManagerHandler::GetDir2L(const RMessage2& aMessage)
	{
		// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry attribute mask
	TUint entryAttMask;
	TPckg<TUint> entryAttMaskPckg(entryAttMask);
	aMessage.Read(1, entryAttMaskPckg);
	
	// read entry sort key, packaged beside entryAttMask in the same parameter
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(1, entrySortKeyPckg, entryAttMaskPckg.MaxLength());
	
	CDir* entryList = NULL;
	CDir* dirList = NULL;
	User::LeaveIfError(Server().Fs().GetDir(*name, entryAttMask, entrySortKey, entryList, dirList));
	
	// copy data across into CDirStreamable
	CleanupStack::PushL(entryList);
	CleanupStack::PushL(dirList);
	WriteCDirToMessageL(aMessage, 2, *entryList);
	WriteCDirToMessageL(aMessage, 3, *dirList);
		
	CleanupStack::PopAndDestroy(3, name); // dirlist, entryList, name
	}

void CRtaManagerHandler::GetDir3L(const RMessage2& aMessage)
	{
		// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry Uid
	TUidType entryUid;
	TPckg<TUidType> entryUidPckg(entryUid);
	aMessage.Read(1, entryUidPckg);
	
	// read entry sort key, packaged beside entryAttMask in the same parameter
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(2, entrySortKeyPckg);
	
	CDir* fileList = NULL;
	User::LeaveIfError(Server().Fs().GetDir(*name, entryUid, entrySortKey, fileList));
	
	// copy data across into CDirStreamable
	CleanupStack::PushL(fileList);
	WriteCDirToMessageL(aMessage, 3, *fileList);
		
	CleanupStack::PopAndDestroy(2, name); // fileList, name
	}

void CRtaManagerHandler::GetAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;
	TPckg <TInt> attributePckg(attribute);

	TInt value;
	TPckg <TInt> valuePckg(value);			

	// open the content object specified in parameter 0
	OpenContentObjectL(aMessage, 0, 1);

	aMessage.ReadL(2, attributePckg);
	
	iContentObject->GetAttribute(attribute, value);
	
	WriteL(aMessage, 3, valuePckg);
	}
	
void CRtaManagerHandler::GetAttributeSetL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0, 1
	OpenContentObjectL(aMessage, 0, 1);

	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 2);
	TPtr8 valuePtr = value->Des();
	
	RDesReadStream readStream(valuePtr);
	CleanupClosePushL(readStream);
	
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	iContentObject->GetAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	TPtr8 bufPtr = buf->Ptr(0);
	
	WriteL(aMessage, 2, bufPtr);
		
	CleanupStack::PopAndDestroy(3, &attributeSet); // buf, value, attributeSet
	}
	
void CRtaManagerHandler::GetStringAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;

	// open the content object specified in parameter 0
	OpenContentObjectL(aMessage, 0, 1);

	TPckg <TInt> attributePckg(attribute);
	aMessage.ReadL(2, attributePckg);
	
	HBufC* value = ReadDes16LC(aMessage, 3);
	TPtr valuePtr = value->Des();
	
	User::LeaveIfError(iContentObject->GetStringAttribute(attribute, valuePtr));
	
	WriteL(aMessage, 3, valuePtr);
	
	CleanupStack::PopAndDestroy(value);
	}
	
void CRtaManagerHandler::GetStringAttributeSetL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0
	OpenContentObjectL(aMessage, 0, 1);

	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 2);
	TPtr8 valuePtr = value->Des();
	
	RDesReadStream readStream(valuePtr);
	CleanupClosePushL(readStream);
	
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	
	iContentObject->GetStringAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	TPtr8 bufPtr = buf->Ptr(0);
	
	WriteL(aMessage, 2, bufPtr);
		
	CleanupStack::PopAndDestroy(3, &attributeSet); // buf, value, attributeSet
	}
	
void CRtaManagerHandler::NotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	// Server().NotifyStatusChange(aVirtualPath)
	}
	
void CRtaManagerHandler::CancelNotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	// Server().CancelNotifyStatusChange(aVirtualPath)
	}

void CRtaManagerHandler::WriteCDirToMessageL(const RMessage2& aMessage, TInt aParam, CDir& aDir)
	{
	// Create a CDirStreamable from the directory
	CDirStreamable* dirStreamable = CDirStreamable::NewL(aDir);
	CleanupStack::PushL(dirStreamable);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	dirStreamable->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	TPtr8 bufPtr = buf->Ptr(0);

	WriteL(aMessage, aParam, bufPtr);
	
	CleanupStack::PopAndDestroy(2, dirStreamable); // buf, dirStreamable
	}


void CRtaManagerHandler::OpenContentObjectL(const RMessage2& aMessage, TInt aUriParam, TInt aUniqueIdParam)
	{
	// close any existing content object
	delete iArchive;
	iArchive = NULL;
	iFile.Close();

	HBufC* uri = ReadDesC16LC(aMessage, aUriParam);
	HBufC* uniqueId = ReadDesC16LC(aMessage, aUniqueIdParam);	
	
	User::LeaveIfError(iFile.Open(Server().Fs(),*uri, EFileShareReadersOnly | EFileRead | EFileStream));
	iArchive = CRefTestAgentArchive::NewL(iFile);
	iContentObject = &iArchive->DrmFilesL().FindL(*uniqueId);
	CleanupStack::PopAndDestroy(2, uri); // uniqueId, uri
	}

