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

/**
 @file
*/
#include "CMtfTestActionUtils.h"


void CMtfTestActionUtils::OpenResourceFileL(RResourceFile& rFile, RFs& anFs, const TDesC& aFileName )
	{
	// open resource file
	TFileName fileName(aFileName);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
	BaflUtils::NearestLanguageFile( anFs, fileName );
	if (BaflUtils::FileExists(anFs,fileName))
 		{
		rFile.OpenL( anFs, fileName );
 		return;
 		}
 	User::Leave(KErrNotFound);
	}


void CMtfTestActionUtils::CloseResourceFile( TAny* aFile )
	{
	((RResourceFile*)aFile)->Close();
	}
