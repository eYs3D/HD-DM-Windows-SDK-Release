
#include "stdafx.h"
#include "EtronDI_Test.h"
#include "ColorDlg.h"
#include "WmMsgDef.h"

static UINT BASED_CODE indicators[] =
{
    ID_INDICATOR_FPS
};

const size_t maxFrameTimestampQueueSize = 31;

IMPLEMENT_DYNAMIC(CColorDlg, CResizableDlg)

CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CResizableDlg(CColorDlg::IDD, pParent)
{
    m_dlgName = "ColorDlg";
    m_nColorResWidth   = 0;
    m_nColorResHeight  = 0;
    m_imgSerialNumber  = -1;
	m_serialNumberShowImage = -1;

    m_mouseTracking = FALSE;
    m_bRotate = FALSE;
    m_bRun = FALSE;
    m_bImageArrival = FALSE;
    m_eImageType = EtronDIImageType::IMAGE_UNKNOWN;
    m_pShowImage = nullptr;

    m_cpDepth.SetPoint( NULL, NULL );

    QueryPerformanceFrequency( &m_liPerfFreq );
}

CColorDlg::~CColorDlg() 
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColorDlg, CResizableDlg)
    ON_MESSAGE(WM_MSG_UPDATE_DLG_TITLE, &CColorDlg::OnUpdateDlgTitle)
	ON_WM_PAINT()
	ON_WM_CLOSE()
    ON_WM_SYSCOMMAND()
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CColorDlg::SetDlgName(std::string name)
{
    m_dlgName = CString(name.c_str());
}

void CColorDlg::SetColorParams( void* hEtronDI, const DEVSELINFO& devSelInfo, const int imgWidth, const int imgHeight, const BOOL isLRD_Mode, const CPoint& cpDepth )
{
    SetHandle( hEtronDI, devSelInfo );

    m_nColorResWidth = imgWidth; 
    m_nColorResHeight = imgHeight;
    m_cpDepth = cpDepth;
    m_IsLRD_Mode = isLRD_Mode;
}

void CColorDlg::SetHandle( void* hEtronDI, const DEVSELINFO& devSelInfo )
{
    m_hEtronDI = hEtronDI;
    m_DevSelInfo.index = devSelInfo.index;
}

BOOL CColorDlg::OnInitDialog()
{
    CResizableDlg::OnInitDialog();
  
    SetWindowText( m_dlgName );

    m_bRun = TRUE;
    m_bImageArrival = FALSE;
    hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
    m_BmpInfo = ( LPBITMAPINFO )GlobalLock(hloc);
  
    m_BmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_BmpInfo->bmiHeader.biPlanes = 1;
    m_BmpInfo->bmiHeader.biBitCount = 24;
    m_BmpInfo->bmiHeader.biCompression = BI_RGB;
    m_BmpInfo->bmiHeader.biWidth  = m_nColorResWidth;
    m_BmpInfo->bmiHeader.biHeight = -m_nColorResHeight;
    m_BmpInfo->bmiHeader.biSizeImage = m_nColorResWidth * m_nColorResHeight * 3;
  
    m_vecRGBImageBuf.resize( m_BmpInfo->bmiHeader.biSizeImage );
    m_vecRawImageBuf.resize( m_nColorResWidth * m_nColorResHeight * 2 );

    memset( &m_vecRGBImageBuf[ NULL ], 0xF0, m_vecRGBImageBuf.size() );

    SetResizableInfo( m_nColorResWidth, m_nColorResHeight, 320 );

    m_pShowImage = new std::thread( std::bind( &CColorDlg::Thread_ShowImage, this ) );

    return TRUE;
}

float CColorDlg::UpdateAndGetFramerate()
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

bool CColorDlg::GetImage(std::vector<unsigned char>& imgBuf, int& width, int& height, int& serialNumber)
{
	width  = m_nColorResWidth;
	height = m_nColorResHeight;

    std::lock_guard< std::mutex > lock( m_imgBufMutex );

    if ( m_BmpInfo->bmiHeader.biSizeImage != imgBuf.size() ) imgBuf.resize( m_BmpInfo->bmiHeader.biSizeImage );

    memcpy( &imgBuf[ 0 ], &m_vecRGBImageBuf[ 0 ], m_BmpInfo->bmiHeader.biSizeImage );			
      		
    serialNumber = m_serialNumberShowImage;

    return (!imgBuf.empty() && width > 0 && height > 0);
}

void CColorDlg::EnableRotate( const BOOL bRotate )
{
    m_bRotate = bRotate;
}

void CColorDlg::ApplyImage(unsigned char *pColorBuf, int *dataSize, BOOL bIsOutputRGB, BOOL bIsMJPEG, int nColorSerialNum) 
{ 
    std::lock_guard< std::mutex > lock( m_imgBufMutex );

    if ( m_bRun )
    {
        if ( bIsOutputRGB )
        {
            memcpy( &m_vecRGBImageBuf[ NULL ], pColorBuf, *dataSize );

            m_eImageType = EtronDIImageType::COLOR_RGB24;
        }
        else
        {
            memcpy( &m_vecRawImageBuf[ NULL ], pColorBuf, *dataSize );

            m_eImageType = bIsMJPEG ? EtronDIImageType::COLOR_MJPG : EtronDIImageType::COLOR_YUY2;
        }
        m_imgSerialNumber = nColorSerialNum;

        m_bImageArrival = TRUE;

        m_event.notify_all();

        PostMessage( WM_MSG_UPDATE_DLG_TITLE );
    }
}

