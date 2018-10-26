///////////////////////////////////////////////////////////////////////
// BEToolbox
// Copyright � 1999-2016  Washington State Department of Transportation
//                        Bridge and Structures Office
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the Alternate Route Open Source License as 
// published by the Washington State Department of Transportation, 
// Bridge and Structures Office.
//
// This program is distributed in the hope that it will be useful, but 
// distribution is AS IS, WITHOUT ANY WARRANTY; without even the implied 
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See 
// the Alternate Route Open Source License for more details.
//
// You should have received a copy of the Alternate Route Open Source 
// License along with this program; if not, write to the Washington 
// State Department of Transportation, Bridge and Structures Office, 
// P.O. Box  47340, Olympia, WA 98503, USA or e-mail 
// Bridge_Support@wsdot.wa.gov
///////////////////////////////////////////////////////////////////////

#pragma once


// CBoxGdrRptView view

#include <EAF\EAFReportView.h>
#include "BoxGdrChildFrame.h"

class CBoxGdrRptView : public CEAFReportView
{
	DECLARE_DYNCREATE(CBoxGdrRptView)

protected:
	CBoxGdrRptView();           // protected constructor used by dynamic creation
	virtual ~CBoxGdrRptView();

   virtual boost::shared_ptr<CReportSpecification> CreateReportSpecification();
   virtual boost::shared_ptr<CReportBrowser> CreateReportBrowser();
   virtual void RefreshReport();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

   virtual BOOL CanEditReport() { return FALSE; }

protected:

   CBoxGdrChildFrame* m_pFrame;

	DECLARE_MESSAGE_MAP()
public:
   virtual void OnInitialUpdate();
   virtual void OnUpdate(CView* pSender,LPARAM lHint,CObject* pHint);
};


