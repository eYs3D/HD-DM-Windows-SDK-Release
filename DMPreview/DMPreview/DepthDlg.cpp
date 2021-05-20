// DepthDlg.cpp : πÍß@¿…
//

#include "stdafx.h"
#include "EtronDI_Test.h"
#include "DepthDlg.h"
#include "WmMsgDef.h"
#include "ColorPaletteGenerator.h"

const size_t maxFrameTimestampQueueSize = 31;

void CDepthDlg::UpdateD8DisplayImage_DIB24( const RGBQUAD* pColorPalette, const BYTE* pDepth, BYTE* pResult)
{
    if ( m_nDepthResWidth <=0 || m_nDepthResHeight <= 0 ) return;

	int nBPS = ( ( m_nDepthResWidth * 3 + 3 ) / 4 ) * 4;
	BYTE* pDL    = pResult + ( m_nDepthResHeight - 1 ) * nBPS;
    BYTE* pD     = NULL;
	const RGBQUAD* pClr = NULL;

    for ( int y = 0; y < m_nDepthResHeight; y++ )
    {
		pD = pDL;
		for ( int x = 0; x < m_nDepthResWidth; x++ )
        {
            if ( m_zdTableSize == 4096 ) pClr = &( pColorPalette[ ( ( WORD* )m_zdTable )[ pDepth[ x ] << 3 ] ] );
            else                         pClr = &( pColorPalette[ ( ( WORD* )m_zdTable )[ pDepth[ x ] ] ] );

			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pDepth += m_nDepthResWidth;
		pDL -= nBPS;
	}
}

void CDepthDlg::UpdateD11DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult)
{
    if ( m_nDepthResWidth <=0 || m_nDepthResHeight <= 0 ) return;

	int nBPS = ( ( m_nDepthResWidth * 3 + 3 ) / 4 ) * 4;
	BYTE* pDL    = pResult + ( m_nDepthResHeight - 1 ) * nBPS;
    BYTE* pD     = NULL;
	const RGBQUAD* pClr = NULL;

	for ( int y = 0; y < m_nDepthResHeight; y++ )
    {
		pD = pDL;
		for ( int x = 0; x < m_nDepthResWidth; x++ )
        {
			pClr = &( pColorPalette[ ( ( WORD* )m_zdTable )[ pDepth[ x ] ] ] );
			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pDepth += m_nDepthResWidth;
		pDL -= nBPS;
	}
}

void CDepthDlg::UpdateD11_Fusion_DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepthFs, const WORD* pDepth, BYTE* pResult)
{
    if ( m_nDepthResWidth <=0 || m_nDepthResHeight <= 0 ) return;

	int nBPS = ( ( m_nDepthResWidth * 3 + 3 ) / 4 ) * 4;
	BYTE* pDL    = pResult + ( m_nDepthResHeight - 1 ) * nBPS;
    BYTE* pD     = NULL;
	const RGBQUAD* pClr = NULL;
    static const RGBQUAD empty = { NULL };
    WORD wDepth = NULL;

	for ( int y = 0; y < m_nDepthResHeight; y++ )
    {
		pD = pDL;
		for ( int x = 0; x < m_nDepthResWidth; x++ )
        {
			wDepth = ( pDepth[ x ] ? ( WORD )( 8.0 * m_camFocus * m_baselineDist / pDepth[ x ] ) : NULL );

            if ( wDepth > m_Far || wDepth < m_Near )
			    pClr = &empty;
            else pClr = &(pColorPalette[pDepthFs[x]]);

			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
        pDepth += m_nDepthResWidth;
		pDepthFs += m_nDepthResWidth;
		pDL -= nBPS;
	}
}

void CDepthDlg::UpdateD11_Baseline_DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult)
{
    if ( m_nDepthResWidth <=0 || m_nDepthResHeight <= 0 ) return;

	int nBPS = ( ( m_nDepthResWidth * 3 + 3 ) / 4 ) * 4;
	BYTE* pDL    = pResult + ( m_nDepthResHeight - 1 ) * nBPS;
    BYTE* pD     = NULL;
    const RGBQUAD* pClr = NULL;
    static const RGBQUAD empty = { NULL };
    WORD wDepth = NULL;

	for ( int y = 0; y < m_nDepthResHeight; y++ )
    {
		pD = pDL;
		for ( int x = 0; x < m_nDepthResWidth; x++ )
        {
            wDepth = ( pDepth[ x ] ? ( WORD )( 8.0 * m_camFocus * m_baselineDist / pDepth[ x ] ) : NULL );

            if ( wDepth > m_Far || wDepth < m_Near )
			    pClr = &empty;
            else pClr = &(pColorPalette[pDepth[x]]);

			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pDepth += m_nDepthResWidth;
		pDL -= nBPS;
	}
}

void CDepthDlg::UpdateZ14DisplayImage_DIB24( const RGBQUAD* pColorPalette, const WORD* pDepth, BYTE* pResult)
{
    if ( m_nDepthResWidth <=0 || m_nDepthResHeight <= 0 ) return;

	int x,y,nBPS = ( ( m_nDepthResWidth * 3 + 3 ) / 4 ) * 4;
	BYTE* pDL    = pResult + ( m_nDepthResHeight - 1 ) * nBPS;
    BYTE* pD     = NULL;
	const RGBQUAD* pClr = NULL;

    for ( y = 0; y < m_nDepthResHeight; y++ )
    {
		pD = pDL;
		for ( x = 0; x < m_nDepthResWidth; x++ )
        {
			pClr = &( pColorPalette[ pDepth[ x ] ] );
			pD[0] = pClr->rgbBlue; //B
			pD[1] = pClr->rgbGreen; //G
			pD[2] = pClr->rgbRed; //R
			pD += 3;
		}
		pDepth += m_nDepthResWidth;
		pDL -= nBPS;
	}
}

void generatePaletteColor(RGBQUAD* palette, int size, int mode, int customROI1, int customROI2, bool reverseRedToBlue) {
	float ROI2 = 1.0f;
	float ROI1 = 0.0f;

	//The value ranges from 0.0f ~ 1.0f as hue angle
	float ROI2Value = 1.0f;
	float ROI1Value = 0.0f;

	//BYTE* buf = (BYTE*)malloc(sizeof(BYTE) * 4 * size);
	//BYTE buf[(size) * 4];
	//Set ROI by mode setting.The bigger the disparity the nearer the distance
	switch (mode) {
	case 1: //near
		ROI2 = 0.8f;
		ROI1 = 0.5f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 2: //midle
		ROI2 = 0.7f;
		ROI1 = 0.3f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 3: //far
		ROI2 = 0.6f;
		ROI1 = 0.2f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 4: //custom
		ROI2 = 1.0f*customROI2 / size;
		ROI1 = 1.0f*customROI1 / size;
		ROI2Value = 1.0f;
		ROI1Value = 0.0f;
		break;
	default: //normal 
		ROI2 = 1.0f;
		ROI1 = 0.0f;
		ROI2Value = 1.0f;
		ROI1Value = 0.0f;
		break;
	}
	ColorPaletteGenerator::generatePalette((BYTE*)palette, size, ROI1 * size, ROI1Value, ROI2 * size, ROI2Value, reverseRedToBlue);

	//for ( int i = 0; i < size; i++ )
 //   {
 //       memcpy( &palette[ i ], &buf[ i * 4 ], sizeof( RGBQUAD ) );
	//}
 //   free( buf );
}


void generatePaletteGray(RGBQUAD* palette, int size, int mode, int customROI1, int customROI2, bool reverseGraylevel){
	float ROI1 = 0.0f;
	float ROI2 = 1.0f;

	//The value ranges from 0.0f ~ 1.0f as hue angle
	float ROI1Value = 0.0f;
	float ROI2Value = 1.0f;
	

	//BYTE* buf = (BYTE*)malloc(sizeof(BYTE) * 4 * size);
	//BYTE buf[(size) * 4];
	//Set ROI by mode setting.The bigger the disparity the nearer the distance
	switch (mode) {
	case 1: //near
		ROI2 = 0.8f;
		ROI1 = 0.5f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 2: //midle
		ROI2 = 0.7f;
		ROI1 = 0.3f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 3: //far
		ROI2 = 0.6f;
		ROI1 = 0.2f;
		ROI2Value = 0.9f;
		ROI1Value = 0.1f;
		break;
	case 4: //custom
		ROI2 = 1.0f*customROI2 / size;
		ROI1 = 1.0f*customROI1 / size;
		ROI2Value = 1.0f;
		ROI1Value = 0.0f;
		break;
	default: //normal 
		ROI2 = 1.0f;
		ROI1 = 0.0f;
		ROI2Value = 1.0f;
		ROI1Value = 0.0f;
		break;
	}

	ColorPaletteGenerator::generatePaletteGray((BYTE*)palette, size, ROI1 * size, ROI1Value, ROI2 * size, ROI2Value, reverseGraylevel);
	//for (int i = 0; i < size; i++) {
	//	palette[i].rgbBlue = buf[i * 4 + 2];
	//	palette[i].rgbGreen = buf[i * 4 + 1];
	//	palette[i].rgbRed = buf[i * 4 + 0];
	//	palette[i].rgbReserved = buf[i * 4 + 3];
	//}
    //free( buf );
}

//void DmColorMode11(RGBQUAD pallete[2048], int mode)
//{
//	double R, G, B;
//	for (int i = 0; i<2048; i++) {
//		HSV_to_RGB((double)(i >> 3), 1.0, 1.0, R, G, B);
//
//		pallete[i].rgbBlue = (BYTE)B;
//		pallete[i].rgbGreen = (BYTE)G;
//		pallete[i].rgbRed = (BYTE)R;
//		pallete[i].rgbReserved = 0;
//	}
//}
//void DmColorMode14(RGBQUAD *pColorPaletteZ14, int mode)
//{
//	int i;
//	double R, G, B;
//	double fx, fy;
//	//
//	double fCV = 180;
//	int nCenter = 1500;
//	double r1 = 0.35;
//	double r2 = 0.55;
//	//
//	for (i = 1; i<16384; i++) {
//		if (i == nCenter) {
//			fy = fCV;
//		}
//		else if (i<nCenter) {
//			fx = (double)(nCenter - i) / nCenter;
//			fy = fCV - pow(fx, r1)*fCV;
//		}
//		else {
//			fx = (double)(i - nCenter) / (16384 - nCenter);
//			fy = fCV + pow(fx, r2)*(256 - fCV);
//		}
//		HSV_to_RGB(fy, 1.0, 1.0, R, G, B);
//		pColorPaletteZ14[i].rgbBlue = (BYTE)B;
//		pColorPaletteZ14[i].rgbGreen = (BYTE)G;
//		pColorPaletteZ14[i].rgbRed = (BYTE)R;
//		pColorPaletteZ14[i].rgbReserved = 0;
//	}
//	{
//		i = 0;
//		pColorPaletteZ14[i].rgbBlue = (BYTE)0;
//		pColorPaletteZ14[i].rgbGreen = (BYTE)0;
//		pColorPaletteZ14[i].rgbRed = (BYTE)0;
//		pColorPaletteZ14[i].rgbReserved = 0;
//	}
//	{
//		i = 16383;
//		pColorPaletteZ14[i].rgbBlue = (BYTE)255;
//		pColorPaletteZ14[i].rgbGreen = (BYTE)255;
//		pColorPaletteZ14[i].rgbRed = (BYTE)255;
//		pColorPaletteZ14[i].rgbReserved = 0;
//	}
//}

CDepthDlg::CDepthDlg(CWnd* pParent /*=NULL*/)
	: CResizableDlg(CDepthDlg::IDD, pParent)
{
  
  m_pImageBuf = NULL;
  m_mouseTracking = false;
  m_curDepthPos.x = m_curDepthPos.y = MAXWORD;
  
  // for paint thread +
  m_PaintThread = NULL;
  m_bThreadContinue = TRUE;
  // for paint thread -
  
  m_nDepthResWidth  = 0;
  m_nDepthResHeight = 0; 
  
  m_DepthMapType = TRANSFER_TO_COLORFULRGB;
  m_DevType = PUMA;
  m_depthImageType = EtronDIImageType::DEPTH_11BITS;

  m_depthId = -1;
  m_depthSerialNumber = -1;
  m_zdTable = NULL;
  m_zdTableSize = NULL;
  m_zdTableIndex = 0;
  m_camFocus = 0.0;
  m_baselineDist = 0.0;
  m_iDepthROI = 20;
  m_iUpdateZ_Count = 0;
  m_iUpdateZ_period = 0;
  m_fBaseline = 0;
  m_fFocalLength = 0;

  m_rtAccuracyRegion.SetRectEmpty();

  QueryPerformanceFrequency( &m_liPerfFreq );

  memset( &m_bmiDepth, NULL, sizeof( m_bmiDepth ) );
}

CDepthDlg::~CDepthDlg() 
{
    if ( m_zdTable ) delete[] m_zdTable;
    if( m_pImageBuf ) delete[] m_pImageBuf;

    m_MemDC.DeleteDC(); 
}

void CDepthDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDlg::DoDataExchange(pDX);
}


