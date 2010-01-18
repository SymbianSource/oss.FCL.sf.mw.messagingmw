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
// Description: Verify email header

// @file

//
//
// [TestStep Name]
// VerifyHeaders
//
// [Paramaters]
// PopAccountName			:		Name of the Pop account
// ToAddress%d%d			: 		Name of To Address with
//									First %d for multiple message entries
//									Seconde %d for multiple email addresses
// CcAddress%d%d			:		Name of Cc Address with
//									First %d for multiple message entries
//									Seconde %d for multiple email addresses
// BccAddress%d%d			:		Name of Bcc Address with
//									First %d for multiple message entries
//									Seconde %d for multiple email addresses
// FromAddress%d			:		Name of From Address with
//									First %d for multiple message entries
// Subject%d				:		Name of Subject with
//									First %d for multiple message entries
// ContentType%d			:		Name of ContentType with
//									First %d for multiple message entries
// ContentSubType%d			:		Name of ContentSubType with
//									First %d for multiple message entries
//
// [Description]
// Veriy the email messge header parts
//
// [APIs Used]
// CMsvEntry::ReadStoreL
// CImHeader::RestoreL
//
//

// User includes
#include "t_verifyheaders.h"

// Epoc includes
#include <t_utils.h>
#include <t_utilscentralrepository.h>
#include <miuthdr.h>

// Literals used
_LIT(KPopAccountName,"PopAccountName");
_LIT(KToAddress,"ToAddress%d%d");
_LIT(KCcAddress,"CcAddress%d%d");
_LIT(KBccAddress,"BccAddress%d%d");
_LIT(KFromAddress,"FromAddress%d");
_LIT(KSubject,"Subject%d");

_LIT(KContentType,"ContentType%d");
_LIT(KContentSubType,"ContentSubType%d");

/**
  Function 		: CT_MsgVerifyHeaders
  Description 	: Constructor
  @param 		:
  @return 		: N/A
*/
CT_MsgVerifyHeaders::CT_MsgVerifyHeaders(CT_MsgSharedDataPop& aSharedDataPOP)
:	CT_MsgSyncStepPOP(aSharedDataPOP)
	{
	SetTestStepName(KVerifyHeaders);
	}

/**
  Function    	: doTestStepL
  Description 	: Reads the POP account name and the expected header parts from the ini file.
				  Obtains the header parts of the children entries under the given folder.
  @return     	: TVerdict - Test step result
			      EPass, if the header part matches
			      EFail, if the header part does not match
*/
TVerdict CT_MsgVerifyHeaders::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Step :VerifyHeaders for POP services "));
	// Read the POP account name from the ini file
	TPtrC	popAccountName;
	if(!GetStringFromConfig(ConfigSection(), KPopAccountName, popAccountName))
		{
		ERR_PRINTF1(_L("POP account name is not specified"));
		SetTestStepResult(EFail);
		}
	else
		{
		// Retrieving the POP service Id based on the account name read from ini
		TMsvId popServiceId = CT_MsgUtilsCentralRepository::GetPopServiceIdL((TDes&) popAccountName);
		if(popServiceId == KMsvNullIndexEntryId)
			{
			ERR_PRINTF1(_L("Invalid Pop account name specified"));
			SetTestStepResult(EFail);
			}
		else
			{
			// Create a selection of the children entries under the given folder and verify the header parts
			CMsvEntrySelection* selection = CT_MsgUtils::CreateChildrenSelectionL(iSharedDataPOP.iSession, popServiceId);
			CleanupStack::PushL(selection);
			// Verify the header parts
			TBool status = VerifyImHeaderPartsL(selection);
			if (status == EFail )
				{
				SetTestStepResult(EFail);
				}
			// Verify mime header parts
			status = VerifyMimeHeaderPartsL(selection);
			if (status == EFail )
				{
				SetTestStepResult(EFail);
				}
			CleanupStack::PopAndDestroy(selection);
			}
		}
	return TestStepResult();
	}
