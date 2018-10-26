///////////////////////////////////////////////////////////////////////
// BEToolbox
// Copyright � 1999-2018  Washington State Department of Transportation
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

// PGStableLiftingView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PGStableDoc.h"
#include "PGStableLiftingView.h"
#include "PGStableEffectivePrestressDlg.h"
#include <MFCTools\MFCTools.h>


// CPGStableLiftingView

IMPLEMENT_DYNCREATE(CPGStableLiftingView, CPGStableFormView)

CPGStableLiftingView::CPGStableLiftingView()
	: CPGStableFormView(CPGStableLiftingView::IDD)
{
}

CPGStableLiftingView::~CPGStableLiftingView()
{
}

void CPGStableLiftingView::DoDataExchange(CDataExchange* pDX)
{
   CPGStableFormView::DoDataExchange(pDX);

   DDX_Control(pDX, IDC_EC, m_ctrlEc);
   DDX_Control(pDX, IDC_FC, m_ctrlFc);
   DDX_Control(pDX, IDC_K1, m_ctrlK1);
   DDX_Control(pDX, IDC_K2, m_ctrlK2);

   CEAFApp* pApp = EAFGetApp();
   const unitmgtIndirectMeasure* pDispUnits = pApp->GetDisplayUnits();

   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();

   stbLiftingStabilityProblem problem = pDoc->GetLiftingStabilityProblem();

   stbTypes::WindType windLoadType;
   Float64 windLoad;
   problem.GetWindLoading(&windLoadType,&windLoad);
   DDX_CBEnum(pDX,IDC_WIND_TYPE,windLoadType);
   if ( windLoadType == stbTypes::Speed )
   {
      DDX_UnitValueAndTag(pDX,IDC_WIND_PRESSURE,IDC_WIND_PRESSURE_UNIT,windLoad,pDispUnits->Velocity);
   }
   else
   {
      DDX_UnitValueAndTag(pDX,IDC_WIND_PRESSURE,IDC_WIND_PRESSURE_UNIT,windLoad,pDispUnits->WindPressure);
   }

   bool bEvaluateStressesAtEquilibriumAngle = problem.EvaluateStressesAtEquilibriumAngle();
   DDX_CBItemData(pDX,IDC_STRESSES,bEvaluateStressesAtEquilibriumAngle);

   Float64 L;
   problem.GetSupportLocations(&L,&L);
   DDX_UnitValueAndTag(pDX,IDC_LIFT,IDC_LIFT_UNIT,L,pDispUnits->SpanLength);

   Float64 Fs,Fh,Ft;
   GetMaxFpe(&Fs,&Fh,&Ft);
   DDX_UnitValueAndTag(pDX,IDC_FPE_STRAIGHT,IDC_FPE_STRAIGHT_UNIT,Fs,pDispUnits->GeneralForce);
   DDX_UnitValueAndTag(pDX,IDC_FPE_HARPED,IDC_FPE_HARPED_UNIT,Fh,pDispUnits->GeneralForce);
   DDX_UnitValueAndTag(pDX,IDC_FPE_TEMP,IDC_FPE_TEMP_UNIT,Ft,pDispUnits->GeneralForce);

   Float64 sweepTolerance = problem.GetSweepTolerance();
   Float64 supportPlacementTolerance = problem.GetSupportPlacementTolerance();

   if ( pApp->GetUnitsMode() == eafTypes::umSI )
   {
      sweepTolerance *= 1000;
      DDX_Text(pDX,IDC_SWEEP_TOLERANCE,sweepTolerance);
      if ( pDX->m_bSaveAndValidate )
      {
         sweepTolerance /= 1000;
      }
      else
      {
         GetDlgItem(IDC_SWEEP_TOLERANCE_LABEL)->SetWindowText(_T(""));
         GetDlgItem(IDC_SWEEP_TOLERANCE_UNIT)->SetWindowText(_T("mm/m"));
      }
   }
   else
   {
      INT x = (INT)::RoundOff((1.0/(sweepTolerance*120.0)),1.0);
      DDX_Text(pDX,IDC_SWEEP_TOLERANCE,x);

      if ( pDX->m_bSaveAndValidate )
      {
         sweepTolerance = 1.0/(x*120.0);
      }
      else
      {
         GetDlgItem(IDC_SWEEP_TOLERANCE_LABEL)->SetWindowText(_T("1/"));
         GetDlgItem(IDC_SWEEP_TOLERANCE_UNIT)->SetWindowText(_T("in/10 ft"));
      }
   }

   DDX_UnitValueAndTag(pDX,IDC_SUPPORT_PLACEMENT_TOLERANCE,IDC_SUPPORT_PLACEMENT_TOLERANCE_UNIT,supportPlacementTolerance,pDispUnits->ComponentDim);

   Float64 impactUp, impactDown;
   problem.GetImpact(&impactUp,&impactDown);
   DDX_Percentage(pDX,IDC_IMPACT_UP,impactUp);
   DDX_Percentage(pDX,IDC_IMPACT_DOWN,impactDown);

   Float64 liftAngle = problem.GetLiftAngle();
   DDX_UnitValueAndTag(pDX,IDC_LIFT_ANGLE,IDC_LIFT_ANGLE_UNIT,liftAngle,pDispUnits->Angle);

   Float64 Yra = problem.GetYRollAxis();
   DDX_UnitValueAndTag(pDX,IDC_YRA,IDC_YRA_UNIT,Yra,pDispUnits->ComponentDim);

   bool bDirectCamber;
   Float64 camber;
   Float64 camberOffset;
   problem.GetCamber(&bDirectCamber,&camber);
   Float64 camberMultiplier = problem.GetCamberMultiplier();
   int camber_method;
   if ( bDirectCamber )
   {
      camber_method = 1;
      camberOffset = 0.02;
   }
   else
   {
      camber_method = 0;
      camberOffset = camber;
      camber = ::ConvertToSysUnits(1.0,unitMeasure::Inch);
   }
   DDX_Radio(pDX,IDC_CAMBER1,camber_method);
   DDX_Percentage(pDX,IDC_CAMBER_OFFSET,camberOffset);
   DDX_UnitValueAndTag(pDX,IDC_CAMBER,IDC_CAMBER_UNIT,camber,pDispUnits->ComponentDim);
   DDX_Text(pDX, IDC_CAMBER_MULTIPLIER, camberMultiplier);


   Float64 fci, frCoefficient;
   bool bComputeEci;
   pDoc->GetLiftingMaterials(&fci,&bComputeEci,&frCoefficient);
   bComputeEci = !bComputeEci;
   Float64 Eci = problem.GetConcrete().GetE(); // this is the computed value
   DDX_UnitValueAndTag(pDX,IDC_FC,IDC_FC_UNIT,fci,pDispUnits->Stress);
   DDX_Check_Bool(pDX,IDC_COMPUTE_EC,bComputeEci);
   DDX_UnitValueAndTag(pDX,IDC_EC,IDC_EC_UNIT,Eci,pDispUnits->ModE);
   Float64 K1 = pDoc->GetK1();
   Float64 K2 = pDoc->GetK2();
   DDX_Text(pDX,IDC_K1,K1);
   DDX_Text(pDX,IDC_K2,K2);

   DDX_UnitValueAndTag(pDX,IDC_FR_COEFFICIENT,IDC_FR_COEFFICIENT_UNIT,frCoefficient,pDispUnits->SqrtPressure);
   CString tag;
   if ( lrfdVersionMgr::GetVersion() < lrfdVersionMgr::SeventhEditionWith2016Interims )
   {
      tag = pApp->GetUnitsMode() == eafTypes::umSI ? _T("sqrt(f'ci (MPa))") : _T("sqrt(f'ci (KSI))");
   }
   else
   {
      tag = pApp->GetUnitsMode() == eafTypes::umSI ? _T("(lambda)sqrt(f'ci (MPa))") : _T("(lambda)sqrt(f'ci (KSI))");
   }
   DDX_Text(pDX,IDC_FR_COEFFICIENT_UNIT,tag);

   CString strLiftingTag(pApp->GetUnitsMode() == eafTypes::umUS ? _T("sqrt(f'ci (KSI))") : _T("sqrt(f'ci (MPa))"));

   DDX_Text(pDX,IDC_LIFTING_FS_CRACKING,m_LiftingCriteria.MinFScr);
   DDX_Text(pDX,IDC_LIFTING_FS_FAILURE,m_LiftingCriteria.MinFSf);
   DDX_Text(pDX,IDC_LIFTING_COMPRESSION,m_LiftingCriteria.CompressionCoefficient);
   DDX_UnitValue(pDX,IDC_LIFTING_TENSION,m_LiftingCriteria.TensionCoefficient,pDispUnits->SqrtPressure);
   DDX_Text(pDX,IDC_LIFTING_TENSION_UNIT,strLiftingTag);
   DDX_Check_Bool(pDX,IDC_CHECK_LIFTING_TENSION_MAX,m_LiftingCriteria.bMaxTension);
   DDX_UnitValueAndTag(pDX,IDC_LIFTING_TENSION_MAX,IDC_LIFTING_TENSION_MAX_UNIT,m_LiftingCriteria.MaxTension,pDispUnits->Stress);
   DDX_UnitValue(pDX,IDC_LIFTING_TENSION_WITH_REBAR,m_LiftingCriteria.TensionCoefficientWithRebar,pDispUnits->SqrtPressure);
   DDX_Text(pDX,IDC_LIFTING_TENSION_WITH_REBAR_UNIT,strLiftingTag);

   if ( pDX->m_bSaveAndValidate )
   {
      problem.GetConcrete().SetE(Eci);
      pDoc->SetK1(K1);
      pDoc->SetK2(K2);

      problem.SetSweepTolerance(sweepTolerance);
      problem.SetSupportPlacementTolerance(supportPlacementTolerance);

      problem.SetSupportLocations(L,L);

      problem.SetImpact(impactUp,impactDown);

      problem.SetLiftAngle(liftAngle);

      problem.SetWindLoading(windLoadType,windLoad);

      problem.EvaluateStressesAtEquilibriumAngle(bEvaluateStressesAtEquilibriumAngle);

      problem.SetYRollAxis(Yra);

      if ( camber_method == 0 )
      {
         bDirectCamber = false;
         camber = camberOffset;
      }
      else
      {
         bDirectCamber = true;
      }
      problem.SetCamber(bDirectCamber,camber);
      problem.SetCamberMultiplier(camberMultiplier);

      pDoc->SetLiftingStabilityProblem(problem);
      pDoc->SetLiftingMaterials(fci,!bComputeEci,frCoefficient);

      pDoc->SetLiftingCriteria(m_LiftingCriteria);

      if ( m_Strands.strandMethod == CPGStableStrands::Simplified )
      {
         m_Strands.FpeStraight = Fs;
         m_Strands.FpeHarped   = Fh;
         m_Strands.FpeTemp     = Ft;
      }
      pDoc->SetStrands(pDoc->GetGirderType(),LIFTING,m_Strands);
   }
}

