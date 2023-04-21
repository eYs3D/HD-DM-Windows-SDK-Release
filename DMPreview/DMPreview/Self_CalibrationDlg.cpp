
#include "stdafx.h"
#include "APC_Test.h"
#include "afxdialogex.h"
#include <ks.h>
#include <KsMedia.h>
#include "Self_CalibrationDlg.h"


IMPLEMENT_DYNAMIC( Self_CalibrationDlg, CDialog )
BEGIN_MESSAGE_MAP( Self_CalibrationDlg, CDialog )
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_EN_CHANGE(IDC_SELF_K_Basic_Schedule, &Self_CalibrationDlg::OnEnChangeSelfKBasicSchedule)
    ON_BN_CLICKED(IDC_SELF_K_Basic_Rub_Chk, &Self_CalibrationDlg::OnBnClickedSelfKBasicRubChk)
    ON_BN_CLICKED(IDC_IDC_SELF_K_Basic_Resetall_Btn, &Self_CalibrationDlg::OnBnClickedIdcSelfKBasicResetallBtn)
    ON_BN_CLICKED(IDC_SELF_K_Basic_Advance_Chk, &Self_CalibrationDlg::OnBnClickedSelfKBasicAdvanceChk)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_do_qulity_monitor, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategydoqulitymonitor)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_sleep_time_for_quality_check, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategysleeptimeforqualitycheck)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_threshold_for_bad_quality, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategythresholdforbadquality)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_threshold_auto_calibrarting, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategythresholdautocalibrarting)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_auto_calibration, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategyautocalibration)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_target_quality, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategytargetquality)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_calibration_mode, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategycalibrationmode)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_accuracy_level, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategyaccuracylevel)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_max_error_trials, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategymaxerrortrials)
    ON_EN_CHANGE(IDC_EDIT_Self_K_Advance_Strategy_tare_target, &Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategytaretarget)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Strategy_read_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategyreadbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Strategy_write_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategywritebtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Strategy_reset_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategyresetbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_calculate_score_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorcalculatescorebtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_do_calibration_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatordocalibrationbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_apply_before_reg_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplybeforeregbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_apply_after_reg_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplyafterregbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_apply_after_flash_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplyafterflashbtn)
    ON_BN_CLICKED(IDC_EDIT_Self_K_Advance_Operator_reset_all_btn, &Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorresetallbtn)
END_MESSAGE_MAP()

#define Check_BasicSchedule_Timer 1

Self_CalibrationDlg::Self_CalibrationDlg( void*&                  hApcDI,
                                      DEVSELINFO&             devSelInfo,
                                      const DEVINFORMATIONEX& devinfoEx,
                                      CWnd*                   pParent ,
                                      CPreviewImageDlg* pPreviewDlg)
    : CDialog(IDD_SELF_K_DIALOG, pParent), m_hApcDI(hApcDI), m_DevSelInfo(devSelInfo), m_xDevinfoEx(devinfoEx), m_pPreviewDlg(pPreviewDlg)
{
    m_nColorResWidth = 0;
    m_nColorResHeight = 0;
    m_cpDepth.SetPoint(NULL, NULL);
    m_IsLRD_Mode = false;
    mRun = false;
    mDumpColorRGBFrame = false;
    mBasicSchedule = 1;
}

Self_CalibrationDlg::~Self_CalibrationDlg()
{
    releaseSelfK();
}

BOOL Self_CalibrationDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    InitUI();
    return TRUE;
}

void Self_CalibrationDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

void Self_CalibrationDlg::InitUI()
{
    TRACE("InitUI\n");
    ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->SetCheck(BST_UNCHECKED);
    //((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->EnableWindow(FALSE);
    ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Rub_Chk))->SetCheck(BST_UNCHECKED);
    SetDlgItemInt(IDC_SELF_K_Basic_Schedule, mBasicSchedule);
    OnBnClickedSelfKBasicAdvanceChk();
    if (p_self_k)
    {
        // workaround to assign default value to calibration_mode and tare distance
        self_calibration::self_calibration_issue ret = p_self_k->resetConfiguration();
    }
}

