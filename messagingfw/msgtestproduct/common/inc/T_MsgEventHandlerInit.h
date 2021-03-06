/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This contains the header file for EventHandlerInit
* 
*
*/



/**
 @file 
*/


#ifndef __T_EVENT_HANDLER_INIT_H__
#define __T_EVENT_HANDLER_INIT_H__


/** User includes */
#include "T_MsgSharedDataCommon.h"
#include "T_MsgSyncStepCommon.h"


/* Literals Used */
_LIT(KMsgEventHandlerInit,"EventHandlerInit");


/* Implements a test step to initialise the Ras connection */
class CT_MsgEventHandlerInit : public CT_MsgSyncStepCommon
	{
public:
	CT_MsgEventHandlerInit(CT_MsgSharedDataCommon& aSharedDataCommon); 

	/* CTestStep implementation */
	virtual TVerdict doTestStepL();	
	};
	
#endif /* __T_EVENT_HANDLER_INIT__ */
