
#include "stdafx.h"
#include "APC_Test.h"
#include "DepthDlg.h"
#include "DistanceAccuracyDlg.h"

#define WM_UPDATE_UI WM_USER + 100

#define TemporalNoiseCount 9

#define UPDATE_INTERVAL_MS 300

enum
{
    _80,
    _60,
    _40,
    _20
};

IMPLEMENT_DYNAMIC( DistanceAccuracyDlg, CDialogEx )
BEGIN_MESSAGE_MAP( DistanceAccuracyDlg, CDialogEx )
    ON_CBN_SELCHANGE( IDC_CB_INTEREST, &DistanceAccuracyDlg::OnCbnSelchangeCbInterest )
    ON_BN_CLICKED(IDC_CHK_GROUNDTRUTH, &DistanceAccuracyDlg::OnBnClickedChkGroundtruth)
    ON_MESSAGE( WM_UPDATE_UI, &DistanceAccuracyDlg::OnUpdateUI )
    ON_EN_CHANGE( IDC_ED_GROUNDTRUTH, &DistanceAccuracyDlg::OnEnChangeEdGroundtruth )
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_CB_DEPTHLIST, &DistanceAccuracyDlg::OnCbnSelchangeCbDepthlist)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_ADJUST_FOCAL_LENGTH, &DistanceAccuracyDlg::OnBnClickedButtonAdjustFocalLength)
END_MESSAGE_MAP()

DistanceAccuracyDlg::DistanceAccuracyDlg( void*& hApcDI, DEVSELINFO& devSelInfo, CPreviewImageDlg* pPreviewDlg ) : 
	CDialogEx( IDD_DLG_DISTANCE_ACCURACY, NULL ),
	m_hApcDI(hApcDI),
	m_devSelInfo(devSelInfo),
    m_pPreviewDlg( pPreviewDlg ),
    m_bEnable( FALSE ),
    m_bStop( FALSE ),
    m_bRunAccuracy( FALSE ),
    m_iGroundTruth( NULL ),
    m_Width( NULL ),
    m_Height( NULL ),
    m_pDepthDlg( NULL )
{
    m_rtRegion.SetRectEmpty();
	m_csPixelUnit.Empty();
	m_csLeftFx.Empty();
	m_csLeftFy.Empty();
	m_csRightFx.Empty();
	m_csRightFy.Empty();
}

DistanceAccuracyDlg::~DistanceAccuracyDlg()
{
}

void DistanceAccuracyDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL DistanceAccuracyDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CComboBox* pComboBox = ( CComboBox* )GetDlgItem( IDC_CB_INTEREST );

    pComboBox->AddString( L"80%" );
    pComboBox->AddString( L"60%" );
    pComboBox->AddString( L"40%" );
    pComboBox->AddString( L"20%" );
    pComboBox->SetCurSel( NULL );

    CComboBox* pDepthList = ( ( CComboBox* )GetDlgItem( IDC_CB_DEPTHLIST ) );
    pDepthList->AddString( L"Depth 0" );
    pDepthList->AddString( L"Depth 1" );
    pDepthList->AddString( L"Depth 2" );
    pDepthList->SetCurSel( NULL );

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PIXEL_UNIT))->SetRange(0, 10, true);

	bool bAujustingFocalLengthVisible = m_pPreviewDlg->IsDevicePid(APC_PID_8062);
	GetDlgItem(IDC_BUTTON_ADJUST_FOCAL_LENGTH)->EnableWindow(bAujustingFocalLengthVisible ? SW_SHOW : SW_HIDE);

    OnCbnSelchangeCbInterest();

    return TRUE;
}

void DistanceAccuracyDlg::OnDestroy()
{
    m_bRunAccuracy = FALSE;
    m_bStop = TRUE;

    m_event.notify_all();

    auto ThreadFree = [ = ] ( std::unique_ptr< std::thread >& Thread )
    {
        if ( Thread )
        {
            Thread->join();

            Thread.reset();
        }
    };
    ThreadFree( m_ThreadAccuracy );
    ThreadFree( m_ThreadSpatialNoise );
    ThreadFree( m_ThreadTemporalNoise );

    CDialogEx::OnDestroy();
}

