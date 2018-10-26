///////////////////////////////////////////////////////////////////////
// BEToolbox
// Copyright � 1999-2015  Washington State Department of Transportation
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

#include "StdAfx.h"
#include "resource.h"
#include "GenCompChildFrame.h"
#include "GenCompDoc.h"

IMPLEMENT_DYNCREATE(CGenCompChildFrame, CEAFChildFrame)

CGenCompChildFrame::CGenCompChildFrame(void)
{
}

CGenCompChildFrame::~CGenCompChildFrame(void)
{
}

BEGIN_MESSAGE_MAP(CGenCompChildFrame, CEAFChildFrame)
   ON_WM_CREATE()
   ON_BN_CLICKED(IDC_UPDATE,OnUpdate)
   ON_BN_CLICKED(IDC_US,OnUSUnits)
   ON_BN_CLICKED(IDC_SI,OnSIUnits)
   ON_EN_CHANGE(IDC_N,OnModularRatioChanged)
   ON_BN_CLICKED(IDC_ADD_PRIMARY,OnAddPrimary)
   ON_BN_CLICKED(IDC_REMOVE_PRIMARY,OnRemovePrimary)
   ON_UPDATE_COMMAND_UI(IDC_REMOVE_PRIMARY,OnUpdateRemovePrimary)
   ON_BN_CLICKED(IDC_ADD_SECONDARY,OnAddSecondary)
   ON_BN_CLICKED(IDC_REMOVE_SECONDARY,OnRemoveSecondary)
   ON_UPDATE_COMMAND_UI(IDC_REMOVE_SECONDARY,OnUpdateRemoveSecondary)
END_MESSAGE_MAP()

BOOL CGenCompChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   // force this window to be maximized (not sure why WS_VISIBLE is required)
   cs.style |= WS_MAXIMIZE | WS_VISIBLE;

   return CEAFChildFrame::PreCreateWindow(cs);
}

int CGenCompChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CEAFChildFrame::OnCreate(lpCreateStruct) == -1)
      return -1;

   {
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if (!m_DlgBar.Create(this,IDD_GENCOMP,CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, AFX_IDW_CONTROLBAR_FIRST))
   {
      TRACE0("Failed to create dialog bar\n");
      return -1;
   }

   EnableToolTips();
   m_DlgBar.EnableToolTips();

   SetIcon(AfxGetApp()->LoadIcon(IDR_GENCOMP),TRUE);
   }

   return 0;
}

void CGenCompChildFrame::OnUpdate()
{
   m_DlgBar.UpdateData(TRUE);

   CGenCompDoc* pDoc = (CGenCompDoc*)GetActiveDocument();
   if ( pDoc )
   {
      pDoc->SetModifiedFlag();
      pDoc->UpdateAllViews(NULL);
   }
}

void CGenCompChildFrame::OnUSUnits()
{
   ASSERT(m_DlgBar.IsDlgButtonChecked(IDC_US) == 1 );
   SetUnitsMode(eafTypes::umUS);
}

void CGenCompChildFrame::OnSIUnits()
{
   ASSERT(m_DlgBar.IsDlgButtonChecked(IDC_SI) == 1 );
   SetUnitsMode(eafTypes::umSI);
}

void CGenCompChildFrame::OnAddPrimary()
{
   m_DlgBar.AddPrimaryPoint();

   CGenCompDoc* pDoc = (CGenCompDoc*)EAFGetDocument();
   pDoc->SetModifiedFlag();
}

void CGenCompChildFrame::OnRemovePrimary()
{
   m_DlgBar.RemoveSelectedPrimaryPoints();

   CGenCompDoc* pDoc = (CGenCompDoc*)EAFGetDocument();
   pDoc->SetModifiedFlag();
}

void CGenCompChildFrame::OnUpdateRemovePrimary(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_DlgBar.ArePrimaryPointsSelected() ? TRUE : FALSE);
}

void CGenCompChildFrame::OnModularRatioChanged()
{
   CGenCompDoc* pDoc = (CGenCompDoc*)EAFGetDocument();
   pDoc->SetModifiedFlag();
}

void CGenCompChildFrame::OnAddSecondary()
{
   m_DlgBar.AddSecondaryPoint();

   CGenCompDoc* pDoc = (CGenCompDoc*)EAFGetDocument();
   pDoc->SetModifiedFlag();
}

void CGenCompChildFrame::OnRemoveSecondary()
{
   m_DlgBar.RemoveSelectedSecondaryPoints();

   CGenCompDoc* pDoc = (CGenCompDoc*)EAFGetDocument();
   pDoc->SetModifiedFlag();
}

void CGenCompChildFrame::OnUpdateRemoveSecondary(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_DlgBar.AreSecondaryPointsSelected() ? TRUE : FALSE);
}

void CGenCompChildFrame::SetUnitsMode(eafTypes::UnitMode um)
{
   CEAFApp* pApp = EAFGetApp();
   if ( pApp->GetUnitsMode() != um )
   {
      UpdateData(TRUE); // update the data
      // before changing the units mode
      pApp->SetUnitsMode(um);
      UpdateUnitsDisplayMode();

      CGenCompDoc* pDoc = (CGenCompDoc*)GetActiveDocument();
      if ( pDoc )
      {
         pDoc->SetModifiedFlag();
      }
   }
}

void CGenCompChildFrame::UpdateUnitsDisplayMode()
{
   m_DlgBar.OnUnitsModeChanged();
}

#if defined _DEBUG
void CGenCompChildFrame::AssertValid() const
{
   CEAFChildFrame::AssertValid();
}
#endif

BOOL CGenCompChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle , CWnd* pParentWnd , CCreateContext* pContext)
{
   // CEAFChildFrame::LoadFrame is going to load resources based on nIDResource
   // We have to set the module context to this module so the load happens
   // correctly
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   if ( !CEAFChildFrame::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext) )
      return FALSE;

   return TRUE;
}

void CGenCompChildFrame::UpdateData(BOOL bUpdate)
{
   m_DlgBar.UpdateData(bUpdate);
}