void Self_CalibrationDlg::OnBnClickedSelfKBasicAdvanceChk()
{
    const BOOL checked = ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->GetCheck();
    GetDlgItem(IDC_SELF_K_Basic_Rub_Chk)->EnableWindow(!checked);
    ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Rub_Chk))->SetCheck(BST_UNCHECKED);
    OnBnClickedSelfKBasicRubChk();

    GetDlgItem(IDC_SELF_K_Basic_Schedule)->EnableWindow(!checked);
    //GetDlgItem(IDC_IDC_SELF_K_Basic_Resetall_Btn)->EnableWindow(!checked);

    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_do_qulity_monitor)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_sleep_time_for_quality_check)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_threshold_for_bad_quality)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_threshold_auto_calibrarting)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_auto_calibration)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_target_quality)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_calibration_mode)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_accuracy_level)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_max_error_trials)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_tare_target)->EnableWindow(checked);

    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_read_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_write_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Strategy_reset_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_calculate_score_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_do_calibration_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_apply_before_reg_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_apply_after_reg_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_apply_after_flash_btn)->EnableWindow(checked);
    GetDlgItem(IDC_EDIT_Self_K_Advance_Operator_reset_all_btn)->EnableWindow(checked);

    mRun = checked;
}

bool Self_CalibrationDlg::checkLRDMode()
{
    if (!m_pPreviewDlg->m_isPreviewed || !m_IsLRD_Mode)
    {
        CString  str;
        str.Format(_T("Must preview L+R+D"));
        AfxMessageBox(str, MB_ICONINFORMATION | MB_SYSTEMMODAL);
        return false;
    }
    return true;
}

void Self_CalibrationDlg::BGR2Gray(unsigned char *src, unsigned char *dst, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int r = (int)src[3 * i + 2];
        int g = (int)src[3 * i + 1];
        int b = (int)src[3 * i + 0];
        dst[i] = (r * 38 + g * 75 + b * 15) >> 7;
    }
}

void Self_CalibrationDlg::RGB2Gray(unsigned char *src, unsigned char *dst, int width, int height)
{
    for (int i = 0; i < width * height; i++) {
        int r = (int)src[3 * i + 0];
        int g = (int)src[3 * i + 1];
        int b = (int)src[3 * i + 2];
        dst[i] = (r * 38 + g * 75 + b * 15) >> 7;
    }
}

void Self_CalibrationDlg::OnBnClickedSelfKBasicRubChk()
{
    const BOOL checked = ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Rub_Chk))->GetCheck();
    if (checked)
    {
        if (!checkLRDMode())
        {
            ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Rub_Chk))->SetCheck(BST_UNCHECKED);
            return;
        }
        mRun = true;
        if (mBasicSchedule < 1)
        {
            CString  str;
            str.Format(_T("schedule must be more than one second"));
            AfxMessageBox(str, MB_ICONINFORMATION | MB_SYSTEMMODAL);
            ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Rub_Chk))->SetCheck(BST_UNCHECKED);
            SetDlgItemInt(IDC_SELF_K_Basic_Schedule, mBasicSchedule);
            return;
        }
        int schedule = (mBasicSchedule * 1000);
        SetTimer(Check_BasicSchedule_Timer, schedule, NULL);
        TRACE("SetTimer Check_BasicSchedule_Timer :%d\n", schedule);
        //((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->EnableWindow(FALSE);
    }
    else
    {
        mRun = false;
        KillTimer(Check_BasicSchedule_Timer);
        TRACE("KillTimer Check_BasicSchedule_Timer :%d\n");
        //((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->EnableWindow(TRUE);
    }
    GetDlgItem(IDC_SELF_K_Basic_Schedule)->EnableWindow(!checked);
    //GetDlgItem(IDC_IDC_SELF_K_Basic_Resetall_Btn)->EnableWindow(!checked);
    //OnBnClickedSelfKBasicAdvanceChk();
}