void DistanceAccuracyDlg::OnBnClickedChkGroundtruth()
{
    m_bEnable = ( ( CButton* )GetDlgItem( IDC_CHK_GROUNDTRUTH ) )->GetCheck();

    SetDlgItemText( IDC_ED_GROUNDTRUTH, L"" );

    GetDlgItem( IDC_ED_GROUNDTRUTH )->EnableWindow( m_bEnable );

    OnEnChangeEdGroundtruth();
}

void DistanceAccuracyDlg::OnCbnSelchangeCbInterest()
{
    switch ( ( ( CComboBox* )GetDlgItem( IDC_CB_INTEREST ) )->GetCurSel() )
    {
    case _80: m_fRegion = 0.1f; break;
    case _60: m_fRegion = 0.2f; break;
    case _40: m_fRegion = 0.3f; break;
    case _20: m_fRegion = 0.4f; break;
    }
    m_pPreviewDlg->EnableAccuracy( TRUE );
}

void DistanceAccuracyDlg::OnEnChangeEdGroundtruth()
{
    m_iGroundTruth = GetDlgItemInt( IDC_ED_GROUNDTRUTH );
}

void DistanceAccuracyDlg::OnCbnSelchangeCbDepthlist()
{
    m_pPreviewDlg->DepthIndexAccuracy( ( ( CComboBox* )GetDlgItem( IDC_CB_DEPTHLIST ) )->GetCurSel() );
}

LRESULT DistanceAccuracyDlg::OnUpdateUI( WPARAM wp, LPARAM lp )
{
    SetDlgItemText( IDC_ED_DISTANCE,       m_csDistance      );
    SetDlgItemText( IDC_ED_FILLRATE,       m_csFillRate      );
    SetDlgItemText( IDC_ED_ZACCURACY,      m_csZAccuracy     );
    SetDlgItemText( IDC_ED_TEMPORAL_NOISE, m_csTrmporalNoise );
    SetDlgItemText( IDC_ED_SPATIAL_NOISE,  m_csSpatialNoise  );
    SetDlgItemText( IDC_ED_ANGLE,          m_csAngle         );
    SetDlgItemText( IDC_ED_ANGLE_X,        m_csAngleX        );
    SetDlgItemText( IDC_ED_ANGLE_Y,        m_csAngleY        );

	SetDlgItemText( IDC_ED_PIXEL_UNIT , m_csPixelUnit);
	SetDlgItemText( IDC_ED_LEFT_FX , m_csLeftFx);
	SetDlgItemText( IDC_ED_LEFT_FY, m_csLeftFy);
	SetDlgItemText( IDC_ED_RIGHT_FX, m_csRightFx);
	SetDlgItemText( IDC_ED_RIGHT_FY, m_csRightFy);

    return NULL;
}

void DistanceAccuracyDlg::Reset( const BOOL bRunAccuracy )
{
    m_bRunAccuracy = bRunAccuracy;

    m_csDistance.Empty();
    m_csFillRate.Empty();
    m_csZAccuracy.Empty();
    m_csTrmporalNoise.Empty();
    m_csSpatialNoise.Empty();
    m_csAngle.Empty();
    m_csAngleX.Empty();
    m_csAngleY.Empty();

    PostMessage( WM_UPDATE_UI );
	std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
}

const CRect& DistanceAccuracyDlg::GetAccuracyRegion()
{
    static CRect rtEmpty( 0, 0, 0, 0 );

    return m_bRunAccuracy ? m_rtRegion : rtEmpty;
}

void DistanceAccuracyDlg::EnableDepthList( const BOOL bEnable )
{
    GetDlgItem( IDC_CB_DEPTHLIST )->ShowWindow( bEnable ? SW_SHOW : SW_HIDE );
}

