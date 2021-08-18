#pragma once

struct sqlite3;
class ModeConfig
{
public:

    enum IMU_TYPE
    {
        IMU_NONE,
        IMU_6Axis = 6,
        IMU_9Axis = 9,
    };
    struct MODE_CONFIG
    {
        enum DECODE_TYPE
        {
            YUYV,
            MJPEG
        };
        struct RESOLUTION
        {
            int Width;
            int Height;

            RESOLUTION() : Width( NULL ), Height( NULL ) {}
        };       
        int                iMode;
        int                iUSB_Type;
        int                iInterLeaveModeFPS;
        BOOL               bRectifyMode;
        DECODE_TYPE        eDecodeType_L;
        DECODE_TYPE        eDecodeType_K;
        DECODE_TYPE        eDecodeType_T;
        RESOLUTION         L_Resolution;
        RESOLUTION         D_Resolution;
        RESOLUTION         K_Resolution;
        RESOLUTION         T_Resolution;      
        std::vector< int > vecDepthType;   
        std::vector< int > vecColorFps;
        std::vector< int > vecDepthFps;
        CString            csModeDesc;

        MODE_CONFIG() : eDecodeType_L( YUYV ), eDecodeType_K( YUYV ),  eDecodeType_T( YUYV ), iInterLeaveModeFPS( NULL ) {}
    };
    const std::vector< MODE_CONFIG >& GetModeConfigList( const int iPID );
    IMU_TYPE GetIMU_Type( const int iPID );

    static ModeConfig& GetModeConfig() { return m_ModeConfig; }

private:

    struct PID_TABLE
    {
        CStringA csTableName;
        IMU_TYPE IMU_Type;

        std::vector< MODE_CONFIG > vecModeConfig;

        PID_TABLE() : IMU_Type( IMU_NONE ) {}
    };
    ModeConfig();
    ~ModeConfig();

    static ModeConfig m_ModeConfig;

    sqlite3* m_sq3;
    std::map< int, PID_TABLE > m_mapDeviceTable;

    void ReadModeConfig();
};

extern ModeConfig& g_ModeConfig;