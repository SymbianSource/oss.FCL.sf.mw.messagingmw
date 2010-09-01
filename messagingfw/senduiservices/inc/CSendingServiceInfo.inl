/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Encapsulates the ECom plugged sending service data.
*
*/



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceName
// Returns service human readable name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TPtrC CSendingServiceInfoImpl::ServiceName() const
    {
    return iServiceName ? TPtrC( *iServiceName ) : TPtrC();
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceMenuName
// Returns service human readable name for the "Send"-menu.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TPtrC CSendingServiceInfoImpl::ServiceMenuName() const
    {
    return iServiceMenuName ? TPtrC( *iServiceMenuName ) : TPtrC();
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceAddress
// Returns service address.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TPtrC CSendingServiceInfoImpl::ServiceAddress() const
    {
    return iServiceAddress ? TPtrC( *iServiceAddress ) : TPtrC();
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::SetServiceId
// Sets service id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSendingServiceInfoImpl::SetServiceId( TUid aServiceId )
    {
    iServiceId = aServiceId;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceId
// Returns service id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUid CSendingServiceInfoImpl::ServiceId() const
    {
    return iServiceId;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::SetServiceProviderId
// Sets service provider id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSendingServiceInfoImpl::SetServiceProviderId(
    TUid aServiceProviderId )
    {
    iServiceProviderId = aServiceProviderId;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceProviderId
// Returns service provider id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUid CSendingServiceInfoImpl::ServiceProviderId() const
    {
    return iServiceProviderId;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::SetServiceCapabilities
// Set service sending capabilities.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSendingServiceInfoImpl::SetServiceCapabilities(
    TSendingCapabilities aServiceCapabilities )
    {
    iServiceCapabilities = aServiceCapabilities;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceCapabilities
// Returns service sending capabilities.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TSendingCapabilities CSendingServiceInfoImpl::ServiceCapabilities() const
    {
    return iServiceCapabilities;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceId
// Returns service id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUid CSendingServiceInfoImpl::TechnologyTypeId() const
    {
    return iTechnologyTypeId;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::SetServiceProviderId
// Sets service provider id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSendingServiceInfoImpl::SetTechnologyTypeId(
    TUid aTechnologyTypeId )
    {
    iTechnologyTypeId = aTechnologyTypeId;
    }
    
// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::SetServiceCapabilities
// Set service sending capabilities.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CSendingServiceInfoImpl::SetServiceFeatures(
    TInt aServiceFeatures )
    {
    iServiceFeatures = aServiceFeatures;
    }

// -----------------------------------------------------------------------------
// CSendingServiceInfoImpl::ServiceCapabilities
// Returns service sending capabilities.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CSendingServiceInfoImpl::ServiceFeatures() const
    {
    return iServiceFeatures;
    }

//  End of File