TBool CT_MsgVerifyHeaders::VerifyImHeaderPartsL(CMsvEntrySelection* aEntrySelection)
	{
	TBool status = EPass;
	for(int index = 0; index < aEntrySelection->Count(); ++index)
		{
		CMsvEntry* entry = iSharedDataPOP.iSession->GetEntryL(aEntrySelection->At(index));
		CleanupStack::PushL(entry);
		CMsvStore* store = entry->ReadStoreL();
		CleanupStack::PushL(store);
		CImHeader* header = CImHeader::NewLC();
		// Retrieve the email message header part from the message store
		header->RestoreL(*store);

		// Validate the header fields
		TBuf<256> param;

		for(int indexTo = 0; indexTo < header->ToRecipients().Count(); ++indexTo)
			{
			TPtrC toAddress;
			param.Format(KToAddress, index, indexTo);
			if(GetStringFromConfig( ConfigSection(), param, toAddress))
				{
				TPtrC toAdd = header->ToRecipients()[indexTo];
				if(toAddress.Compare(header->ToRecipients()[indexTo]))
					{
					ERR_PRINTF1(_L(" To Address is not matched"));
					status = EFail;
					}
				}
			}
		for(int indexCc = 0; indexCc < header->CcRecipients().Count(); ++indexCc)
			{
			TPtrC ccAddress;
			param.Format(KCcAddress, index, indexCc);
			if(GetStringFromConfig( ConfigSection(), param, ccAddress))
				{
				TPtrC ccAdd = header->CcRecipients()[indexCc];
				if(ccAddress.Compare(header->CcRecipients()[indexCc]))
					{
					ERR_PRINTF1(_L("CC Address is not matched"));
					status = EFail;
					}
				}
			}
		for(int indexBcc = 0; indexBcc < header->BccRecipients().Count(); ++indexBcc)
			{
			TPtrC bccAddress;
			param.Format(KBccAddress, index, indexBcc);
			if(GetStringFromConfig( ConfigSection(), param, bccAddress))
				{
				TPtrC bccAdd = header->BccRecipients()[indexBcc];
				if(bccAddress.Compare(header->BccRecipients()[indexBcc]))
					{
					ERR_PRINTF1(_L("BCC Address is not matched"));
					status = EFail;
					}
				}
			}
		TPtrC fromAddress;
		param.Format(KFromAddress, index);
		if(GetStringFromConfig( ConfigSection(), param, fromAddress))
			{
			TPtrC fromAdd = header->From();
			if(fromAddress.Compare(header->From()))
				{
				ERR_PRINTF1(_L("From Address is not matched"));
				status = EFail;
				}
			}
		TPtrC subject;
		param.Format(KSubject, index);
		if(GetStringFromConfig( ConfigSection(), param, subject))
			{
			TPtrC tsub = header->Subject();
			if(subject.Compare(header->Subject()))
				{
				ERR_PRINTF1(_L("Subject is not matched"));
				status = EFail;
				}
			}
		CleanupStack::PopAndDestroy(header);
		CleanupStack::PopAndDestroy(store);
		CleanupStack::PopAndDestroy(entry);
		}
	return status;
	}
TBool CT_MsgVerifyHeaders::VerifyMimeHeaderPartsL(CMsvEntrySelection* aEntrySelection)
	{
	TBool status = EPass;
	for(int index = 0; index < aEntrySelection->Count(); ++index)
		{
		CMsvEntry* entry = iSharedDataPOP.iSession->GetEntryL(aEntrySelection->At(index));
		CleanupStack::PushL(entry);
		CMsvStore* store = entry->ReadStoreL();
		CleanupStack::PushL(store);
		if (store->IsPresentL(KUidMsgFileMimeHeader))
			{
			CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
			mimeHeader->RestoreL(*store);
	
			TBuf<256> param;
			TPtrC contentType;
			param.Format(KContentType, index);
			if(GetStringFromConfig( ConfigSection(), param, contentType))
				{
				TPtrC8 content8 = mimeHeader->ContentType();
				HBufC* content16 = HBufC::NewLC(content8.Length());
				content16->Des().Copy(content8);
				if(contentType.Compare(content16->Des()))
					{
					ERR_PRINTF1(_L("Content Type is not matched"));
					status = EFail;
					}
				CleanupStack::PopAndDestroy(content16);
				}
			TPtrC contentSubType;
			param.Format(KContentSubType, index);
			if(GetStringFromConfig( ConfigSection(), param, contentSubType))
				{
				TPtrC8 content8 = mimeHeader->ContentSubType();
				HBufC* content16 = HBufC::NewLC(content8.Length());
				content16->Des().Copy(content8);
				if(contentSubType.Compare(content16->Des()))
					{
					ERR_PRINTF1(_L("Content Sub Type is not matched"));
					status = EFail;
					}
				CleanupStack::PopAndDestroy(content16);
				}
			CleanupStack::PopAndDestroy(mimeHeader);
			}
		CleanupStack::PopAndDestroy(store);
		CleanupStack::PopAndDestroy(entry);
		}
	return status;
	}