void Self_CalibrationDlg::OnEnChangeSelfKBasicSchedule()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    mBasicSchedule = GetDlgItemInt(IDC_SELF_K_Basic_Schedule);
}

void Self_CalibrationDlg::OnBnClickedIdcSelfKBasicResetallBtn()
{
    GetDlgItem(IDC_IDC_SELF_K_Basic_Resetall_Btn)->EnableWindow(false);

    bool isPreviewed = m_pPreviewDlg->m_isPreviewed;
    if (isPreviewed)
        m_pPreviewDlg->OnClosePreviewDlg((WPARAM)m_pPreviewDlg, 0);

    m_pPreviewDlg->m_pdlgVideoDeviceDlg->OnBnClickedEditForceOverride();

    if (isPreviewed)
        m_pPreviewDlg->OnBnClickedPreviewBtn();

    GetDlgItem(IDC_IDC_SELF_K_Basic_Resetall_Btn)->EnableWindow(true);
}

void Self_CalibrationDlg::checkNumberAndPoint(int nID)
{
    CString csAreaS;
    GetDlgItem(nID)->GetWindowText( csAreaS );

    int nStringLength = csAreaS.GetLength();
    int nDotCount = 0;
    for (int nIndex = 0; nIndex < nStringLength; nIndex++)
    {
        if (csAreaS[nIndex] == '.')
        {
            nDotCount++;
            if (nDotCount > 1)
            {
                CString csTmp;
                csTmp = csAreaS.Left(nIndex);
                csTmp += csAreaS.Right(csAreaS.GetLength() - nIndex - 1);
                GetDlgItem(nID)->SetWindowText(csTmp);
                return;
            }
        }
    }

    for (int nIndex = 0; nIndex < nStringLength; nIndex++)
    {
        if (csAreaS[ nIndex ] != '.' && ( csAreaS[ nIndex ] > '9' || csAreaS[ nIndex ] < '0'))
        {
            csAreaS = csAreaS.Left(nIndex) + csAreaS.Right(csAreaS.GetLength() - nIndex - 1);
            GetDlgItem(nID)->SetWindowText(csAreaS);
            return;
        }
    }
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategydoqulitymonitor()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategysleeptimeforqualitycheck()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategythresholdforbadquality()
{
    checkNumberAndPoint(IDC_EDIT_Self_K_Advance_Strategy_target_quality);
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategythresholdautocalibrarting()
{
    checkNumberAndPoint(IDC_EDIT_Self_K_Advance_Strategy_threshold_auto_calibrarting);
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategyautocalibration()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategytargetquality()
{
    checkNumberAndPoint(IDC_EDIT_Self_K_Advance_Strategy_target_quality);
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategycalibrationmode()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategyaccuracylevel()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategymaxerrortrials()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void Self_CalibrationDlg::OnEnChangeEditSelfKAdvanceStrategytaretarget()
{
    checkNumberAndPoint(IDC_EDIT_Self_K_Advance_Strategy_tare_target);
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategyreadbtn()
{
    bool is_monitor = 0;
    int second = 0;
    float score1 = 0;
    float score2 = 0;
    bool is_auto = 0;
    float target = 0;
    self_calibration::calibration_mode mode = self_calibration::calibration_mode::Tare;
    int level = 0;
    int number = 0;
    float distance = 0;

    if (p_self_k)
    {
        self_calibration::self_calibration_issue ret = self_calibration::self_calibration_issue::Self_Calibration_Success;
        ret = p_self_k->get_do_quality_monitoring(&is_monitor);
        ret = p_self_k->get_period_for_quality_check(&second);
        ret = p_self_k->get_low_quality_threshold_to_trigger_warning(&score1);
        ret = p_self_k->get_quality_threshold_to_trigger_parameter_estimation_for_recovery(&score2);
        ret = p_self_k->get_automatically_triger_parameter_estimation_for_recovery(&is_auto);
        ret = p_self_k->get_target_quality_threshold_for_recovery(&target);
        ret = p_self_k->get_recovery_mode(&mode);
        ret = p_self_k->get_accuracy_level(&level);
        ret = p_self_k->get_max_recovery_trials(&number);
        ret = p_self_k->get_tare_distance(&distance);

        TRACE("%d %d %f %f %d %f %d %d %d %f\n", is_monitor, second, score1, score2, is_auto, target, mode, level, number, distance);
        CString csText;
        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_do_qulity_monitor, is_monitor);
        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_sleep_time_for_quality_check, second);

        csText.Format(L"%.1f", score1);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_threshold_for_bad_quality, csText);
        csText.Format(L"%.1f", score2);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_threshold_auto_calibrarting, csText);

        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_auto_calibration, is_auto);

        csText.Format(L"%.1f", target);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_target_quality, csText);

        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_calibration_mode, mode);
        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_accuracy_level, level);
        SetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_max_error_trials, number);

        csText.Format(L"%.1f", distance);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_tare_target, csText);
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategywritebtn()
{
    CString csText;
    bool is_monitor = GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_do_qulity_monitor);
    int second = GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_sleep_time_for_quality_check);

    GetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_threshold_for_bad_quality, csText);
    float score1 = (float)_wtof(csText);
    GetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_threshold_auto_calibrarting, csText);
    float score2 = (float)_wtof(csText);

    bool is_auto = GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_auto_calibration);

    GetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_target_quality, csText);
    float target = (float)_wtof(csText);

    self_calibration::calibration_mode mode = (self_calibration::calibration_mode) GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_calibration_mode);
    int level = GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_accuracy_level);
    int number = GetDlgItemInt(IDC_EDIT_Self_K_Advance_Strategy_max_error_trials);

    GetDlgItemText(IDC_EDIT_Self_K_Advance_Strategy_tare_target, csText);
    float distance = (float)_wtof(csText);

    if (p_self_k)
    {
        self_calibration::self_calibration_issue ret = self_calibration::self_calibration_issue::Self_Calibration_Success;
        ret = p_self_k->set_do_quality_monitoring(is_monitor);
        ret = p_self_k->set_period_for_quality_check(second);
        ret = p_self_k->set_low_quality_threshold_to_trigger_warning(score1);
        ret = p_self_k->set_quality_threshold_to_trigger_parameter_estimation_for_recovery(score2);
        ret = p_self_k->set_automatically_triger_parameter_estimation_for_recovery(is_auto);
        ret = p_self_k->set_target_quality_threshold_for_recovery(target);
        ret = p_self_k->set_recovery_mode(mode);
        ret = p_self_k->set_accuracy_level(level);
        ret = p_self_k->set_max_recovery_trials(number);
        ret = p_self_k->set_tare_distance(distance);
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceStrategyresetbtn()
{
    if (p_self_k)
    {
        self_calibration::self_calibration_issue ret = p_self_k->resetConfiguration();
        OnBnClickedEditSelfKAdvanceStrategyreadbtn();
    }
    AfxMessageBox(_T("done"));
}

bool Self_CalibrationDlg::updateMonitorState()
{
    self_calibration::monitor_state state;
    self_calibration::self_calibration_issue ret = p_self_k->getMonitorState(&state);
    if (ret != self_calibration::Self_Calibration_Success)
        return false;

    switch (state)
    {
        case self_calibration::monitor_state::Initing:
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_monitor_state, L"Initing");
            return true;
        case self_calibration::monitor_state::Idle:
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_monitor_state, L"Idle");
            return true;
        case self_calibration::monitor_state::Suspended:
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_monitor_state, L"Suspended");
            return true;
        case self_calibration::monitor_state::Calibrating:
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_monitor_state, L"Calibrating");
            return true;
        case self_calibration::monitor_state::CalculatingHealth:
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_monitor_state, L"CalculatingHealth");
            return true;
    }
    return true;
}

