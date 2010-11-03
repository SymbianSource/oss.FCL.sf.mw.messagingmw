// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSCRIPTFILEPROCESSOR_H__
#define __CSCRIPTFILEPROCESSOR_H__

#include <s32file.h>
 
class CScriptFileProcessor : public CBase
	{	
public:	

	enum TDataDirection 
		{
		EFromServer=0, 
		EFromClient
		};	

	static CScriptFileProcessor* NewL(const TDesC& aScriptFile);
	static CScriptFileProcessor* NewLC(const TDesC& aScriptFile);
	~CScriptFileProcessor();
	TInt ReadLine(TDes8& aLine);
	CScriptFileProcessor::TDataDirection DataDirection() const;

private:
	CScriptFileProcessor();	
	void ConstructL(const TDesC& aScriptFile);
	TInt ProcessLine(TDes8& aLine);

private:
	RFileReadStream iFileReadStream;
	RFs iFSession;
	RFile iFile;
	TDataDirection iDataDirection;
	};


#endif //__CSCRIPTFILEPROCESSOR_H__