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
//



#ifndef __CMTFTESTACTIONLOADPOPSETTINGS_H__
#define __CMTFTESTACTIONLOADPOPSETTINGS_H__


#include "CMtfSynchronousTestAction.h"

class CEmailAccounts;
class TPopAccount;
class CImPop3Settings;
class CImSmtpSettings;
class CImIAPPreferences;

_LIT( KTestActionLoadPopSettings,"LoadPopSettings");

class CMtfTestActionLoadPopSettings : public CMtfSynchronousTestAction
	{
public:
	static CMtfTestAction* NewL( CMtfTestCase& aTestCase,CMtfTestActionParameters* aActionParameters );
	virtual void ExecuteActionL();
	~CMtfTestActionLoadPopSettings();
	
private:
	CMtfTestActionLoadPopSettings(CMtfTestCase& aTestCase);
	
	void LoadSettingsL(CEmailAccounts& aAccounts, const TPopAccount& aAccount, CImPop3Settings& aPopSettings, CImIAPPreferences& aPopIapSettings, CImSmtpSettings& aSmtpSettings, CImIAPPreferences& aSmtpIapSettings);
	};


#endif  // __CMTFTESTACTIONLOADPOPSETTINGS_H__