void DistanceAccuracyDlg::UpdateDepthMap( const int Width, const int Height, CDepthDlg* pDepthDlg )
{
    m_Width     = Width;
    m_Height    = Height;
    m_pDepthDlg = pDepthDlg;

    int iDepthLeftInterval = NULL;

    if ( m_iGroundTruth && !isinf(pDepthDlg->GetBaseline()) && !isinf(pDepthDlg->GetFocalLength()))
    {
        iDepthLeftInterval = pDepthDlg->GetBaseline() * pDepthDlg->GetFocalLength() / m_iGroundTruth;
		iDepthLeftInterval = std::min(iDepthLeftInterval, (int)m_Width);
    }
    m_rtRegion.left   = int( m_fRegion * ( m_Width - iDepthLeftInterval ) ) + iDepthLeftInterval;
    m_rtRegion.top    = int( m_fRegion * m_Height );
    m_rtRegion.right  = int( ( 1.0f - m_fRegion ) * ( m_Width - iDepthLeftInterval ) ) + iDepthLeftInterval;
    m_rtRegion.bottom = int( ( 1.0f - m_fRegion ) * m_Height );

	m_rtRegion.left = std::min((int)m_rtRegion.left, m_Width - 2);
	m_rtRegion.top = std::min((int)m_rtRegion.top, m_Height - 2);
	m_rtRegion.right = std::min((int)m_rtRegion.right, m_Width - 1);
	m_rtRegion.bottom = std::min((int)m_rtRegion.bottom, m_Height - 1);

    if ( m_bStop ) return;

    if ( !m_ThreadAccuracy )
    {
        m_ThreadAccuracy.reset( new std::thread( std::bind( &DistanceAccuracyDlg::Thread_UpdateAccuracy, this, std::placeholders::_1 ), nullptr ) );
    }
    if ( !m_ThreadSpatialNoise )
    {
        m_ThreadSpatialNoise.reset( new std::thread( std::bind( &DistanceAccuracyDlg::Thread_UpdateSpatialNoise, this, std::placeholders::_1 ), nullptr ) );
    }
    if ( !m_ThreadTemporalNoise )
    {
        m_ThreadTemporalNoise.reset( new std::thread( std::bind( &DistanceAccuracyDlg::Thread_UpdateTemporalNoise, this, std::placeholders::_1 ), nullptr ) );
    }
    if ( m_bRunAccuracy ) m_event.notify_all();
}

std::vector< WORD > DistanceAccuracyDlg::GetDepthZOfROI(int &nWidth, int &nHeight)
{
	const int nLeft = m_rtRegion.left;
	const int nTop = m_rtRegion.top;
	const int nRight = m_rtRegion.right;
	const int nBottom = m_rtRegion.bottom;

	nWidth = m_rtRegion.Width();
	nHeight = m_rtRegion.Height();

	std::vector< WORD > vecDepthZ(nWidth * nHeight, 0);
	for (int y = nTop; y < nBottom; y++)
	{
		for (int x = nLeft; x < nRight; x++)
		{
			vecDepthZ[(y - nTop) * nWidth + (x - nLeft)] = m_pDepthDlg->GetZValue(x, y);
		}
	}

	return vecDepthZ;
}

