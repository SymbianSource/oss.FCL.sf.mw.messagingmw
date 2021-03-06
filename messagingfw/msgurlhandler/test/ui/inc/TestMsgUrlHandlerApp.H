// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the API definition for the classes 
// CTestMsgUrlHandlerApplication, CTestMsgUrlHandlerAppView, 
// CTestMsgUrlHandlerAppUi, CTestMsgUrlHandlerDocument.
// 
//

/**
 @file
*/

#ifndef __TESTMSGURLHANDLERAPP__H
#define __TESTMSGURLHANDLERAPP__H

#include <coeccntx.h>

#include <eikenv.h> 
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <techview/eikmenup.h> 

#include <techview/eikon.hrh> 
#include <msvstd.h>
#include <msvapi.h>

#include <testhandler.rsg>
#include "TestMsgUrlHandler.hrh"


/**
	@class
	Comments :	Defines the application's UID and manufactures a new, 
				blank document.
	@since 6.0
*/
class CTestMsgUrlHandlerApplication : public CEikApplication
	{
private: 

/**
	@fn				CreateDocumentL()
	Intended Usage	:	This function is inherited from class CApaApplication.
						This function is called by the UI framework at
						application start-up. It creates an instance of the
						document class.
	@since			6.0
	@return			CApaDocument
 */
	CApaDocument* CreateDocumentL();

/**
	@fn				AppDllUid() const
	Intended Usage	:	The function is called by the UI framework to ask for the
						application's UID.
	@since			6.0
	@return			TUid 
 */
	TUid AppDllUid() const;
	};


/**
	@class
	Comments :	This class displays the application data on screen and allows 
				user interaction.
	@since 6.0
*/
class CTestMsgUrlHandlerAppView : public CCoeControl
    {
public:

/**
	@fn				NewL(const TRect& aRect)
	Intended Usage	:	Static factory constructor. Uses two phase construction and 
						leaves nothing on the CleanupStack.
	@since			6.0
	@return			A pointer to the newly created CTestMsgUrlHandlerAppView object. 
	@pre 			None
	@post			A fully constructed and initialised CTestMsgUrlHandlerAppView object.
 */
	static CTestMsgUrlHandlerAppView* NewL(const TRect& aRect);

private:

/**
	@fn				CTestMsgUrlHandlerAppView()
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			6.0
 */
	CTestMsgUrlHandlerAppView();

/**
	@fn				~CTestMsgUrlHandlerAppView()
	Intended Usage	:	Destructor.
	@since			6.0
 */
	~CTestMsgUrlHandlerAppView();

/**
	@fn				ConstructL(const TRect& aRect)
	Intended Usage	:	Second phase of two-phase construction method. Does any 
						allocations required to fully construct the object.
	@since			6.0
	@pre 			First phase of construction is complete
	@post			The object is fully constructed and initialised.
 */
    void ConstructL(const TRect& aRect);

/**
	@fn				Draw(const TRect&) const
	Intended Usage	:	This function is inherited from CCoeControl.
						Draws the view - consists of a simple outline rectangle
						with text in the middle using the Normal font 
						supplied by the UI.
	@since			6.0
	@param			aRect 
	@return			CTestMsgUrlHandlerAppView
	@pre 			None
	@post			The view is drawn
 */
	void Draw(const TRect& /*aRect*/) const;

private:
	HBufC*  iText;
    };


/**
	@class
	Comments :	This class handles command generated by the menu items, toolbar
				buttons and shortcut keys. It delegates drawing and screen based
				interaction to the app view and other controls.
	@since 6.0
*/
class CTestMsgUrlHandlerAppUi : public CEikAppUi
    {

public:

/**
	@fn				CTestMsgUrlHandlerAppUi()
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			6.0
 */
	CTestMsgUrlHandlerAppUi();

private:

/**
	@fn				ConstructL()
	Intended Usage	:	Second phase of two-phase construction method. Does any 
						allocations required to fully construct the object.

						This function is called by Eikon hence no NewL().

						Starts the Idle time active object which calls the callback
						function. This starts the background task.
						This background task is encapsulated in the callback function. 
	@since			6.0
	@pre 			First phase of construction is complete
	@post			The object is fully constructed and initialised.
					The background task has started.
 */
    void ConstructL();

/**
	@fn				~CTestMsgUrlHandlerAppUi()
	Intended Usage	:	Destructor.
	@since			6.0
 */
	~CTestMsgUrlHandlerAppUi();

/**
	@fn				HandleCommandL(TInt aCommand)
	Intended Usage	:	Closes the application from the menu item and the shortcut
						key Ctrl-e.
	@since			6.0
	@param			aCommand 
	@return			void 
	@pre 			None
	@post			Application is closed if the menu item or the shortcut key is used. 
 */
	void HandleCommandL(TInt aCommand);	

/**
	@fn				LaunchMailEditorL(const TDesC& aUrl)
	Intended Usage	:	Parses the url (i.e the command line) and 
						launches the correct mail editor.
	Error Condition	:	This function leaves with KErrNotFound if the messaging
						scheme is not found.
	@since			6.0
	@param			aUrl		An url descriptor
	@return			void 
	@pre 			None
	@post			Parses the url and launches the correct mail editor.
 */
	void LaunchMailEditorL(const TDesC& aUrl);

private:
	CCoeControl* iAppView;
	};


/**
	@class
	Comments :	Launches the App UI
	@since 6.0
*/
class CTestMsgUrlHandlerDocument : public CEikDocument
	{
public:

/**
	@fn				NewL(CEikApplication& aApp)
	Intended Usage	:	Static factory constructor. Uses two phase construction and 
						leaves nothing on the CleanupStack.
	@since			6.0
	@return			A pointer to the newly created CTestMsgUrlHandlerDocument object. 
	@pre 			None
	@post			A fully constructed and initialised CTestMsgUrlHandlerDocument object.
 */
	static CTestMsgUrlHandlerDocument* NewL(CEikApplication& aApp);

private:

/**
	@fn				CTestMsgUrlHandlerDocument(CEikApplication& aApp)
	Intended Usage	:	Constructor. First phase of two-phase construction method. Does 
						non-allocating construction.
	@since			6.0
 */
	CTestMsgUrlHandlerDocument(CEikApplication& aApp);

/**
	@fn				ConstructL()
	Intended Usage	:	Second phase of two-phase construction method. Does any 
						allocations required to fully construct the object.
	@since			6.0
	@pre 			First phase of construction is complete
	@post			The object is fully constructed and initialised.
 */
	void ConstructL();

/**
	@fn				CEikAppUi* CreateAppUiL()
	Intended Usage	:	This function is inherited from CEikDocument.
						Launches the App UI.
	@since			6.0
	@return			CEikAppUi* 
	@pre 			App is created
	@post			App UI is launched.
 */
	CEikAppUi* CreateAppUiL();

	void CreateSMSServiceL();
	void CreateEmailServiceL();
	void ServiceIdL(TUid aMtm, TMsvId& rFirstId);
	};

class CDummyObserver : public CBase, public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent, TAny*, TAny*, TAny*) {};
	};


#endif