BEGIN_MESSAGE_MAP(CPGStableLiftingView, CPGStableFormView)
   ON_BN_CLICKED(IDC_COMPUTE_EC, &CPGStableLiftingView::OnUserEc)
	ON_EN_CHANGE(IDC_FC, &CPGStableLiftingView::OnChangeFc)
   ON_EN_CHANGE(IDC_K1, &CPGStableLiftingView::OnChangeFc)
   ON_EN_CHANGE(IDC_K2, &CPGStableLiftingView::OnChangeFc)
   ON_EN_CHANGE(IDC_FPE_STRAIGHT, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_FPE_HARPED, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_FPE_TEMP, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_EC, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_FR_COEFFICIENT, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_LIFT, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_IMPACT_UP, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_IMPACT_DOWN, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_LIFT_ANGLE, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_WIND_PRESSURE, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_CAMBER_OFFSET, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_CAMBER, &CPGStableLiftingView::OnChange)
   ON_BN_CLICKED(IDC_CAMBER1, &CPGStableLiftingView::OnChange)
   ON_BN_CLICKED(IDC_CAMBER2, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_CAMBER_MULTIPLIER, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_YRA, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_SWEEP_TOLERANCE, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_LATERAL_SWEEP_INCREMENT, &CPGStableLiftingView::OnChange)
   ON_EN_CHANGE(IDC_SUPPORT_PLACEMENT_TOLERANCE, &CPGStableLiftingView::OnChange)
   ON_BN_CLICKED(IDC_EDIT_FPE, &CPGStableLiftingView::OnEditFpe)
   ON_COMMAND(ID_FILE_PRINT,&CPGStableLiftingView::OnPrint)
   ON_COMMAND(ID_FILE_PRINT_DIRECT,&CPGStableLiftingView::OnPrintDirect)
   ON_BN_CLICKED(IDC_CHECK_LIFTING_TENSION_MAX, &CPGStableLiftingView::OnClickedLiftingTensionMax)
   ON_WM_SIZE()
   ON_CBN_SELCHANGE(IDC_WIND_TYPE, &CPGStableLiftingView::OnWindTypeChanged)
	ON_MESSAGE(WM_HELP, OnCommandHelp)
   ON_CBN_SELCHANGE(IDC_STRESSES, &CPGStableLiftingView::OnChange)
   ON_COMMAND_RANGE(CCS_CMENU_BASE, CCS_CMENU_MAX, OnCmenuSelected)