LRESULT CDepthDlg::OnUpdateDlgTitle(WPARAM wParam, LPARAM lParam)
{
    SetWindowText( m_csDialogTitle );
    return 0;
}

IMPLEMENT_DYNAMIC(CDepthDlg, CResizableDlg)
BEGIN_MESSAGE_MAP(CDepthDlg, CResizableDlg)
	ON_WM_CLOSE()
	ON_WM_PAINT()
    ON_MESSAGE(WM_MSG_UPDATE_DLG_TITLE, &CDepthDlg::OnUpdateDlgTitle)
    ON_WM_SYSCOMMAND()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CDepthDlg::UpdateColorPalette( DEPTHMAP_TYPE depthMapType )
{
    m_DepthMapType = depthMapType;
}

void CDepthDlg::UpdateColorPalette( int zFar, int zNear )
{
    GetDepthZValue( zFar, zNear );

    m_Near = zNear;
    m_Far  = zFar;

    memset( m_ColorPalette, NULL, sizeof( m_ColorPalette ) );
    memset( m_GrayPalette,  NULL, sizeof( m_GrayPalette  ) );

    if ( m_mblDistMultiplier.size() || ( m_camFocus > 0.0 && m_baselineDist > 0.0 ) ) 
    {
        generatePaletteColor( m_ColorPalette, 1 << 11, 4, 130, 2047, false );
        generatePaletteGray ( m_GrayPalette,  1 << 11, 4, 130, 2047, true  );
    }
    else
    {
        generatePaletteColor( m_ColorPalette, 1 << 14, 4, zNear, zFar, true  );
        generatePaletteGray ( m_GrayPalette,  1 << 14, 4, zNear, zFar, false );
    }
}