void DistanceAccuracyDlg::CalculateFittedPlane(double &a, double &b, double &d,
	std::vector< WORD > &vecDepthZ, int nWidth, int nHeight)
{
	double MatrixXX = 0.0;
	double MatrixYY = 0.0;
	double MatrixXY = 0.0;
	double MatrixX = 0.0;
	double MatrixY = 0.0;
	double MatrixN = 0.0;
	double MatrixXZ = 0.0;
	double MatrixYZ = 0.0;
	double MatrixZ = 0.0;
	double MatrixBase = 0.0;
	int    idx = 0;

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			idx = y * nWidth + x;
			if (vecDepthZ[idx])
			{
				MatrixXX += (x * x);
				MatrixYY += (y * y);
				MatrixXY += (x * y);
				MatrixX += x;
				MatrixY += y;
				MatrixN++;
				MatrixXZ += (x * vecDepthZ[idx]);
				MatrixYZ += (y * vecDepthZ[idx]);
				MatrixZ += vecDepthZ[idx];
			}
		}
	}

	MatrixBase = MatrixXX * MatrixYY * MatrixN + 2 * MatrixXY * MatrixX * MatrixY - MatrixX * MatrixX  * MatrixYY
		- MatrixY * MatrixY  * MatrixXX
		- MatrixXY * MatrixXY * MatrixN;
	a = (MatrixXZ * MatrixYY * MatrixN + MatrixZ * MatrixXY * MatrixY + MatrixYZ * MatrixX  * MatrixY -
		MatrixZ * MatrixYY * MatrixX - MatrixXZ * MatrixY  * MatrixY - MatrixYZ * MatrixXY * MatrixN) / MatrixBase;
	b = (MatrixYZ * MatrixXX * MatrixN + MatrixXZ * MatrixX  * MatrixY + MatrixZ * MatrixXY * MatrixX -
		MatrixYZ * MatrixX  * MatrixX - MatrixZ * MatrixXX * MatrixY - MatrixXZ * MatrixXY * MatrixN) / MatrixBase;
	d = (MatrixZ  * MatrixXX * MatrixYY + MatrixYZ * MatrixXY * MatrixX + MatrixXZ * MatrixXY * MatrixY -
		MatrixXZ * MatrixYY * MatrixX - MatrixYZ * MatrixXX * MatrixY - MatrixZ * MatrixXY * MatrixXY) / MatrixBase;
}

double DistanceAccuracyDlg::CalculateZAccuracy(std::vector< WORD > &vecDepthZ,
	int nWidth, int nHeight,
	double grandtrue, double fitted_GT,
	double vecBefore[3], double vecAfter[3])
{
	if (!grandtrue) return 0.0;

	auto CrossProduct = [=](double a[3], double b[3], double crossvector[3])
	{
		double Cross_vector[3] = { 1,2,3 };

		crossvector[0] = a[1] * b[2] - a[2] * b[1];
		crossvector[1] = a[2] * b[0] - a[0] * b[2];
		crossvector[2] = a[0] * b[1] - a[1] * b[0];
	};

	auto DotProduct = [=](double a[3], double b[3]) -> double
	{
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	};

	auto Length = [=](double v[3])-> double
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	};

	int ROI_PointNum = nWidth * nHeight;
	int center_x = nWidth / 2;
	int center_y = nHeight / 2;

	double u[3];
	CrossProduct(vecBefore, vecAfter, u);
	double dotresult = DotProduct(vecBefore, vecAfter);
	double n_before = Length(vecBefore);
	double n_after = Length(vecAfter);

	double rotationangle = acos(dotresult / (n_before * n_after));

	double norm = Length(u);

	double rotatinMatrix[3][3] = { 0 };

	u[0] /= norm;
	u[1] /= norm;
	u[2] /= norm;

	rotatinMatrix[0][0] = cos(rotationangle) + u[0] * u[0] * (1 - cos(rotationangle));
	rotatinMatrix[0][1] = u[0] * u[1] * (1 - cos(rotationangle) - u[2] * sin(rotationangle));
	rotatinMatrix[0][2] = u[1] * sin(rotationangle) + u[0] * u[2] * (1 - cos(rotationangle));

	rotatinMatrix[1][0] = u[2] * sin(rotationangle) + u[0] * u[1] * (1 - cos(rotationangle));
	rotatinMatrix[1][1] = cos(rotationangle) + u[1] * u[1] * (1 - cos(rotationangle));
	rotatinMatrix[1][2] = -u[0] * sin(rotationangle) + u[1] * u[2] * (1 - cos(rotationangle));

	rotatinMatrix[2][0] = -u[1] * sin(rotationangle) + u[0] * u[2] * (1 - cos(rotationangle));
	rotatinMatrix[2][1] = u[0] * sin(rotationangle) + u[1] * u[2] * (1 - cos(rotationangle));
	rotatinMatrix[2][2] = cos(rotationangle) + u[2] * u[2] * (1 - cos(rotationangle));

	int index = center_y * nWidth + center_x;
	double trans_z = rotatinMatrix[2][0] * center_x + rotatinMatrix[2][1] * center_y + rotatinMatrix[2][2] * vecDepthZ[index] - fitted_GT;

	std::vector<float> vecZError(nWidth * nHeight, 0.0f);

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			int index = y * nWidth + x;
			double z = vecDepthZ[index];
			if (z)
			{
#if 1 // rotation flag
				vecZError[index] = rotatinMatrix[2][0] * x + rotatinMatrix[2][1] * y + rotatinMatrix[2][2] * z - grandtrue - trans_z;
#else
				vecZError[index] = z - grandtrue;
#endif
			}
		}
	}

	std::sort(vecZError.begin(), vecZError.end());

	double mid_erro = vecZError[ROI_PointNum / 2];

	return mid_erro / grandtrue;
}

