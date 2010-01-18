/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file
*/


#ifndef __CMTFTESTACTIONSENDASSETBODYTEXT_H__
#define __CMTFTESTACTIONSENDASSETBODYTEXT_H__

#include "CMtfSynchronousTestAction.h"

/*@{*/
/// Literal defined for the name of the Test Action
_LIT(KTestActionSendAsSetBodyText,"SendAsSetBodyText");
/*@}*/

/**
SendAsSetBodyText Test Action is intended to set the body text contents for a message that was created using Send-As API.
@internalTechnology
*/
class CMtfTestActionSendAsSetBodyText : public CMtfSynchronousTestAction
	{
public:
	virtual ~CMtfTestActionSendAsSetBodyText(); 
	static CMtfTestAction* NewL(CMtfTestCase& aTestCase,
		CMtfTestActionParameters* ActionParameters);
	virtual void ExecuteActionL();

private:
	CMtfTestActionSendAsSetBodyText(CMtfTestCase& aTestCase);
	};

#endif //__CMTFTESTACTIONSENDASSETBODYTEXT_H__