END_MESSAGE_MAP()


// CPGStableLiftingView diagnostics

#ifdef _DEBUG
void CPGStableLiftingView::AssertValid() const
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
	CPGStableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPGStableLiftingView::Dump(CDumpContext& dc) const
{
	CPGStableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPGStableLiftingView message handlers

BOOL CPGStableLiftingView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,	DWORD dwRequestedStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext)
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   return CPGStableFormView::Create(lpszClassName,lpszWindowName,dwRequestedStyle,rect,pParentWnd,nID,pContext);
}

void CPGStableLiftingView::OnActivateView()
{
   UpdateData(FALSE);

   if ( m_strUserEc == _T("") )
   {
      m_ctrlEc.GetWindowText(m_strUserEc);
   }

   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();
   m_Strands = pDoc->GetStrands(pDoc->GetGirderType(),LIFTING);
   UpdateFpeControls();
   UpdateCriteriaControls();

   OnUserEc();

   RefreshReport();
}

void CPGStableLiftingView::OnDeactivateView()
{
   UpdateData();
}

void CPGStableLiftingView::OnUserEc()
{
   BOOL bEnable = ((CButton*)GetDlgItem(IDC_COMPUTE_EC))->GetCheck();
   GetDlgItem(IDC_EC)->EnableWindow(bEnable);
   GetDlgItem(IDC_EC_UNIT)->EnableWindow(bEnable);
   GetDlgItem(IDC_K1)->EnableWindow(!bEnable);
   GetDlgItem(IDC_K2)->EnableWindow(!bEnable);

   if (bEnable==FALSE)
   {
      m_ctrlEc.GetWindowText(m_strUserEc);
      UpdateEc();
   }
   else
   {
      m_ctrlEc.SetWindowText(m_strUserEc);
   }
}