void DistanceAccuracyDlg::SortZ(std::vector< WORD > &vecDepthZ, double dblDeleteBoundaryRatio)
{
	std::vector< WORD > vecDepthZ_bak = vecDepthZ;
	std::sort(vecDepthZ_bak.begin(), vecDepthZ_bak.end());

	auto iterFirstNotZero = std::find_if(vecDepthZ_bak.begin(), vecDepthZ_bak.end(), [](float val) { return val != 0; });

	if (iterFirstNotZero == vecDepthZ_bak.end()) return;

	int nBoundCount = std::distance(iterFirstNotZero, vecDepthZ_bak.end()) * dblDeleteBoundaryRatio;

	if (!nBoundCount) return;

	WORD nUpperBoundVal = *(vecDepthZ_bak.end() - nBoundCount);
	WORD nLowerBoundVal = *(iterFirstNotZero + nBoundCount);

	for (WORD &depth : vecDepthZ)
	{
		if (!depth) continue;

		if (depth > nUpperBoundVal || depth < nLowerBoundVal)
		{
			depth = 0;
		}
	}
}

void DistanceAccuracyDlg::Thread_UpdateAccuracy( void* )
{
    while ( !m_bStop )
    {
        {
            std::unique_lock< std::mutex > lock( m_mutex );

            m_event.wait( lock );
        }
        if ( m_bStop ) break;

		
		int nWidth, nHeight;
		std::vector<WORD> vecDepthZ = GetDepthZOfROI(nWidth, nHeight);

		unsigned long nFillCount = 0;
		for (float depth : vecDepthZ)
		{
			if (!depth) continue;
			++nFillCount;
		}

		if (!nFillCount) continue;

		double a, b, d;
		SortZ(vecDepthZ);
		CalculateFittedPlane(a, b, d, vecDepthZ, nWidth, nHeight);

		int nCenterX = nWidth / 2;
		int nCenerY = nHeight / 2;

		double vecBefore[3] = { -a, -b, 1 };
		double vecAfter[3] = { 0, 0, 1 };

        if (nFillCount)
        {
            const double Distance = (a * nCenterX) + (b * nCenerY) + d;

            m_csDistance.Format( L"%.2fmm", Distance );

            if ( !vecDepthZ.empty() ) m_csFillRate.Format( L"%.2f%%", nFillCount * 100.0f / vecDepthZ.size());
			if (m_bEnable && m_iGroundTruth)
			{
				double ZAccuracy = CalculateZAccuracy(vecDepthZ, nWidth, nHeight, 
													  m_iGroundTruth, Distance, 
													  vecBefore, vecAfter);
				m_csZAccuracy.Format(L"%.2f%%", 100.0f * ZAccuracy);
			}
			else
			{
				m_csZAccuracy.Empty();
			}

            PostMessage( WM_UPDATE_UI );
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
        }
    }
}