bool Self_CalibrationDlg::updateCurrentPars()
{
    float score = 0;
    self_calibration::parameter_state current_state;
    self_calibration::self_calibration_issue ret = p_self_k->getStateOfCurrentPars(&current_state);
    if (ret == self_calibration::Self_Calibration_Success)
    {
        if (current_state == self_calibration::Waiting)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_state, L"Waiting");
        else if (current_state == self_calibration::Good)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_state, L"Good");
        else if (current_state == self_calibration::CouldbeImproved)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_state, L"CouldBeImproved");
        else if (current_state == self_calibration::NeedCalibration)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_state, L"NeedCalibration");
        else
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_state, L"Unknown");
    }
    else
    {
        return false;
    }
    ret = p_self_k->getScroeOfCurrentPars(&score);
    if (ret == self_calibration::Self_Calibration_Success)
    {
        CString csText;
        csText.Format(L"%.2f", score);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_before_score, csText);
        return true;
    }
    return false;
}

bool Self_CalibrationDlg::updateEstimatedPars()
{
    float score = 0;
    self_calibration::parameter_state current_state;
    self_calibration::self_calibration_issue ret = p_self_k->getStateOfEstimatedPars(&current_state);
    if (ret == self_calibration::Self_Calibration_Success)
    {
        if (current_state == self_calibration::Waiting)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_state, L"Waiting");
        else if (current_state == self_calibration::Good)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_state, L"Good");
        else if (current_state == self_calibration::CouldbeImproved)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_state, L"CouldBeImproved");
        else if (current_state == self_calibration::NeedCalibration)
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_state, L"NeedCalibration");
        else
            SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_state, L"Unknown");
    }
    else
    {
        return false;
    }
    ret = p_self_k->getScroeOfEstimatedPars(&score);
    if (ret == self_calibration::Self_Calibration_Success)
    {
        CString csText;
        csText.Format(L"%.2f", score);
        SetDlgItemText(IDC_EDIT_Self_K_Advance_Monitor_after_score, csText);
        return true;
    }
    return false;
}