void CColorDlg::Thread_ShowImage()
{
    std::vector< BYTE > vecRawImageBuf;

    EtronDIImageType::Value eImageType = EtronDIImageType::IMAGE_UNKNOWN;

    CDC* pDC = GetDC();

    while ( TRUE )
    {
        {
            std::unique_lock< std::mutex > lock( m_imgBufMutex );

            m_event.wait( lock, [ = ]()
            {
                return ( !m_bRun || m_bImageArrival );
            } );
            m_bImageArrival = FALSE;
            eImageType      = m_eImageType;

            vecRawImageBuf = m_vecRawImageBuf;
        }
        if ( m_bRun )
        {
            {
                std::unique_lock< std::mutex > lock( m_imgBufMutex );
                if ( ETronDI_OK != EtronDI_ColorFormat_to_RGB24( m_hEtronDI, 
                                                                 &m_DevSelInfo,
                                                                 &m_vecRGBImageBuf[ NULL ],
                                                                 &vecRawImageBuf[ NULL ],
                                                                 vecRawImageBuf.size(),
                                                                 m_nColorResWidth,
                                                                 m_nColorResHeight,
                                                                 eImageType ) )
                {
                    TRACE( "EtronDI_ColorFormat_to_RGB24 fail\n" );
                }
            }
            ShowImage( *pDC );
        }
        else break;
    }
    ReleaseDC( pDC );
}

void CColorDlg::OnPaint()
{
    CPaintDC dc(this);
    CMemDC mDC( dc, this );

    {
        std::lock_guard< std::mutex > lock( m_imgBufMutex );

        ShowImage( mDC.GetDC() );
    }
}

LRESULT CColorDlg::OnUpdateDlgTitle(WPARAM wParam, LPARAM lParam)
{
    m_csDialogTitle.Format( _T( "%s [RES] %d x %d [FPS] %.2f [SN] %d" ),
                            m_dlgName, m_nColorResWidth, m_nColorResHeight, UpdateAndGetFramerate(), m_imgSerialNumber );
    SetWindowText( m_csDialogTitle );

    return NULL;
}

void CColorDlg::ShowImage( CDC& dc )
{
    CRect rt;
    GetClientRect( &rt );

    dc.SetStretchBltMode(STRETCH_DELETESCANS);

    if ( m_vecRGBImageBuf.size() )
    {
        StretchDIBits( dc,
                       NULL, NULL, rt.Width(), rt.Height(),
                       NULL, NULL, m_nColorResWidth, m_nColorResHeight,
                       &m_vecRGBImageBuf[ NULL ],
                       m_BmpInfo,
                       DIB_RGB_COLORS,
                       SRCCOPY );
    }
}

void CColorDlg::OnClose()
{
    {
        std::lock_guard< std::mutex > lock( m_imgBufMutex );

        m_bImageArrival = FALSE;
        m_bRun = FALSE;
    }
    m_event.notify_all();

    if ( m_pShowImage )
    {
        m_pShowImage->join();

        delete m_pShowImage;

        m_pShowImage = nullptr;
    }
    m_pParentWnd->PostMessage( WM_MSG_CLOSE_PREVIEW_DLG, ( WPARAM )this );

    CResizableDlg::OnClose();
}

void CColorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_MAXIMIZE)
    {
        return MaximizeWindow();
    }

    CResizableDlg::OnSysCommand(nID, lParam);
}

void CColorDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCAPTION)
    {
        return MaximizeWindow();
    }

    CResizableDlg::OnNcLButtonDblClk(nHitTest, point);
}

void CColorDlg::OnOK()
{
    OnCancel();
}

void CColorDlg::OnCancel()
{
    SendMessage(WM_CLOSE);
}

void CColorDlg::OnMouseLeave()
{
    m_mouseTracking = false;

    m_pParentWnd->PostMessage(WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, MAKEWPARAM( MAXWORD, MAXWORD ) );

    CResizableDlg::OnMouseLeave();
}

void CColorDlg::OnMouseMove(UINT nFlags, CPoint point)
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
    CPoint cp2( MAXWORD, MAXWORD );
    CRect  rt;
    GetClientRect( rt );

    const int w = rt.Width() / ( m_IsLRD_Mode ? 2 : 1 );

    if ( point.x < w )
    {
        cp2.x = point.x * m_cpDepth.x / w;
        cp2.y = point.y * m_cpDepth.y / rt.Height();
    }
    m_pParentWnd->PostMessage( WM_MSG_UPDATE_MOUSE_POS_FROM_DIALOG, MAKEWPARAM( cp2.x, cp2.y ) );

    CResizableDlg::OnMouseMove( nFlags, point );
}

void CColorDlg::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if ( nChar == 'D' && GetKeyState( VK_CONTROL ) < 0 )// ctrl + D
    {
        m_pParentWnd->PostMessage( WM_MSG_SNAPSHOT_ALL, ( WPARAM )this );
    }
    CResizableDlg::OnKeyDown(nChar, nRepCnt, nFlags);
}
