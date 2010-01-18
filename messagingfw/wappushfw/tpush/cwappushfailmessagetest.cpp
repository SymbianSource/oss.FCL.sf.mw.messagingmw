// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cwappushfailmessagetest.h"

//#include "pushtests.h"
//#include "dummywapstack.h"
//#include "WapPushMsgGen.h"

/** set test name to 'Service Indication'
	@param void
	@return string
		string represents test name
*/
const TDesC& CWapPushFailMessageTest::TestName()
	{
	_LIT(KTextFailMessageTest, "Failing Messages (were causing panics)");
	return KTextFailMessageTest;
	}

/** set the number of test messages to use */
void CWapPushFailMessageTest::NumberTestCasesToRun()
	{
	iNumberOfTestCases = 17;
	}

/** build different test messages to test specific SI reception aspects
	in the SI content handler processing code.
	@param TInt
		the number of test messages to create and use
	@return void
*/
void CWapPushFailMessageTest::PrepareTestMessageL(TInt aTestCase)
	{
	const TUint8 KFailMessageBody[] = 
		{
		0x02,
		0x05,
		0x6a,
		0x00,
		0x45,
		0xc6,
		0x0c,
		0x03,
		0x74,
		0x65,
		0x73,
		0x74,
		0x2e,
		0x6c,
		0x6f,
		0x63,
		0x61,
		0x6c,
		0x00,
		0x01,
		0x03,
		0x66,
		0x6f,
		0x6f,
		0x00,
		0x01,
		0x01
		};	

	iBodyBuf.Delete(0,KBufferSize);
	iBodyBuf.Append(KFailMessageBody, sizeof(KFailMessageBody));
	
	switch(aTestCase)
		{
	case 0:
		{
		const TUint8 KFailMessageHeader_0[] = 
			{
			0xAE,	// Content-type: application/vnd.wap.sic
			0xAF,	// X-Wap-Application-Id:
			0x1E 	// CORRUPT - should be Integer-value = Short-integer | Long-integer
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_0, sizeof(KFailMessageHeader_0));
		} break;
	case 1:
		{
		const TUint8 KFailMessageHeader_1[] = 
			{
			0xAE,	// Content-type: application/vnd.wap.sic
			0xAF,	// X-Wap-Application-Id:
			0x1F	// CORRUPT - this indicates a UIntVar, which is incorrect
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_1, sizeof(KFailMessageHeader_1));
		} break;
	case 2:
		{
		const TUint8 KFailMessageHeader_2[] = 
			{
			0x1F,
			0x00
			};
		iHeadersBuf.Copy(KFailMessageHeader_2, sizeof(KFailMessageHeader_2));
		} break;
	case 3:
		{
		const TUint8 KFailMessageHeader_3[] = 
			{
			0x1F,
			0x01,
			0x00
			};
		iHeadersBuf.Copy(KFailMessageHeader_3, sizeof(KFailMessageHeader_3));
		} break;
	case 4:
		{
		const TUint8 KFailMessageHeader_4[] = 
			{
			0x1F,
			0x02,
			0x00,
			0x00
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_4, sizeof(KFailMessageHeader_4));
		} break;
	case 5:
		{
		const TUint8 KFailMessageHeader_5[] = 
			{
			0x1F,
			0x02,
			0x80,
			0x00
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_5, sizeof(KFailMessageHeader_5));
		} break;
	case 6:
		{
		const TUint8 KFailMessageHeader_6[] = 
			{
			0x1F,
			0x02,
			0x1E,
			0x00
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_6, sizeof(KFailMessageHeader_6));
		} break;
	case 7:
		{
		const TUint8 KFailMessageHeader_7[] = 
			{
			0x1F,
			0x02,
			0x1F,
			0x00
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_7, sizeof(KFailMessageHeader_7));
		} break;
	case 8:
		{
		const TUint8 KFailMessageHeader_8[] = 
			{
			0x1F,
			0x00,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_8, sizeof(KFailMessageHeader_8));
		} break;
	case 9:
		{
		const TUint8 KFailMessageHeader_9[] = 
			{
			0x1F,
			0x80,
			0x00,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_9, sizeof(KFailMessageHeader_9));
		} break;
	case 10:
		{
		const TUint8 KFailMessageHeader_10[] = 
			{
			0x1F,
			0x9F,
			0xFF,
			0xFF,
			0xFF,
			0x00,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_10, sizeof(KFailMessageHeader_10));
		} break;
	case 11:
		{
		const TUint8 KFailMessageHeader_11[] = 
			{
			0x1F,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_11, sizeof(KFailMessageHeader_11));
		} break;
	case 12:
		{
		const TUint8 KFailMessageHeader_12[] = 
			{
			0x1F,
			0x01,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_12, sizeof(KFailMessageHeader_12));
		} break;
	case 13:
		{
		const TUint8 KFailMessageHeader_13[] = 
			{
			0x1F,
			0x02,
			0xAE,
			0x00
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_13, sizeof(KFailMessageHeader_13));
		} break;
	case 14:
		{
		const TUint8 KFailMessageHeader_14[] = 
			{
			0x1F,
			0x02,
			0xAE,
			0x80
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_14, sizeof(KFailMessageHeader_14));
		} break;
	case 15:
		{
		const TUint8 KFailMessageHeader_15[] = 
			{
			0x1F,
			0x02,
			0xAE,
			0x1F
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_15, sizeof(KFailMessageHeader_15));
		} break;
	case 16:
		{
		const TUint8 KFailMessageHeader_16[] = 
			{
			0x1F,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0xAE
			};
	
		iHeadersBuf.Copy(KFailMessageHeader_16, sizeof(KFailMessageHeader_16));
		} break;
		
	default:
		break;
		}
	}

/** confirm the message has been stored.
	Writes the number of stored messages found to the log file - if present.
	@param void
	@return TBool
		returns ETrue if messages found
		returns EFalse if no messages found 
*/
TBool CWapPushFailMessageTest::ConfirmMessagesSavedL()
	{
	return EFalse;
	}