void DistanceAccuracyDlg::Thread_UpdateSpatialNoise( void* )
{
    while ( !m_bStop )
    {		
		{
			std::unique_lock< std::mutex > lock(m_mutex);

			m_event.wait(lock);
		}
		if (m_bStop) break;
		
		int nROIWidth, nROIHeight;
		std::vector< WORD > vecDepthZ = GetDepthZOfROI(nROIWidth, nROIHeight);
		double a, b, d;

		SortZ(vecDepthZ);
		CalculateFittedPlane(a, b, d, vecDepthZ, nROIWidth, nROIHeight);

		double DepthZSum = 0.0;
		int    Count = 0;
		int    idx = 0;
		for (int y = 0; y < nROIHeight; y++)
		{
			for (int x = 0; x < nROIWidth; x++)
			{
				idx = y * nROIWidth + x;

				if (vecDepthZ[idx])
				{
					Count++;
					DepthZSum += pow(vecDepthZ[idx] - (a * x + b * y + d), 2);
				}
			}
		}

        m_csAngle.Format( L"%.2f deg", acos( 1.0f / sqrt( a * a + b * b + 1 ) ) * 180.0f / 3.1415926f );
        m_csAngleX.Format( L"%c%.2f deg", a < 0 ? '-' : ' ', acos( 1.0f / sqrt( a * a + 1 ) ) * 180.0f / 3.1415926f );
        m_csAngleY.Format( L"%c%.2f deg", b > 0 ? '-' : ' ', acos( 1.0f / sqrt( b * b + 1 ) ) * 180.0f / 3.1415926f );

		if (m_iGroundTruth <= 0)
		{//#6494; 移至這裡以免影響 Angle 資訊顯示...
			m_csSpatialNoise = _T("0.0%");
			PostMessage(WM_UPDATE_UI);
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
			continue;
		}
		//#6421； mm => %
        //m_csSpatialNoise.Format( L"%.2f mm", Count ? ( sqrt( DepthZSum / Count ) ) : 0.0f );
		float fValue = 0.0f;
		if (m_iGroundTruth == 0 || Count == 0)
			m_csSpatialNoise = _T("0.0%");
		else
		{
			fValue = sqrt(DepthZSum / Count);
			//#6483;	// 變更計算方式;
			fValue /= m_iGroundTruth;
			fValue *= 100;
			m_csSpatialNoise.Format(L"%.2f%%", fValue);
		}

        PostMessage( WM_UPDATE_UI );
		std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
    }
}

