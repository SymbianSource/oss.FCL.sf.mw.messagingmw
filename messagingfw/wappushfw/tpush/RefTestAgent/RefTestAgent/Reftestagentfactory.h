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


#ifndef __REFTESTAGENTFACTORY_H__
#define __REFTESTAGENTFACTORY_H__

#include <caf/caf.h>
#include <caf/agentfactory.h>

using namespace ContentAccess;

	class CRefTestAgentFactory : public CAgentFactory
		{
	public:
		static CAgentFactory* NewL();
		
		// From CAgentFactory
		virtual CAgentContent* CreateContentBrowserL(const TDesC& aURI, TContentShareMode aShareMode);	
		virtual CAgentContent* CreateContentBrowserL(RFile& aFile);
		virtual CAgentData* CreateDataConsumerL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		virtual CAgentData* CreateDataConsumerL(RFile& aFile, const TDesC& aUniqueId);
		virtual CAgentManager*  CreateManagerL();
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray, const TDesC& aOutputDirectory, const TDesC& aSuggestedFileName);
		virtual CAgentImportFile* CreateImporterL(const TDesC8& aMimeType, const CMetaDataArray& aMetaDataArray);
		virtual CAgentRightsManager* CreateRightsManagerL();	
		};

#endif // __REFTESTAGENTFACTORY_H__