void CPGStableLiftingView::UpdateEc()
{
   if ( IsDlgButtonChecked(IDC_COMPUTE_EC) )
   {
      return;
   }

    // need to manually parse strength and density values
   CString strFc, strDensity, strK1, strK2;
   m_ctrlFc.GetWindowText(strFc);
   m_ctrlK1.GetWindowText(strK1);
   m_ctrlK2.GetWindowText(strK2);

   CEAFApp* pApp = EAFGetApp();
   const unitmgtIndirectMeasure* pDispUnits = pApp->GetDisplayUnits();

   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();

   strDensity.Format(_T("%s"),FormatDimension(pDoc->GetDensity(),pDispUnits->Density,false));

   CString strEc = pDoc->UpdateEc(strFc,strDensity,strK1,strK2);
   m_ctrlEc.SetWindowText(strEc);
}

void CPGStableLiftingView::OnChangeFc() 
{
   UpdateEc();
   OnChange();
}

void CPGStableLiftingView::OnChange()
{
   UpdateData();
   RefreshReport();
}

void CPGStableLiftingView::UpdateFpeControls()
{
   BOOL bEnable = (m_Strands.strandMethod == CPGStableStrands::Simplified ? TRUE : FALSE);
   GetDlgItem(IDC_FPE_STRAIGHT)->EnableWindow(bEnable);
   GetDlgItem(IDC_FPE_HARPED)->EnableWindow(bEnable);
   GetDlgItem(IDC_FPE_TEMP)->EnableWindow(bEnable);

   GetDlgItem(IDC_EDIT_FPE)->ShowWindow(bEnable ? SW_HIDE : SW_SHOW);
}