void Self_CalibrationDlg::getErrorMessage(self_calibration::self_calibration_issue error, std::string& message)
{
    switch (error)
    {
        case self_calibration::self_calibration_issue::Self_Calibration_Success:
            message.append("Success");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_Fail:
            message.append("Fail");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_ParameterError:
            message.append("ParameterError");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_FrameError:
            message.append("FrameError");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_TareDepthError:
            message.append("TareDepthError");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_PlaneError:
            message.append("PlaneError");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_QualityBad:
            message.append("QualityBad");
            break;
        case self_calibration::self_calibration_issue::Self_Calibration_NotReady:
            message.append("NotReady");
            break;
    }
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorcalculatescorebtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        float score = 0;
        BGR2Gray(m_color, m_gray_sbs, color_width, color_height);
        self_calibration::self_calibration_issue ret = p_self_k->doCalculatingForCurrentHealth(score);
        if (ret == self_calibration::Self_Calibration_Success && updateMonitorState() && updateCurrentPars())
        {
            AfxMessageBox(_T("done"));
        }
        else
        {
            std::string error_message("error:");
            getErrorMessage(ret, error_message);
            CString cstr(error_message.c_str());
            AfxMessageBox(cstr);
        }
    }
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatordocalibrationbtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        float score = 0;
        BGR2Gray(m_color, m_gray_sbs, color_width, color_height);
        self_calibration::self_calibration_issue ret = p_self_k->doParameterEstimationForRecovery();
        if (ret == self_calibration::Self_Calibration_Success && updateMonitorState() && updateCurrentPars() && updateEstimatedPars())
        {
            AfxMessageBox(_T("done"));
        }
        else
        {
            std::string error_message("error:");
            getErrorMessage(ret, error_message);
            CString cstr(error_message.c_str());
            AfxMessageBox(cstr);
        }
    }
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplybeforeregbtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        self_calibration::module_parameters new_pars;
        p_self_k->getCurrentParameters(&new_pars);
        eSPCtrl_RectLogData *tmp_rectifyLogData = nullptr;
        tmp_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
        memset(tmp_rectifyLogData, 0, sizeof(eSPCtrl_RectLogData));

        APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, tmp_rectifyLogData, res_depth_option_id);
        ModuleParameter2RectLogData(new_pars, *tmp_rectifyLogData);
        WriteRectLogData_temp(*tmp_rectifyLogData, m_hApcDI, &m_DevSelInfo);
        free(tmp_rectifyLogData);
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplyafterregbtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        self_calibration::module_parameters new_pars;
        p_self_k->getEstimatedParameters(&new_pars);
        eSPCtrl_RectLogData *tmp_rectifyLogData = nullptr;
        tmp_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
        memset(tmp_rectifyLogData, 0, sizeof(eSPCtrl_RectLogData));

        APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, tmp_rectifyLogData, res_depth_option_id);
        ModuleParameter2RectLogData(new_pars, *tmp_rectifyLogData);
        WriteRectLogData_temp(*tmp_rectifyLogData, m_hApcDI, &m_DevSelInfo);
        free(tmp_rectifyLogData);
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorapplyafterflashbtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        self_calibration::module_parameters new_pars;
        p_self_k->getEstimatedParameters(&new_pars);
        eSPCtrl_RectLogData *tmp_rectifyLogData = nullptr;
        tmp_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
        memset(tmp_rectifyLogData, 0, sizeof(eSPCtrl_RectLogData));

        APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, tmp_rectifyLogData, res_depth_option_id);
        ModuleParameter2RectLogData(new_pars, *tmp_rectifyLogData);
        WriteRectification(*tmp_rectifyLogData, m_hApcDI, &m_DevSelInfo, res_depth_option_id);
        WriteRectifyLogData(*tmp_rectifyLogData, m_hApcDI, &m_DevSelInfo, res_depth_option_id);
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::OnBnClickedEditSelfKAdvanceOperatorresetallbtn()
{
    if (!checkLRDMode())
    {
        return;
    }
    if (p_self_k)
    {
        p_self_k->resetCurrentAndEstimatedParametersToDefault();
    }
    AfxMessageBox(_T("done"));
}