void CDepthDlg::GetDepthZValue( int& zFar, int& zNear )
{
    WORD wNear       = MAXINT;
    WORD wFar        = NULL;
    WORD* zdTable    = ( WORD* )m_zdTable;
    WORD  wTableSize = m_zdTableSize / 2;

    if ( m_zdTable )
    {
        for ( int i = 0; i < wTableSize; i++ ) if ( zdTable[ i ] && zdTable[ i ] > wFar  ) { wFar  = zdTable[ i ]; }
        for ( int i = wTableSize - 1; i > -1; i-- ) if ( zdTable[ i ] && zdTable[ i ] < wNear ) { wNear = zdTable[ i ]; }

        if ( zNear < wNear ) zNear = wNear;
        if ( zFar  > wFar  ) zFar  = wFar;
    }
}

void CDepthDlg::SetImageParams(void* hEtronDI, DEVSELINFO devSelInfo, unsigned short devType,
    EtronDIImageType::Value depthImageType,
	int depthId, int zdTableIndex, int depthWidth, int depthHeight, int iUpdateZ_period, 
	float camFocus, float baselineDist, std::vector<float> multiBaselineDist )
{
    m_hEtronDI = hEtronDI;
    m_DevSelInfo.index = devSelInfo.index;
    m_DevType = devType;
    m_depthImageType = depthImageType;
    m_depthId = depthId;
    m_zdTableIndex = zdTableIndex;
    m_nDepthResWidth = depthWidth;
    m_nDepthResHeight = depthHeight;
    m_camFocus = camFocus;
    m_baselineDist = baselineDist;
    m_iUpdateZ_period = iUpdateZ_period;

    if (!multiBaselineDist.empty())
    {
        std::vector<float> multiplier;
        for (auto dist : multiBaselineDist)
        {
            if (dist == m_baselineDist)
            {
                multiplier.push_back(1.0);
            }
            else
            {
                multiplier.push_back(dist / m_baselineDist);
            }
        }
        m_mblDistMultiplier = multiplier;
    }
    else
    {
        m_mblDistMultiplier.clear();
    }
    m_bmiDepth.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bmiDepth.bmiHeader.biWidth = m_nDepthResWidth;
    m_bmiDepth.bmiHeader.biHeight = m_nDepthResHeight;
    m_bmiDepth.bmiHeader.biBitCount = 24;
    m_bmiDepth.bmiHeader.biPlanes = 1;
    m_bmiDepth.bmiHeader.biSizeImage = m_nDepthResWidth * m_nDepthResHeight * ( EtronDIImageType::Value::DEPTH_8BITS == depthImageType ? 1 : 3 );
}

void CDepthDlg::SetHandle( void* hEtronDI, const DEVSELINFO& devSelInfo )
{
    m_hEtronDI = hEtronDI;
    m_DevSelInfo.index = devSelInfo.index;
}