void DistanceAccuracyDlg::Thread_UpdateTemporalNoise( void* )
{
    const int DepthSize = m_Width * m_Height;

    std::list< std::vector< short > > lstDepth( TemporalNoiseCount );

    for ( auto& i : lstDepth ) i.resize( DepthSize, NULL );

    int DepthZSum = NULL;
    int Count = NULL;
    short AvgDepth = NULL;

    std::vector< float > vecSTD( DepthSize );

    int std_cnt = NULL;
    int idx = NULL;

    while ( !m_bStop )
    {
        {
            std::unique_lock< std::mutex > lock( m_mutex );

            m_event.wait( lock );
        }
        if ( m_bStop ) break;

		if (m_iGroundTruth <= 0)
		{
			m_csTrmporalNoise = _T("0.0%");
			PostMessage(WM_UPDATE_UI);
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
			continue;
		}

        std::vector< short >& vecDepthZ = *lstDepth.rbegin();

        memset( vecSTD.data(), NULL, DepthSize * sizeof( float ) );

        std_cnt = NULL;

        for ( int y = m_rtRegion.top; y < m_rtRegion.bottom; y++ )
        {
            for ( int x = m_rtRegion.left; x < m_rtRegion.right; x++ )
            {
                vecDepthZ[ y * m_Width + x ] = m_pDepthDlg->GetZValue( x, y );
            }
        }
        for ( int y = m_rtRegion.top; y < m_rtRegion.bottom; y++ )
        {
            for ( int x = m_rtRegion.left; x < m_rtRegion.right; x++ )
            {
                idx = y * m_Width + x;

                DepthZSum = NULL;
                Count     = NULL;

                for ( auto& vecDepth : lstDepth )
                {
                    if ( vecDepth[ idx ] ) Count++;

                    DepthZSum += vecDepth[ idx ];
                }
                if ( Count )
                {
                    AvgDepth = DepthZSum / Count;

                    DepthZSum = NULL;

                    for ( auto& vecDepth : lstDepth )
                    {
                        if ( vecDepth[ idx ] )
                        {
                            DepthZSum += ( int )pow( vecDepth[ idx ] - AvgDepth, 2 );
                        }
                    }
                    vecSTD[ std_cnt++ ] = sqrt( DepthZSum / ( float )Count );
                }
            }
        }
        if ( std_cnt )
        {
            std::sort( vecSTD.begin(), vecSTD.begin() + std_cnt );

			//#6421； mm => %
            //m_csTrmporalNoise.Format( L"%.2f mm", vecSTD[ std_cnt / 2 ] );
			float fValue = 0.0f;
			
			if (m_iGroundTruth == 0)
				m_csTrmporalNoise = _T("0.0%");
			else
			{
				fValue = vecSTD[std_cnt / 2];
				//#6483;	// 變更計算方式;
				fValue /= m_iGroundTruth;
				fValue *= 100;
				m_csTrmporalNoise.Format(L"%.2f%%", fValue);
			}

            PostMessage( WM_UPDATE_UI );
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
        }
        lstDepth.splice( lstDepth.end(), lstDepth, lstDepth.begin() );
    }
}

void DistanceAccuracyDlg::UpdatePixelUnit()
{
	int PixelUnit = 0;

	int LeftFx, LeftFy, RightFx, RightFy;

	if (APC_OK != APC_GetFlashFocalLength(m_hApcDI, &m_devSelInfo,
												  m_Width, m_Height,
												  &LeftFx, &LeftFy, &RightFx, &RightFy,
												  &PixelUnit)) 
	{
		PixelUnit = 0;
	}

	m_csPixelUnit.Format(L"%d", PixelUnit);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PIXEL_UNIT))->SetPos(-PixelUnit);

	PostMessage(WM_UPDATE_UI);
}

void DistanceAccuracyDlg::UpdateFocalLength()
{
	int LeftFx, LeftFy, RightFx, RightFy;
	if (APC_OK != APC_GetDeviceFocalLength(m_hApcDI, &m_devSelInfo,
		&LeftFx, &LeftFy, &RightFx, &RightFy) )
	{
		return;
	}

	m_csLeftFx.Format(L"%d", LeftFx);
	m_csLeftFy.Format(L"%d", LeftFy);
	m_csRightFx.Format(L"%d", RightFx);
	m_csRightFy.Format(L"%d", RightFy);

	PostMessage(WM_UPDATE_UI);
}


void DistanceAccuracyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);

	const int iValue = ((CSliderCtrl*)pScrollBar)->GetPos();

	switch (pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PIXEL_UNIT:
	{
		m_csPixelUnit.Format(L"%d", -iValue);
		PostMessage(WM_UPDATE_UI);
	}
	break;
	}
}


void DistanceAccuracyDlg::OnBnClickedButtonAdjustFocalLength()
{
	// TODO: Add your control notification handler code here
	int PixelUnit = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_PIXEL_UNIT))->GetPos() * -1;
	APC_AdjustFocalLength(m_hApcDI, &m_devSelInfo, m_Width, m_Height, PixelUnit);

	UpdatePixelUnit();
	UpdateFocalLength();
}