void Self_CalibrationDlg::SetImageParams(const int imgWidth, const int imgHeight, const BOOL isLRD_Mode, const BOOL isRectified, const CPoint& cpDepth)
{
    m_nColorResWidth = imgWidth;
    m_nColorResHeight = imgHeight;
    m_cpDepth = cpDepth;
    m_IsLRD_Mode = isLRD_Mode;
    m_IsRectified = isRectified;

    //
    color_height = imgHeight;
    color_width = imgWidth;
    depth_width = cpDepth.x;
    depth_height = cpDepth.y;

    m_color = (unsigned char*)malloc(color_width*color_height * 3 * sizeof(unsigned char));
    m_depth = (unsigned char*)malloc(depth_height*depth_width * 2 * sizeof(unsigned char));
    m_gray_sbs = (unsigned char*)malloc(color_width*color_height * sizeof(unsigned char));
    g_p_color = m_color;
    g_p_depth = m_depth;

    unsigned short value;
    APC_GetFWRegister(m_hApcDI, &m_DevSelInfo, 0xf6, &value, FG_Address_1Byte | FG_Value_1Byte);
    res_depth_option_id = int(value);

    initSelfK();
}

void Self_CalibrationDlg::ApplyInputImage(APCImageType::Value imgType, int imgId, unsigned char *imgBuf, int imgSize, int width, int height, int serialNumber)
{
    std::lock_guard< std::mutex > lock(m_imgBufMutex);
    if (mRun)
    {
        if (APCImageType::IsImageColor(imgType))
        {
            if (imgId == APC_Stream_Color)
            {
                APC_ColorFormat_to_BGR24(m_hApcDI, &m_DevSelInfo, (unsigned char *)g_p_color, imgBuf, imgSize, width, height, imgType);
                // g_b_color_image_ready = true;
#if 0
                int color_size = width * height * 3;
                if (!mDumpColorRGBFrame)
                {
                    CFile pFile;
                    CString fileName;
                    DWORD timestamp = GetTickCount();
                    string path = GetCurrentModuleFolder();
                    fileName.Format(_T("self_k_test_%d.rgb24"), timestamp);
                    int fd = pFile.Open(fileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate);
                    if (fd)
                    {
                        try
                        {
                            pFile.Write(g_p_color, color_size);
                            pFile.Flush();
                            pFile.Close();
                            mDumpColorRGBFrame = true;
                        }
                        catch (TCHAR* err)
                        {
                            TRACE(err);
                        }
                    }

                }
#endif
            }
        }
        //else if (APCImageType::IsImageDepth(imgType))
        //{
            //memcpy(g_p_depth, imgBuf, imgSize);
            // g_b_depth_image_ready = true;
        //}
    }
}