void CPGStableLiftingView::UpdateCriteriaControls()
{
   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();
   BOOL bEnable = pDoc->GetCriteria() == gs_strCriteria ? TRUE : FALSE;
   GetDlgItem(IDC_IMPACT_UP)->EnableWindow(bEnable);
   GetDlgItem(IDC_IMPACT_DOWN)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFT_ANGLE)->EnableWindow(bEnable);
   GetDlgItem(IDC_WIND_TYPE)->EnableWindow(bEnable);
   GetDlgItem(IDC_WIND_PRESSURE)->EnableWindow(bEnable);
   GetDlgItem(IDC_CAMBER1)->EnableWindow(bEnable);
   GetDlgItem(IDC_CAMBER2)->EnableWindow(bEnable);
   GetDlgItem(IDC_CAMBER_OFFSET)->EnableWindow(bEnable);
   if (GetCheckedRadioButton(IDC_CAMBER1,IDC_CAMBER2) == IDC_CAMBER1 )
   {
      GetDlgItem(IDC_CAMBER)->EnableWindow(bEnable == FALSE ? FALSE : TRUE); // disable if we are disabling controls and camber is by the % method
   }
   else
   {
      GetDlgItem(IDC_CAMBER)->EnableWindow(TRUE); // if camber is a direct input value, always enable
   }
   GetDlgItem(IDC_CAMBER_MULTIPLIER)->EnableWindow(bEnable);
   GetDlgItem(IDC_YRA)->EnableWindow(bEnable);
   GetDlgItem(IDC_SWEEP_TOLERANCE)->EnableWindow(bEnable);
   GetDlgItem(IDC_SUPPORT_PLACEMENT_TOLERANCE)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_FS_CRACKING)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_FS_FAILURE)->EnableWindow(bEnable);
   GetDlgItem(IDC_STRESSES)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_COMPRESSION)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_TENSION)->EnableWindow(bEnable);
   GetDlgItem(IDC_CHECK_LIFTING_TENSION_MAX)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_TENSION_MAX)->EnableWindow(bEnable);
   GetDlgItem(IDC_LIFTING_TENSION_WITH_REBAR)->EnableWindow(bEnable);
}

void CPGStableLiftingView::OnEditFpe()
{
   AFX_MANAGE_STATE(AfxGetStaticModuleState());
   CPGStableEffectivePrestressDlg dlg;
   dlg.m_Strands = m_Strands;
   if ( dlg.DoModal() == IDOK )
   {
      m_Strands = dlg.m_Strands;

      Float64 Fs,Fh,Ft;
      GetMaxFpe(&Fs,&Fh,&Ft);
      CDataExchange dx(this,FALSE);
      CEAFApp* pApp = EAFGetApp();
      const unitmgtIndirectMeasure* pDispUnits = pApp->GetDisplayUnits();
      DDX_UnitValueAndTag(&dx,IDC_FPE_STRAIGHT,IDC_FPE_STRAIGHT_UNIT,Fs,pDispUnits->GeneralForce);
      DDX_UnitValueAndTag(&dx,IDC_FPE_HARPED,IDC_FPE_HARPED_UNIT,Fh,pDispUnits->GeneralForce);
      DDX_UnitValueAndTag(&dx,IDC_FPE_TEMP,IDC_FPE_TEMP_UNIT,Ft,pDispUnits->GeneralForce);
   }
}

