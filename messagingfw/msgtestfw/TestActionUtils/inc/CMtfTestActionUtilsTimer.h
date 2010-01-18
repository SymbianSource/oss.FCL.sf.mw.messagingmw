/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMTF_TEST_ACTION_UTILS_TIMER_H__
#define __CMTF_TEST_ACTION_UTILS_TIMER_H__


#include <e32base.h>


class CMtfTestActionUtilsTimer : public CTimer
	{
public:
	static CMtfTestActionUtilsTimer* NewL();
   ~CMtfTestActionUtilsTimer();

   void After(TInt aSec,TRequestStatus& aStatus);

protected:
	void RunL();

private:
	CMtfTestActionUtilsTimer();

private:
	TRequestStatus* iRequestStatus;
	};


#endif