int Self_CalibrationDlg::RectLogData2ModuleParameter(eSPCtrl_RectLogData &rectifyData, self_calibration::module_parameters &moduleParameter)
{
    moduleParameter.height = rectifyData.InImgHeight;
    moduleParameter.width = rectifyData.InImgWidth / 2;
    moduleParameter.fixed_focus = rectifyData.NewCamMat1[0];

    for (int i = 0; i < 9; i++)
    {
        moduleParameter.left_intrinsic[i] = rectifyData.CamMat1[i];
        moduleParameter.right_intrinsic[i] = rectifyData.CamMat2[i];
        moduleParameter.left_rotation[i] = rectifyData.LRotaMat[i];
        moduleParameter.right_rotation[i] = rectifyData.RRotaMat[i];
        moduleParameter.extrinsic_rotation[i] = rectifyData.RotaMat[i];
    }
    for (int i = 0; i < 8; i++)
    {
        moduleParameter.left_distortion[i] = rectifyData.CamDist1[i];
        moduleParameter.right_distortion[i] = rectifyData.CamDist2[i];
    }
    for (int i = 0; i < 3; i++)
    {
        moduleParameter.extrinsic_translation[i] = rectifyData.TranMat[i];
    }
    for (int i = 0; i < 16; i++)
    {
        moduleParameter.reprojection[i] = rectifyData.ReProjectMat[i];
    }
    for (int i = 0; i < 12; i++)
    {
        moduleParameter.left_projection[i] = rectifyData.NewCamMat1[i];
        moduleParameter.right_projection[i] = rectifyData.NewCamMat2[i];
    }

    return 0;
}

int Self_CalibrationDlg::ModuleParameter2RectLogData(self_calibration::module_parameters &moduleParameter, eSPCtrl_RectLogData &rectifyData)
{
    for (int i = 0; i < 9; i++)
    {
        rectifyData.CamMat1[i] = (float)moduleParameter.left_intrinsic[i];
        rectifyData.CamMat2[i] = (float)moduleParameter.right_intrinsic[i];
        rectifyData.LRotaMat[i] = (float)moduleParameter.left_rotation[i];
        rectifyData.RRotaMat[i] = (float)moduleParameter.right_rotation[i];
        rectifyData.RotaMat[i] = (float)moduleParameter.extrinsic_rotation[i];
    }
    for (int i = 0; i < 8; i++)
    {
        rectifyData.CamDist1[i] = (float)moduleParameter.left_distortion[i];
        rectifyData.CamDist2[i] = (float)moduleParameter.right_distortion[i];
    }
    for (int i = 0; i < 3; i++)
    {
        rectifyData.TranMat[i] = (float)moduleParameter.extrinsic_translation[i];
    }
    for (int i = 0; i < 16; i++)
    {
        rectifyData.ReProjectMat[i] = (float)moduleParameter.reprojection[i];
    }
    for (int i = 0; i < 12; i++)
    {
        rectifyData.NewCamMat1[i] = (float)moduleParameter.left_projection[i];
        rectifyData.NewCamMat2[i] = (float)moduleParameter.right_projection[i];
    }

    return 0;
}