BOOL CDepthDlg::OnInitDialog() {

  CResizableDlg::OnInitDialog();

  m_MemDC.CreateCompatibleDC(NULL);
  m_MemDC.SetTextColor( RGB( 255, 255, 0 ) );
  m_MemDC.SetBkMode( TRANSPARENT );
  m_MemDC.SelectStockObject( NULL_BRUSH );
  m_MemDC.SetStretchBltMode(STRETCH_DELETESCANS);
  m_MemDC.SelectObject( GetFont() );

  SetWindowText(L"DepthDlg");

  int depthDataSizeOriginal = m_nDepthResWidth * m_nDepthResHeight * (m_depthImageType == EtronDIImageType::DEPTH_8BITS ? 1 : 2);
  int depthDataSize = m_nDepthResWidth * m_nDepthResHeight * (m_depthImageType == EtronDIImageType::DEPTH_8BITS ? 1 : 2);

  m_depthData.resize(depthDataSize, 0);
  m_pImageBuf = new BYTE[ m_nDepthResWidth * m_nDepthResHeight * 3 ];

  if (m_camFocus == 0.0 && m_baselineDist == 0.0)
  {
      GetZDTable();
  }

  SetResizableInfo(m_nDepthResWidth, m_nDepthResHeight, 320);

  m_PaintThread = AfxBeginThread( PaintDepthThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
  
  return TRUE;
}

void CDepthDlg::GetZDTable()
{
#ifndef ESPDI_EG
    int actualLength = 0;

    ZDTABLEINFO zdTableInfo;
    {
        zdTableInfo.nDataType = ETronDI_DEPTH_DATA_11_BITS;
        zdTableInfo.nIndex = m_zdTableIndex;
    }
    do
    {
        if ( ETronDI_OK == EtronDI_GetZDTable(m_hEtronDI, &m_DevSelInfo, NULL, NULL, &m_zdTableSize, &zdTableInfo ) )
        {
            if ( m_zdTable ) delete[] m_zdTable;

            m_zdTable = new BYTE[ m_zdTableSize ];
            memset( m_zdTable, NULL, m_zdTableSize );

            if ( ETronDI_OK == EtronDI_GetZDTable( m_hEtronDI, &m_DevSelInfo, m_zdTable, m_zdTableSize, &actualLength, &zdTableInfo ) )
            {
                for ( int i = 0; i < actualLength; i += 2 )
                {
                    m_zdTable[ i     ] ^= m_zdTable[ i + 1 ];
                    m_zdTable[ i + 1 ] ^= m_zdTable[ i     ];
                    m_zdTable[ i     ] ^= m_zdTable[ i + 1 ];
                }
                m_zdTable[ 0 ] = NULL;
                m_zdTable[ 1 ] = NULL;

                break;
            }
        }
        MessageBox(L"Get ZD table failed!");
        delete[] m_zdTable;
        m_zdTable = NULL;
    }
    while( FALSE );
#endif
}

WORD CDepthDlg::GetDepthData(int x, int y)
{
    if (x < 0 || y < 0)
    {
        return 0;
    }
    const int pixelIndex = y * m_nDepthResWidth + x;
    switch (m_depthImageType)
    {
    case EtronDIImageType::DEPTH_8BITS_0x80:
        return (WORD)m_depthData[pixelIndex * sizeof(WORD)];
    case EtronDIImageType::DEPTH_8BITS:
        return (WORD)m_depthData[pixelIndex];
    case EtronDIImageType::DEPTH_11BITS:
    case EtronDIImageType::DEPTH_14BITS:
        return *(WORD*)(&m_depthData[pixelIndex * sizeof(WORD)]);
    default:
        return 0;
    }
}

WORD CDepthDlg::GetZValue(int x, int y)
{
    WORD depthData = GetDepthData(x, y);

    if (m_camFocus > 0.0 && m_baselineDist > 0.0)
    {
        switch (m_depthImageType)
        {
        case EtronDIImageType::DEPTH_8BITS_0x80:
        case EtronDIImageType::DEPTH_8BITS:
            return ( WORD )( m_camFocus * m_baselineDist / depthData );
        case EtronDIImageType::DEPTH_11BITS:
            // need to transfer range of depth data from [0..2047] to [0.0..256.0]
            return ( WORD )( 8.0 * m_camFocus * m_baselineDist / depthData );
        case EtronDIImageType::DEPTH_14BITS:
            return depthData;
        }
    }
    else
    {
        switch ( m_depthImageType )
        {
        case EtronDIImageType::DEPTH_11BITS:
        case EtronDIImageType::DEPTH_8BITS_0x80:
        case EtronDIImageType::DEPTH_8BITS:
            {
                WORD zdIndex = ( EtronDIImageType::DEPTH_8BITS == m_depthImageType ? ( depthData << 3 ) : depthData );

                if ( m_DevType != PUMA )
                {
                    zdIndex = depthData;
                }
                if (zdIndex > m_zdTableSize / 2 )
                {
                    zdIndex = m_zdTableSize;
                }
                if ( m_zdTable ) return ( ( WORD* )m_zdTable )[ zdIndex ];
            }
            break;
        case EtronDIImageType::DEPTH_14BITS: break;
        }
        return depthData;
    }
    return 0;
}

WORD CDepthDlg::GetZValue(int x, int y, BYTE* pDepth)
{
    if (x < 0 || y < 0)
    {
        return 0;
    }
    const int pixelIndex = y * m_nDepthResWidth + x;

    auto GetDepth = [ = ] ( WORD depthData )->WORD
    {
        WORD Disparity = ( EtronDIImageType::DEPTH_8BITS == m_depthImageType ? ( depthData << 3 ) : depthData );

        if ( m_DevType != PUMA )
        {
            Disparity = depthData;
        }
        if (Disparity > m_zdTableSize / 2 )
        {
            Disparity = m_zdTableSize;
        }
        if ( m_zdTable ) return ( ( WORD* )m_zdTable )[ Disparity ];

        return 0;
    };
    switch (m_depthImageType)
    {
    case EtronDIImageType::DEPTH_8BITS:
        {
            WORD depthData = (WORD)pDepth[pixelIndex];

            if (m_camFocus > 0.0 && m_baselineDist > 0.0)
            {
                return ( WORD )( m_camFocus * m_baselineDist / depthData );
            }
            return GetDepth( depthData );
        }
        break;
    case EtronDIImageType::DEPTH_11BITS:
        {
            WORD depthData = *(WORD*)(&pDepth[pixelIndex * sizeof(WORD)]);

            if (m_camFocus > 0.0 && m_baselineDist > 0.0)
            {
                return ( WORD )( 8.0 * m_camFocus * m_baselineDist / depthData );
            }
            return GetDepth( depthData );
        }
        break;
    case EtronDIImageType::DEPTH_14BITS:
        return *(WORD*)(&pDepth[pixelIndex * sizeof(WORD)]);
    }
    return NULL;
}

bool CDepthDlg::GetDepthData(std::vector<unsigned char>& depthBuf, EtronDIImageType::Value& depthImageType
    , int& width, int& height, int& serialNumber)
{
    {
        std::lock_guard<std::mutex> lock(m_depthDataMutex);
        
        depthBuf = m_depthData;
        serialNumber = m_depthSerialNumber;
    }

    depthImageType = m_depthImageType;
    width = m_bmiDepth.bmiHeader.biWidth;
    height = m_bmiDepth.bmiHeader.biHeight;

    return (!depthBuf.empty() && width > 0 && height > 0);
}

void CDepthDlg::UpdateFusionSelectedIndex(const std::vector<unsigned char>& selectedIndex)
{
    m_fusionSelectedIndex = selectedIndex;
}

void CDepthDlg::EnableRotate( const BOOL bRotate )
{
    m_bRotate = bRotate;

    m_bmiDepth.bmiHeader.biWidth  = m_bRotate ? m_nDepthResHeight : m_nDepthResWidth;
    m_bmiDepth.bmiHeader.biHeight = m_bRotate ? m_nDepthResWidth  : m_nDepthResHeight;
}

void CDepthDlg::ApplyImage( BYTE **pDepthBuf, const int dataSize, const int nDepthSerialNum, const CRect& rtAccuracyRegion, const DepthfilterParam& DfParam )
{
    {
        std::lock_guard<std::mutex> lock(m_depthDataMutex);
        DepthFilter( *pDepthBuf, DfParam );
        memcpy( m_depthData.data(), *pDepthBuf, m_nDepthResWidth * m_nDepthResHeight * ( m_depthImageType == EtronDIImageType::DEPTH_8BITS ? 1 : 2 ) );

        m_depthSerialNumber = nDepthSerialNum;
        m_rtAccuracyRegion  = rtAccuracyRegion;
    }
 
  //if(m_depthImageType == EtronDIImageType::DEPTH_8BITS || 
  //   m_depthImageType == EtronDIImageType::DEPTH_8BITS_0x80)
  //{
		//m_bmiDepth.bmiHeader.biBitCount = 8;
		//m_bmiDepth.bmiHeader.biClrUsed = 256;
		//m_bmiDepth.bmiHeader.biClrImportant = 256;

		//if(m_DepthMapType == TRANSFER_TO_COLORFULRGB)
		//	memcpy( m_bmiDepth.bmiColors, m_ColorPaletteD8, sizeof( m_ColorPaletteD8 ) ); 
	 //   else memcpy( m_bmiDepth.bmiColors, m_GrayPaletteD8, sizeof( m_GrayPaletteD8 ) );
  //}
  //else {
		//m_bmiDepth.bmiHeader.biBitCount = 24;
  //}

  //
  // Source Depth Image: DIB is up-side-down
  //
  //int x, y;
  //BYTE *p0,*p1,*p2;
  switch(m_depthImageType)
  {
  case EtronDIImageType::DEPTH_8BITS:
      //p0 = *pDepthBuf+(m_nDepthResHeight-1)*m_nDepthResWidth;
      //p1 = m_pImageBuf;
      //for (y=0; y<m_nDepthResHeight; y++) {
      //    memcpy(p1, p0, m_nDepthResWidth);
      //    p0 -= m_nDepthResWidth;
      //    p1 += m_nDepthResWidth;
      //}
      {
          if ( m_zdTable ) UpdateD8DisplayImage_DIB24( m_DepthMapType == TRANSFER_TO_COLORFULRGB ? m_ColorPalette : m_GrayPalette, m_depthData.data(), m_pImageBuf);
      }
      break;
  //case EtronDIImageType::DEPTH_8BITS_0x80:
  //    p0 = *pDepthBuf+(m_nDepthResHeight-1)*m_nDepthResWidth*2;
  //    p1 = m_pImageBuf;
  //    for (y=0; y<m_nDepthResHeight; y++) {
  //        p2 = p0;
  //        for (x = 0; x < m_nDepthResWidth * 2; x += 2){
  //            *p1 = *p2;
  //            p2 += 2;
  //            p1++;
  //        }
  //        p0 -= (m_nDepthResWidth * 2);
  //    }
  //    break;
  case EtronDIImageType::DEPTH_11BITS:
      {
          unsigned char* targetBuf = m_depthData.data();
          if (!m_mblDistMultiplier.empty())
          {
              if (m_tempBuf.size() != m_nDepthResWidth * m_nDepthResHeight * 2)
              {
                  m_tempBuf.resize(m_nDepthResWidth * m_nDepthResHeight * 2, 0);
              }
              memcpy(&m_tempBuf[0], m_depthData.data(), m_tempBuf.size());

              targetBuf = &m_tempBuf[0];
              float* pMultiplier = &m_mblDistMultiplier[0];
		      if (!m_fusionSelectedIndex.empty())
		      {
			      unsigned char* pfusionSelectedIndex = &m_fusionSelectedIndex[0];
			      for (int i = 0, size = m_nDepthResWidth * m_nDepthResHeight; i < size; ++i)
			      {
				      if (pfusionSelectedIndex[i] != 255)
				      {
					      ((unsigned short*)targetBuf)[i] = ( ( unsigned short* )targetBuf )[ i ] * ( unsigned short )pMultiplier[ pfusionSelectedIndex [ i ] ];
				      }
			      }
		      }
          }
          if ( m_mblDistMultiplier.size() ) UpdateD11_Fusion_DisplayImage_DIB24( m_DepthMapType == TRANSFER_TO_COLORFULRGB ? m_ColorPalette : m_GrayPalette, ( WORD* )targetBuf, ( WORD* )m_depthData.data(), m_pImageBuf);
          else if ( m_camFocus > 0.0 && m_baselineDist > 0.0 ) UpdateD11_Baseline_DisplayImage_DIB24( m_DepthMapType == TRANSFER_TO_COLORFULRGB ? m_ColorPalette : m_GrayPalette, ( WORD* )targetBuf, m_pImageBuf);
          else if ( m_zdTable ) UpdateD11DisplayImage_DIB24( m_DepthMapType == TRANSFER_TO_COLORFULRGB ? m_ColorPalette : m_GrayPalette, ( WORD* )targetBuf, m_pImageBuf);
      }
      break;
  case EtronDIImageType::DEPTH_14BITS:
      {
          UpdateZ14DisplayImage_DIB24( m_DepthMapType == TRANSFER_TO_COLORFULRGB ? m_ColorPalette : m_GrayPalette, ( WORD* )m_depthData.data(), m_pImageBuf );
      }
      break;
  }
    m_PaintEvent.SetEvent(); 
}

void CDepthDlg::GenerateZDValueTitleText()
{
    if ( ++m_iUpdateZ_Count % m_iUpdateZ_period )
    {
        return;
    }
    m_iUpdateZ_Count = NULL;

    CPoint DepthPos = m_curDepthPos;

    if ( MAXWORD == DepthPos.x || MAXWORD == DepthPos.y )
    {
        m_cs_ZD_Text = "[Z] -  [D] -  ";

        return;
    }
    CRect clientRect;
    GetClientRect(&clientRect);

    DWORD dwDepth = 0;

    if ( m_iDepthROI > 1 ) // do depthROI
    {
        int iROI_X = DepthPos.x - m_iDepthROI / 2;  if ( iROI_X < 0 ) iROI_X = 0;
        int iROI_Y = DepthPos.y - m_iDepthROI / 2;  if ( iROI_Y < 0 ) iROI_Y = 0;
        
        const int iROI_W = ( iROI_X + m_iDepthROI > m_bmiDepth.bmiHeader.biWidth  ) ? m_bmiDepth.bmiHeader.biWidth  - iROI_X : iROI_X + m_iDepthROI;
        const int iROI_H = ( iROI_Y + m_iDepthROI > m_bmiDepth.bmiHeader.biHeight ) ? m_bmiDepth.bmiHeader.biHeight - iROI_Y : iROI_Y + m_iDepthROI;

        DWORD dwDepthRoiSum   = NULL;
        DWORD dwDepthRoiCount = NULL;

        for ( int y2 = iROI_Y; y2 < iROI_H; y2++ )
        {
            for ( int x2 = iROI_X; x2 < iROI_W; x2++ )
            {
                dwDepth = GetZValue( x2, y2 );

                if ( dwDepth )
                {
                    dwDepthRoiSum += dwDepth;
                    dwDepthRoiCount++;
                }
            }
        }
        if ( dwDepthRoiCount ) dwDepth = dwDepthRoiSum / dwDepthRoiCount;
    }
    else dwDepth = GetZValue( DepthPos.x, DepthPos.y );

    m_cs_ZD_Text.Format( L"[Z] %d mm [D] %d", dwDepth, GetDepthData( DepthPos.x, DepthPos.y ) );
}

float CDepthDlg::UpdateAndGetFramerate()
{
    if (m_recentFrameTimestamp.size() >= maxFrameTimestampQueueSize)
    {
        m_recentFrameTimestamp.pop_front();
    }
    QueryPerformanceCounter( &m_liPerfTime );
    m_recentFrameTimestamp.push_back( m_liPerfTime.QuadPart );

    if (m_recentFrameTimestamp.size() > 1)
    {
        return ( float )( ( m_recentFrameTimestamp.size() - 1 ) * m_liPerfFreq.QuadPart) / ( m_recentFrameTimestamp.back() - m_recentFrameTimestamp.front() );
    }
    return 0.0;
}

void CDepthDlg::OnPaint() {
	
  if(m_pImageBuf == NULL) CResizableDlg::OnPaint();
  else CPaintDC dc(this);
}

unsigned char* CDepthDlg::GetDepthColorMapImage() {
	return m_pImageBuf;
}

void CDepthDlg::GetDepthColorfulImageFromColorPalette(std::vector<BYTE> &buffer)
{
	GetDepthRGBImage(m_ColorPalette, buffer);
}

void CDepthDlg::GetDepthGrayImageFromColorPalette(std::vector<BYTE> &buffer)
{
	GetDepthRGBImage(m_GrayPalette, buffer);
}

void CDepthDlg::GetDepthRGBImage(const RGBQUAD* pColorPalette, std::vector<BYTE> &buffer)
{
	size_t nSize = m_nDepthResWidth * m_nDepthResHeight * 3;
	if (buffer.size() != nSize)
	{
		buffer.resize(nSize, 0);
	}

	switch (m_depthImageType)
	{
	case EtronDIImageType::DEPTH_8BITS:
	{
		if (m_zdTable) UpdateD8DisplayImage_DIB24(pColorPalette, m_depthData.data(), &buffer[0]);
	}
	break;
	case EtronDIImageType::DEPTH_11BITS:
	{
		unsigned char* targetBuf = m_depthData.data();
		if (!m_mblDistMultiplier.empty())
		{
			if (m_tempBuf.size() != m_nDepthResWidth * m_nDepthResHeight * 2)
			{
				m_tempBuf.resize(m_nDepthResWidth * m_nDepthResHeight * 2, 0);
			}
			memcpy(&m_tempBuf[0], m_depthData.data(), m_tempBuf.size());

			targetBuf = &m_tempBuf[0];
			float* pMultiplier = &m_mblDistMultiplier[0];
			if (!m_fusionSelectedIndex.empty())
			{
				unsigned char* pfusionSelectedIndex = &m_fusionSelectedIndex[0];
				for (int i = 0, size = m_nDepthResWidth * m_nDepthResHeight; i < size; ++i)
				{
					if (pfusionSelectedIndex[i] != 255)
					{
						((unsigned short*)targetBuf)[i] = ((unsigned short*)targetBuf)[i] * (unsigned short)pMultiplier[pfusionSelectedIndex[i]];
					}
				}
			}
		}
		if (m_mblDistMultiplier.size()) UpdateD11_Fusion_DisplayImage_DIB24(pColorPalette, (WORD*)targetBuf, (WORD*)m_depthData.data(), &buffer[0]);
		else if (m_camFocus > 0.0 && m_baselineDist > 0.0) UpdateD11_Baseline_DisplayImage_DIB24(pColorPalette, (WORD*)targetBuf, &buffer[0]);
		else if (m_zdTable) UpdateD11DisplayImage_DIB24(pColorPalette, (WORD*)targetBuf, &buffer[0]);
	}
	break;
	case EtronDIImageType::DEPTH_14BITS:
	{
		UpdateZ14DisplayImage_DIB24(pColorPalette, (WORD*)m_depthData.data(), &buffer[0]);
	}
	break;
	}
}

BITMAPINFO* CDepthDlg::GetbmiDept() {
	return &m_bmiDepth;
}

// for paint thread +
void CDepthDlg::TerminatePaintDepthThread() {

  m_bThreadContinue = FALSE;
  m_PaintEvent.SetEvent();     
  WaitForSingleObject(m_PaintThread->m_hThread, INFINITE);
}

UINT CDepthDlg::PaintDepthThreadFunc( LPVOID pParam ) {

    CDC *pDC = NULL;
    CBitmap MemBitmap;
  
    CDepthDlg *pdlg = (CDepthDlg*) pParam;
    int i = 0;
  
    CPoint ptBegin;
    CPoint ptEnd;
    pDC = pdlg->GetDC();

    CRect clientRect;
    CPen cp( PS_SOLID, 1, RGB( 255, 255, 0 ) );

    pdlg->m_MemDC.SelectObject( &cp );

    while( pdlg->m_bThreadContinue )
    {
        ::WaitForSingleObject( pdlg->m_PaintEvent, INFINITE ); 
  
        if( pdlg->m_bThreadContinue )
        { 
            pdlg->GetClientRect(&clientRect);
      
            if( MemBitmap.CreateCompatibleBitmap( pDC, clientRect.Width(), clientRect.Height() ) )
            { 
                pdlg->m_MemDC.SelectObject(&MemBitmap);     

	            StretchDIBits(pdlg->m_MemDC.m_hDC,
		            0, 0, clientRect.Width(), clientRect.Height(),
		            0, 0, pdlg->m_bmiDepth.bmiHeader.biWidth, pdlg->m_bmiDepth.bmiHeader.biHeight,
		            pdlg->m_pImageBuf, &pdlg->m_bmiDepth, DIB_RGB_COLORS, SRCCOPY);
                MemBitmap.DeleteObject();

                if ( !pdlg->m_rtAccuracyRegion.IsRectEmpty() )
                {
                    CRect rt( clientRect.Width()  * pdlg->m_rtAccuracyRegion.left   / pdlg->m_nDepthResWidth,
                              clientRect.Height() * pdlg->m_rtAccuracyRegion.top    / pdlg->m_nDepthResHeight,
                              clientRect.Width()  * pdlg->m_rtAccuracyRegion.right  / pdlg->m_nDepthResWidth,
                              clientRect.Height() * pdlg->m_rtAccuracyRegion.bottom / pdlg->m_nDepthResHeight );
                    pdlg->m_MemDC.Rectangle( rt ); rt.MoveToY( rt.top - rt.Height() );
                    pdlg->m_MemDC.DrawText( L"Accuracy Region", rt, DT_SINGLELINE | DT_CENTER | DT_BOTTOM );
                }
                pDC->BitBlt( 0, 0, clientRect.Width(), clientRect.Height(), &(pdlg->m_MemDC), 0, 0, SRCCOPY);
            }
            pdlg->GenerateZDValueTitleText();
            pdlg->m_csDialogTitle.Format( _T( "DepthDlg( %s ) [RES] %d x %d %s [FPS] %.2f [SN] %d" ),
                                         ( pdlg->m_depthId == -1 ? L"fusion" : CString( std::to_string( pdlg->m_depthId ).c_str()) ),
                                         pdlg->m_bmiDepth.bmiHeader.biWidth, pdlg->m_bmiDepth.bmiHeader.biHeight,
                                         pdlg->m_cs_ZD_Text, pdlg->UpdateAndGetFramerate(), pdlg->m_depthSerialNumber );
            pdlg->PostMessage( WM_MSG_UPDATE_DLG_TITLE );
        }
    }
    if( pDC )
    {
        pdlg->ReleaseDC(pDC);
        pDC = NULL;
    }  
    return 0;
}
// for paint thread -
void CDepthDlg::OnClose() {

  TerminatePaintDepthThread();

  // if dialog's parent has WS_CHILD style, GetParent() will return parent's parent, 
  // thus the only way to get dialog's real parent is using m_pParentWnd directly
  m_pParentWnd->PostMessage(WM_MSG_CLOSE_PREVIEW_DLG, (WPARAM)this);
  CResizableDlg::OnClose();
}

void CDepthDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MAXIMIZE)
    {
        return MaximizeWindow();
    }

    CResizableDlg::OnSysCommand(nID, lParam);
}

void CDepthDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCAPTION)
    {
        return MaximizeWindow();
    }

    CResizableDlg::OnNcLButtonDblClk(nHitTest, point);
}

void CDepthDlg::OnCancel()
{
    SendMessage(WM_CLOSE);
}

void CDepthDlg::SetDepthPos(const CPoint pos)
{
    m_curDepthPos = pos;
}

void CDepthDlg::OnMouseLeave()
{
    m_mouseTracking = false;

    m_pParentWnd->PostMessage( WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, MAKEWPARAM( MAXWORD, MAXWORD ) );

    CResizableDlg::OnMouseLeave();
}

void CDepthDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_mouseTracking)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = 10;

        if (_TrackMouseEvent(&tme))
        {
            m_mouseTracking = true;
        }
    }
    CRect rt;
    GetClientRect( rt );

    CPoint cp2;
    cp2.x = point.x * m_bmiDepth.bmiHeader.biWidth  / rt.Width();
    cp2.y = point.y * m_bmiDepth.bmiHeader.biHeight / rt.Height();

    m_pParentWnd->PostMessage( WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, MAKEWPARAM( cp2.x, cp2.y ) );

    CResizableDlg::OnMouseMove(nFlags, point);
}

void CDepthDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == 'D' && GetKeyState(VK_CONTROL) < 0)// ctrl + D
    {
        // if dialog's parent has WS_CHILD style, GetParent() will return parent's parent, 
        // thus the only way to get dialog's real parent is using m_pParentWnd directly
        m_pParentWnd->PostMessage(WM_MSG_SNAPSHOT_ALL, (WPARAM)this);
    }

    CResizableDlg::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDepthDlg::DepthFilter(BYTE* pDepthBuf, const DepthfilterParam& DfParam)
{
	if (!DfParam.bDoDepthFilter) return;

	unsigned char* sub_disparity = pDepthBuf;
	int new_width = m_nDepthResWidth;
	int new_height = m_nDepthResHeight;

	EtronDI_ResetFilters( m_hEtronDI, &m_DevSelInfo );

	if(DfParam.bSubSample)
	{
		new_width = 0;
		new_height = 0;
		sub_disparity = NULL;	// sub_disparity Initialize;
		EtronDI_SubSample( m_hEtronDI, &m_DevSelInfo, &sub_disparity, pDepthBuf, DfParam.bytesPerPixel, m_nDepthResWidth, m_nDepthResHeight, new_width, new_height, DfParam.SubSampleMode, DfParam.SubSampleFactor);
	}

	if (DfParam.bEdgePreServingFilter)
		EtronDI_EdgePreServingFilter(m_hEtronDI, &m_DevSelInfo, sub_disparity, DfParam.type, new_width, new_height, DfParam.nEdgeLevel, DfParam.sigma, DfParam.lumda);
	
	if (DfParam.bHoleFill)
		EtronDI_HoleFill(m_hEtronDI, &m_DevSelInfo, sub_disparity, DfParam.bytesPerPixel, DfParam.kernelSize, new_width, new_height, DfParam.nLevel, DfParam.bHorizontal);
	
	if (DfParam.bTempleFilter)
		EtronDI_TemporalFilter(m_hEtronDI, &m_DevSelInfo, sub_disparity, DfParam.bytesPerPixel, new_width, new_height, DfParam.alpha, DfParam.history);
	
	if (DfParam.bSubSample)
		EtronDI_ApplyFilters(m_hEtronDI, &m_DevSelInfo, pDepthBuf, sub_disparity, DfParam.bytesPerPixel, m_nDepthResWidth, m_nDepthResHeight, new_width, new_height);


	if (DfParam.bFlyingDepthCancellation)
    {
		if (m_depthImageType == EtronDIImageType::DEPTH_8BITS)
			EtronDI_FlyingDepthCancellation_D8(m_hEtronDI, &m_DevSelInfo, pDepthBuf, m_nDepthResWidth, m_nDepthResHeight);
		else if (m_depthImageType == EtronDIImageType::DEPTH_11BITS)
			EtronDI_FlyingDepthCancellation_D11(m_hEtronDI, &m_DevSelInfo, pDepthBuf, m_nDepthResWidth, m_nDepthResHeight);
        else if (m_depthImageType == EtronDIImageType::DEPTH_14BITS)
        {
            if ( !m_zdTable ) return;

            const UINT Depth_Size = m_nDepthResWidth * m_nDepthResHeight;

            const WORD* zdTable = ( WORD* )m_zdTable;

            if ( Depth_Size != m_vecZ14ToD11.size() ) m_vecZ14ToD11.resize( Depth_Size );

            if ( m_vecTableZ14ToD11.empty() )
            {
                int Desparity = 0;

                m_vecTableZ14ToD11.resize( 16385, 0 );

                for ( int i = 0; i < 2048; i++ )
                {
                    m_vecTableZ14ToD11[ zdTable[ i ] ] = i;
                }
                for ( int i = 16384; i >= 0; i-- )
                {
                    if ( m_vecTableZ14ToD11[ i ] ) Desparity = m_vecTableZ14ToD11[ i ];

                    else m_vecTableZ14ToD11[ i ] = Desparity;
                }
				m_vecTableZ14ToD11[0] = 0;	// #6443
            }
            WORD* pZ14Depth = ( WORD* )pDepthBuf;

            EtronDI_TableToData(m_hEtronDI, &m_DevSelInfo, m_nDepthResWidth, m_nDepthResHeight, m_vecTableZ14ToD11.size() * sizeof( WORD ), m_vecTableZ14ToD11.data(), pZ14Depth, m_vecZ14ToD11.data() );
            EtronDI_FlyingDepthCancellation_D11(m_hEtronDI, &m_DevSelInfo, ( BYTE* )m_vecZ14ToD11.data(), m_nDepthResWidth, m_nDepthResHeight);
            EtronDI_TableToData(m_hEtronDI, &m_DevSelInfo, m_nDepthResWidth, m_nDepthResHeight, 2048 * sizeof( WORD ), ( USHORT* )zdTable, m_vecZ14ToD11.data(), pZ14Depth );
        }
    }
}