void CPGStableLiftingView::OnInitialUpdate()
{
   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();

   m_Strands = pDoc->GetStrands(pDoc->GetGirderType(),LIFTING);
   m_LiftingCriteria = pDoc->GetLiftingCriteria();

   CWnd* pWnd = GetDlgItem(IDC_BROWSER);
   pWnd->ShowWindow(SW_HIDE);

   std::shared_ptr<CReportBuilder> pRptBuilder = pDoc->m_RptMgr.GetReportBuilder(_T("Lifting"));
   CReportDescription rptDesc = pRptBuilder->GetReportDescription();

   std::shared_ptr<CReportSpecificationBuilder> pRptSpecBuilder = pRptBuilder->GetReportSpecificationBuilder();
   m_pRptSpec = pRptSpecBuilder->CreateDefaultReportSpec(rptDesc);

   std::shared_ptr<CReportSpecificationBuilder> nullSpecBuilder;
   m_pBrowser = pDoc->m_RptMgr.CreateReportBrowser(GetSafeHwnd(),m_pRptSpec,nullSpecBuilder);

   m_pBrowser->GetBrowserWnd()->ModifyStyle(0,WS_BORDER);

   CComboBox* pcbStresses = (CComboBox*)GetDlgItem(IDC_STRESSES);
   pcbStresses->SetItemData(pcbStresses->AddString(_T("Include girder stability equilibrium angle in stress calculations")), (DWORD_PTR)true);
   pcbStresses->SetItemData(pcbStresses->AddString(_T("Ignore girder stability equilibrium angle in stress calculations")), (DWORD_PTR)false);

   CComboBox* pcbWindType = (CComboBox*)GetDlgItem(IDC_WIND_TYPE);
   pcbWindType->SetItemData(pcbWindType->AddString(_T("Wind Speed")),(DWORD_PTR)stbTypes::Speed);
   pcbWindType->SetItemData(pcbWindType->AddString(_T("Wind Pressure")),(DWORD_PTR)stbTypes::Pressure);

   CPGStableFormView::OnInitialUpdate();

   UpdateFpeControls();

   OnClickedLiftingTensionMax();
}

void CPGStableLiftingView::RefreshReport()
{
   if ( m_pRptSpec == nullptr )
      return;

   CPGStableDoc* pDoc = (CPGStableDoc*)GetDocument();

   // refresh the report
   m_pRptSpec = m_pBrowser->GetReportSpecification();
   std::shared_ptr<CReportBuilder> pBuilder = pDoc->m_RptMgr.GetReportBuilder( m_pRptSpec->GetReportName() );
   std::shared_ptr<rptReport> pReport = pBuilder->CreateReport( m_pRptSpec );
   m_pBrowser->UpdateReport( pReport, true );

}

void CPGStableLiftingView::GetMaxFpe(Float64* pFpeStraight,Float64* pFpeHarped,Float64* pFpeTemp)
{
   if ( m_Strands.strandMethod == CPGStableStrands::Simplified )
   {
      *pFpeStraight = m_Strands.FpeStraight;
      *pFpeHarped   = m_Strands.FpeHarped;
      *pFpeTemp     = m_Strands.FpeTemp;
   }
   else
   {
      Float64 FpeStraight = 0;
      Float64 FpeHarped = 0;
      Float64 FpeTemporary = 0;
      for (const auto& fpe : m_Strands.m_vFpe)
      {
         FpeStraight  = Max(FpeStraight,fpe.FpeStraight);
         FpeHarped    = Max(FpeHarped,fpe.FpeHarped);
         FpeTemporary = Max(FpeTemporary,fpe.FpeTemp);
      }

      *pFpeStraight = FpeStraight;
      *pFpeHarped   = FpeHarped;
      *pFpeTemp     = FpeTemporary;
   }
}