int Self_CalibrationDlg::initSelfK()
{
    self_calibration::module_parameters pars;
    self_calibration::calibration_mode mode = self_calibration::calibration_mode::Fix_extrinsic;

    if (!p_rectifyLogData)
    {
        p_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
        memset(p_rectifyLogData, 0, sizeof(eSPCtrl_RectLogData));
    }

    int nRet = APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, p_rectifyLogData, res_depth_option_id);
    RectLogData2ModuleParameter(*p_rectifyLogData, pars);

    BGR2Gray(m_color, m_gray_sbs, color_width, color_height);

    int kk = p_self_k->init(pars, m_gray_sbs, mode, 200);

    if (m_IsRectified)
    {
        p_self_k->set_input_image_type(self_calibration::input_image_type::rectified);
    }
    else
    {
        p_self_k->set_input_image_type(self_calibration::input_image_type::raw);
    }

    if (!kk)
    {
        TRACE("Init self calibration success.\n");
    }
    else {
        TRACE("Init self calibration fail.\n");
    }

    return 0;
}

void Self_CalibrationDlg::disableSelfK()
{
    ((CButton*)GetDlgItem(IDC_SELF_K_Basic_Advance_Chk))->SetCheck(BST_UNCHECKED);
    OnBnClickedSelfKBasicAdvanceChk();
}

int Self_CalibrationDlg::releaseSelfK()
{
    if (p_rectifyLogData)
    {
        free(p_rectifyLogData);
        p_rectifyLogData = nullptr;
    }

    if(m_color) {
        free(m_color);
        m_color = nullptr;
    }

    if(m_depth) {
        free(m_depth);
        m_depth = nullptr;
    }

    if (m_gray_sbs) {
        free(m_gray_sbs);
        m_gray_sbs = nullptr;
    }

    return true;
}

int Self_CalibrationDlg::executeSelfK()
{
    BGR2Gray(m_color, m_gray_sbs, color_width, color_height);

    self_calibration::parameter_state par_state;
    self_calibration::module_parameters new_pars;
    self_calibration::self_calibration_issue ret = p_self_k->doParameterEstimationForRecovery();

    p_self_k->getStateOfEstimatedPars(&par_state);

    if (par_state == self_calibration::parameter_state::Good)
    {
        p_self_k->getEstimatedParameters(&new_pars);
        eSPCtrl_RectLogData *tmp_rectifyLogData = nullptr;
        tmp_rectifyLogData = (eSPCtrl_RectLogData *)malloc(sizeof(eSPCtrl_RectLogData));
        memset(tmp_rectifyLogData, 0, sizeof(eSPCtrl_RectLogData));

        APC_GetRectifyMatLogData(m_hApcDI, &m_DevSelInfo, tmp_rectifyLogData, res_depth_option_id);
        ModuleParameter2RectLogData(new_pars, *tmp_rectifyLogData);
        WriteRectLogData_temp(*tmp_rectifyLogData, m_hApcDI, &m_DevSelInfo);
        p_self_k->updateCurrentParameters();
        free(tmp_rectifyLogData);
    }

    return 0;
}

void Self_CalibrationDlg::OnTimer(UINT_PTR nIDEvent) {

    switch (nIDEvent)
    {
        case Check_BasicSchedule_Timer:
            TRACE("Check_BasicSchedule_Timer Event:%d\n");
            executeSelfK();
            break;
        default:
            break;
    }
    CDialog::OnTimer(nIDEvent);
}