void CPGStableLiftingView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
   if ( EAFGetActiveView() == this )
   {
      if ( lHint == EAF_HINT_UNITS_CHANGING )
      {
         UpdateData(TRUE);
      }
      else if ( lHint == EAF_HINT_UNITS_CHANGED )
      {
         UpdateData(FALSE);
         OnChange();
      }
   }
}

void CPGStableLiftingView::OnClickedLiftingTensionMax()
{
   BOOL bEnable = IsDlgButtonChecked(IDC_CHECK_LIFTING_TENSION_MAX) == BST_CHECKED ? TRUE : FALSE;
   GetDlgItem(IDC_LIFTING_TENSION_MAX)->EnableWindow(bEnable);
}

LRESULT CPGStableLiftingView::OnCommandHelp(WPARAM, LPARAM lParam)
{
   EAFHelp( EAFGetDocument()->GetDocumentationSetName(), IDH_PGSTABLE_LIFTING_VIEW );
   return TRUE;
}

void CPGStableLiftingView::OnCmenuSelected(UINT id)
{
  UINT cmd = id-CCS_CMENU_BASE ;

  switch(cmd)
  {
  //case CCS_RB_EDIT:
  //   EditReport();
  //   break;

  case CCS_RB_FIND:
     m_pBrowser->Find();
     break;

  case CCS_RB_SELECT_ALL:
     m_pBrowser->SelectAll();
     break;
  case CCS_RB_PRINT:
     m_pBrowser->Print(true);
     break;

  case CCS_RB_REFRESH:
     m_pBrowser->Refresh();
     break;

  case CCS_RB_VIEW_SOURCE:
     m_pBrowser->ViewSource();
     break;

  case CCS_RB_VIEW_BACK:
     m_pBrowser->Back();
     break;

  case CCS_RB_VIEW_FORWARD:
     m_pBrowser->Forward();
     break;

  default:
     // must be a toc anchor
     ATLASSERT(CCS_RB_TOC <= cmd);
     m_pBrowser->NavigateAnchor(cmd-CCS_RB_TOC);
  }
}

void CPGStableLiftingView::OnSize(UINT nType, int cx, int cy)
{
   CPGStableFormView::OnSize(nType, cx, cy);

   if ( m_pBrowser )
   {
      // Convert a 7du x 7du rect into pixels
      CRect sizeRect(0,0,7,7);
      MapDialogRect(GetSafeHwnd(),&sizeRect);

      CRect clientRect;
      GetClientRect(&clientRect);

      CWnd* pWnd = GetDlgItem(IDC_BROWSER);
      CRect placeholderRect;
      pWnd->GetWindowRect(&placeholderRect);
      ScreenToClient(&placeholderRect);

      CRect browserRect;
      browserRect.left = placeholderRect.left;
      browserRect.top  = placeholderRect.top;
      browserRect.right = clientRect.right - sizeRect.Width();
      browserRect.bottom = clientRect.bottom - sizeRect.Height();

      m_pBrowser->Move(browserRect.TopLeft());
      m_pBrowser->Size(browserRect.Size() );

      Invalidate();
   }
}

void CPGStableLiftingView::OnPrint() 
{
   m_pBrowser->Print(true);
}

void CPGStableLiftingView::OnPrintDirect() 
{
   m_pBrowser->Print(false);
}

void CPGStableLiftingView::OnWindTypeChanged()
{
   CComboBox* pcbWindType = (CComboBox*)GetDlgItem(IDC_WIND_TYPE);
   int curSel = pcbWindType->GetCurSel();
   stbTypes::WindType windType = (stbTypes::WindType)pcbWindType->GetItemData(curSel);
   CDataExchange dx(this,false);
   CEAFApp* pApp = EAFGetApp();
   const unitmgtIndirectMeasure* pDispUnits = pApp->GetDisplayUnits();
   if ( windType == stbTypes::Speed )
   {
      DDX_Tag(&dx,IDC_WIND_PRESSURE_UNIT,pDispUnits->Velocity);
   }
   else
   {
      DDX_Tag(&dx,IDC_WIND_PRESSURE_UNIT,pDispUnits->WindPressure);
   }
   OnChange();
}
