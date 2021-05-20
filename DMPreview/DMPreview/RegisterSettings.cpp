#include "stdafx.h"
#include "RegisterSettings.h"
RegisterSettings::RegisterSettings() {
}

int exchange(int data)
{
	int data_low;
	int data_high;
	data_low = (data & 0xff00) >> 8;
	data_high = (data & 0x00ff) << 8;
	int exchange_data = data_high + data_low;
	return exchange_data;
}

//int RegisterSettings::FramesyncD0(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps)
//{
//	unsigned short value = 0;
//
//	if (EtronDI_SetPlumAR0330(hEtronDI, pDevSelInfo, true) != ETronDI_OK)
//	{
//		TRACE("EtronDI_SetPlumAR0330 failed !!");
//	}
//
//	if (EtronDI_GetSensorRegister(hEtronDI, pDevSelInfo, 0x30, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
//	{
//		TRACE("Read Register, from Sensor PID 141 VID1E4E ,AR0330,ETronDI_READ_Sensor_REG_0x30_FAIL");
//	}
//	else
//	{
//		TRACE("0x30, 0x301a,value1=%x\n", value);	/*0x5c00*/
//		value = exchange(value);
//		TRACE("0x30, 0x301a,value2=%x\n", value);	/*0x005c*/
//		BIT_CLEAR(value, 2);
//		if (EtronDI_SetSensorRegister(hEtronDI, pDevSelInfo, 0x30, 0x301a, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
//		{
//			TRACE("ETronDI_WRITE_REG_FAIL");
//		}
//		else
//		{
//			if (EtronDI_GetSensorRegister(hEtronDI, pDevSelInfo, 0x30, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
//			{
//				TRACE("Read Register, from Sensor PID 141 VID1E4E ,AR0330,ETronDI_READ_Sensor_REG_0x30_FAIL");
//			}
//			else
//			{
//				value = exchange(value);
//				TRACE("0x30, 0x301a,value3=%x\n", value);	/*0x0058*/
//			}
//		}
//		BIT_SET(value, 8);
//		if (EtronDI_SetSensorRegister(hEtronDI, pDevSelInfo, 0x30, 0x301a, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
//		{
//			TRACE("ETronDI_WRITE_REG_FAIL");
//		}
//		else
//		{
//			if (EtronDI_GetSensorRegister(hEtronDI, pDevSelInfo, 0x30, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
//			{
//				TRACE("Read Register, from Sensor PID 141 VID1E4E ,AR0330,ETronDI_READ_Sensor_REG_0x30_FAIL");
//			}
//			else
//			{
//				value = exchange(value);
//				TRACE("0x30, 0x301a,value4=%x\n", value);	/*0x0158*/
//			}
//		}
//	}
//
//	if (EtronDI_SetPlumAR0330(hEtronDI, pDevSelInfo, false) != ETronDI_OK)
//	{
//		TRACE("EtronDI_SetPlumAR0330 failed !!");
//	}
//
//	return 0;
//}

int RegisterSettings::FrameSync8053_8059_Clock( void* hEtronDI, PDEVSELINFO pDevSelInfo )
{
    if ( EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF07A, 0x01, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
         EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF07B, 0x00, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
         EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF07C, 0x00, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
         EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF07D, 0x00, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK )
    {
        WORD wF01A = NULL;
        WORD wF041 = NULL;
        WORD wF045 = NULL;
        WORD wF0FE = NULL;
        //WORD wE079 = NULL;

        if ( EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF01A, &wF01A, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
             EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF041, &wF041, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
             EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF045, &wF045, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
             EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF0FE, &wF0FE, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK /*&&
             EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF079, &wE079, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK*/ )
        {
            wF01A &= 0xDF; // bit6 = 0
            wF041 |= 0x40; // bit6 = 1
            wF045 &= 0xDF; // bit6 = 0
            wF0FE &= 0xDF; // bit6 = 0
            //wE079 &= 0xFC; // bit0,1 = 0

            if ( EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF01A, wF01A, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
                 EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF041, wF041, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
                 EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF045, wF045, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK &&
                 EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF0FE, wF0FE, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK /*&&
                 EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF079, wE079, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK*/ )
            {
                //if ( EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF079, wE079 + 1, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK )
                //{
                //    return ETronDI_OK; /* enable mudule-sync-success */
                //}
            }
        }
    }
    TRACE("ETronDI_WRITE_REG_FAIL");
    return ETronDI_WRITE_REG_FAIL;
}

int RegisterSettings::FrameSync8053_8059_Reset( void* hEtronDI, PDEVSELINFO pDevSelInfo )
{
    WORD wE079 = NULL;

    if ( EtronDI_GetHWRegister( hEtronDI, pDevSelInfo, 0xF079, &wE079, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK )
    {
        wE079 &= 0xFC; // bit0,1 = 0

        if ( EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF079, wE079, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK )
        {
            if ( EtronDI_SetHWRegister( hEtronDI, pDevSelInfo, 0xF079, wE079 + 1, FG_Address_2Byte | FG_Value_1Byte ) == ETronDI_OK )
            {
                return ETronDI_OK; /* enable mudule-sync-success */
            }
        }
    }
    return ETronDI_WRITE_REG_FAIL;
}

int RegisterSettings::Framesync( void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps, const int iPid )
{
    unsigned short value = 0;
	if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
	{
		TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
	}
	else
	{
		TRACE("0x6c, 0x301a,value1=%x\n", value);	/*0x1c02*/
		value = exchange(value);
		TRACE("0x6c, 0x301a,value2=%x\n", value);	/*0x021c*/
		BIT_CLEAR(value, 2);
		if (EtronDI_SetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
		{
			TRACE("ETronDI_WRITE_REG_FAIL");
		}
		else
		{
			if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
			{
				TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
			}
			else
			{
				value = exchange(value);
				TRACE("0x6c, 0x301a,value3=%x\n", value);	/*0x0218*/
			}
		}
        Sleep( 100 );
		BIT_SET(value, 8);
		if (EtronDI_SetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
		{
			TRACE("ETronDI_WRITE_REG_FAIL");
		}
		else
		{
			if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
			{
				TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
			}
			else
			{
				value = exchange(value);
				TRACE("0x6c, 0x301a,value4=%x\n", value);	/*0x0318*/
			}
		}
		value = 0;
		if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x3026, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
		{
			TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
		}
		else
		{
			TRACE("0x6c, 0x3026,value5=%x\n", value);	/*0xfbff*/
			value = exchange(value);
			TRACE("0x6c, 0x3026,value6=%x\n", value);	/*0xfffb*/
			BIT_CLEAR(value, 7);
			BIT_CLEAR(value, 9);
			BIT_SET(value, 8);
            Sleep( 100 );
			if (EtronDI_SetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x3026, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
			{
				TRACE("ETronDI_WRITE_REG_FAIL");
			}
			else
			{
				if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x3026, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
				{
					TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
				}
				else
				{
					value = exchange(value);
					TRACE("0x6c, 0x3026,value7=%x\n", value);	/*0xfd7b*/
				}
			}
			value = 0;
			if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x315E, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
			{
				TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
			}
			else
			{
				value = exchange(value);
				TRACE("0x6c, 0x315E,value8=%x\n", value);	/*0x0*/
				BIT_SET(value, 0);
                Sleep( 100 );
				if (EtronDI_SetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x315E, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
				{
					TRACE("ETronDI_WRITE_REG_FAIL");
				}
				else
				{
					if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x315E, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
					{
						TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
					}
					else
					{
						value = exchange(value);
						TRACE("0x6c, 0x315E,value9=%x\n", value);	/*0x1*/
					}
				}
			}
			value = 0;
			if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
			{
				TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
			}
			else
			{
				TRACE("0x6c, 0x301a,value6=%x\n", value);	/*0x1803*/
				value = exchange(value);
				TRACE("0x6c, 0x301a,value10=%x\n", value);	/*0x0318*/
				BIT_SET(value, 2);
                Sleep( 100 );
				if (EtronDI_SetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
				{
					TRACE("ETronDI_WRITE_REG_FAIL");
				}
				else
				{
					if (EtronDI_GetSlaveSensorRegister(hEtronDI, pDevSelInfo, 0x6c, 0x301a, &value, FG_Address_2Byte | FG_Value_2Byte, 2) != ETronDI_OK)
					{
						TRACE("Read Register, from Sensor PID 143 VID1E4E ,AR1335,ETronDI_READ_Sensor_REG_0x6c_FAIL");
					}
					else
					{
						value = exchange(value);
						TRACE("0x6c, 0x301a,value11=%x\n", value);	/*0x031c*/
					}
				}
			}
		}
	}
    if ( EtronDI_GetFWRegister(hEtronDI, pDevSelInfo, 0xf4, &value, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK ||
         EtronDI_GetFWRegister(hEtronDI, pDevSelInfo, 0xf5, &value, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK )
	{
		TRACE("Read Register, from PUMA,ETronDI_WRITE_FW_REG_0xf4_FAIL");
		return -1;
	}
	switch ( iPid )
    {
    case ETronDI_PID_8054:  return FramesyncFor8054( hEtronDI, pDevSelInfo, DepthWidth, DepthHeight, ColorWidth, ColorHeight, bFormatMJPG, Fps );
    case ETronDI_PID_8040S: return FramesyncFor8040S( hEtronDI, pDevSelInfo, DepthWidth, DepthHeight, ColorWidth, ColorHeight, bFormatMJPG, Fps );
    }
    return ETronDI_NotSupport;
}

int RegisterSettings::FramesyncFor8054(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps)
{
	/*Check mode*/
	int mode = 0;
	if (DepthWidth == 1920 && DepthHeight == 1080)
	{
		if (ColorWidth == 3840 && ColorHeight == 2160 && bFormatMJPG)
			mode = 5;
		else if (ColorWidth == 4080 && ColorHeight == 3120 && bFormatMJPG)
			mode = 6;
		else if (ColorWidth == 3840 && ColorHeight == 2160 && !bFormatMJPG)
			mode = 7;
		else if (ColorWidth == 4208 && ColorHeight == 3120 && !bFormatMJPG)
			mode = 8;
		else if (ColorWidth == 1920 && ColorHeight == 1080 && bFormatMJPG)
			mode = 9;
	}
	else if (DepthWidth == 1080 && DepthHeight == 1440)
	{
		if (ColorWidth == 3840 && ColorHeight == 2160 && !bFormatMJPG)
			mode = 10;
		else if (ColorWidth == 3840 && ColorHeight == 2160 && bFormatMJPG)
			mode = 11;
	}
	else if (DepthWidth == 1280 && DepthHeight == 1280)
	{
		if (ColorWidth == 2560 && ColorHeight == 2560 && bFormatMJPG && Fps == 30)
			mode = 16;
		else if (ColorWidth == 2560 && ColorHeight == 2560 && !bFormatMJPG && Fps == 30)
			mode = 17;
		else if (ColorWidth == 2560 && ColorHeight == 2560 && !bFormatMJPG && Fps == 5)
			mode = 23;
	}
	else if (DepthWidth == 640 && DepthHeight == 640)
	{
		if (ColorWidth == 1280 && ColorHeight == 1280 && !bFormatMJPG)
			mode = 22;
	}TRACE("RegisterSettings::FramesyncFor8054 Mode: %d  set Frame-sync reg.", mode);
	/* TRIGGER */
	if (mode == 5 || mode == 7)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x004e, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0001, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 6 || mode == 8)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0075, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0001, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 9)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0014, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0014, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 10 || mode == 11)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0050, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0010, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 16)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x003e, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0001, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 17)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x005e, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0002, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 22)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0013, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x000c, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 23)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0050, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0010, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
    return ETronDI_OK;
}

int RegisterSettings::FramesyncFor8040S(void* hEtronDI, PDEVSELINFO pDevSelInfo, int DepthWidth, int DepthHeight, int ColorWidth, int ColorHeight, bool bFormatMJPG, int Fps)
{
	int mode = 0;
	if (DepthWidth == 1920 && DepthHeight == 912)
	{
		if (ColorWidth == 3840 && ColorHeight == 1824 && Fps == 10)
			mode = 5;
		else if (ColorWidth == 3840 && ColorHeight == 1824 && Fps == 5)
			mode = 6;
		else if (ColorWidth == 2560 && ColorHeight == 1216 && Fps == 10)
			mode = 7;
		else if (ColorWidth == 2560 && ColorHeight == 1216 && Fps == 5)
			mode = 9;
	}
	else if (DepthWidth == 960 && DepthHeight == 456)
	{
		if (ColorWidth == 1920 && ColorHeight == 912 && Fps == 30)
			mode = 8;
	}TRACE("RegisterSettings::FramesyncFor8040S Mode: %d  set Frame-sync reg.", mode);
	if (mode == 5)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x005a, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0005, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 6)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x00b9, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0005, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 7)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0028, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x000A, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 8)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x001e, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0001, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	else if (mode == 9)
	{
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf4, 0x0050, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
		if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, 0xf5, 0x0014, FG_Address_1Byte | FG_Value_1Byte) != ETronDI_OK)
			TRACE("ETronDI_WRITE_REG_FAIL");
	}
	return ETronDI_OK;
}

int RegisterSettings::ForEx8053Mode9(void* hEtronDI, PDEVSELINFO pDevSelInfo)
{
	unsigned short value = 0x0;
	unsigned short address = 0xf0;
	if (EtronDI_GetFWRegister(hEtronDI, pDevSelInfo, address, &value, FG_Address_2Byte | FG_Value_1Byte) != ETronDI_OK)
	{
		TRACE("Read Register, READ FW REG %x FAIL", address);
		return -1;
	}

	value |= 0x0001;
	if (EtronDI_SetFWRegister(hEtronDI, pDevSelInfo, address, value, FG_Address_2Byte | FG_Value_1Byte) != ETronDI_OK)
	{
		TRACE("SeT Register,  WRITE FW REG %x FAIL", address);
		return -1;
	}
    return ETronDI_OK;
}

int RegisterSettings::DM_Quality_Register_Setting_Slave(void* hEtronDI, PDEVSELINFO pDevSelInfo)
{
	DEVINFORMATIONEX devinfo;
	int nRet = EtronDI_GetDeviceInfoEx(hEtronDI, pDevSelInfo, &devinfo);
	if (nRet != ETronDI_OK)
	{
		return nRet;
	}

	wchar_t *modelName = NULL;

    if (devinfo.wPID == 0x124) // EX8038 for M1
	{
		modelName = L"EX8038_BL15cm";
	}
	else
	{
		modelName = NULL;
		return -1;
	}

	CString filePrvName;
	filePrvName.Format(_T(".\\DM_Quality_Cfg\\%s_DM_Quality_Register_Setting.cfg"), modelName);
	//char *fileName = (char *)filePrvName.GetBuffer(0);
	//fileName = ".\\DM_Quality_Cfg\\EX8054_DM_Quality_Register_Setting.cfg";
	//TRACE("DM_Quality_Register_Setting::fileName =%s\n", fileName);

	FILE* fp = _wfopen(filePrvName, L"r");
	if (fp == NULL)
	{
		AfxMessageBox(_T("Can not open the config. file !!"));
		return -1;
	}

	const int len = 18; // 17+1
	char Line[len];
	unsigned int LineValue[len];
	int RegAddress, ValidDataRange, Data;


	while (fgets(Line, len, fp) != NULL)
	{
		for (int i = 0; i < len; i++)
		{
			if (Line[i] >= '0' && Line[i] <= '9')
			{
				LineValue[i] = Line[i] - '0';
			}
			else if (Line[i] >= 'A' && Line[i] <= 'F')
			{
				LineValue[i] = Line[i] - 'A' + 10;
			}
			else if (Line[i] >= 'a' && Line[i] <= 'f')
			{
				LineValue[i] = Line[i] - 'a' + 10;
			}
			else
			{
				LineValue[i] = 0;
			}
		}

		// Check Format
		if ((Line[0] == '0') && (Line[6] == ',') && (Line[11] == ',') && (Line[12] == '0'))
		{
			// valid data
			RegAddress = (LineValue[2] << 12) + (LineValue[3] << 8) + (LineValue[4] << 4) + LineValue[5];
			ValidDataRange = (LineValue[9] << 4) + LineValue[10];
			Data = (LineValue[14] << 4) + LineValue[15];

			////////////////////

			USHORT RegValue;
			USHORT NotValidDataRange;

			EtronDI_GetSlaveHWRegister(hEtronDI, pDevSelInfo, RegAddress, &RegValue, FG_Address_2Byte | FG_Value_1Byte);

			NotValidDataRange = ~ValidDataRange;
			RegValue = RegValue & NotValidDataRange;
			RegValue |= Data;

			EtronDI_SetSlaveHWRegister(hEtronDI, pDevSelInfo, RegAddress, RegValue, FG_Address_2Byte | FG_Value_1Byte);

			//Sleep(5); // delay time, need fine tune in the feature

			////////////////////

			USHORT RegValueCheck = 0;
			EtronDI_GetSlaveHWRegister(hEtronDI, pDevSelInfo, RegAddress, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
			if (RegValue != RegValueCheck)
			{
				AfxMessageBox(_T("Set config. to Register failed !!"));
				return -1;
			}

		}
		else
		{
			AfxMessageBox(_T("The config. format is not correct !!"));
			return -1;
		}
	}

	fclose(fp);

	//AfxMessageBox(_T("Set config. to Register done !!"));
	return 0;
}

int RegisterSettings::DM_Quality_Register_Setting_For6cm(void* hEtronDI, PDEVSELINFO pDevSelInfo)
{
	DEVINFORMATIONEX devinfo;
	int nRet = EtronDI_GetDeviceInfoEx(hEtronDI, pDevSelInfo, &devinfo);
	if (nRet != ETronDI_OK)
	{
		return nRet;
	}

	wchar_t *modelName = NULL;

    if (devinfo.wPID == 0x124) // EX8038 for M0
	{
		modelName = L"EX8038_BL6cm";
	}
	else
	{
		modelName = NULL;
		return -1;
	}

	CString filePrvName;
	filePrvName.Format(_T(".\\DM_Quality_Cfg\\%s_DM_Quality_Register_Setting.cfg"),modelName);
	//char *fileName = (char *)filePrvName.GetBuffer(0);
	//fileName = ".\\DM_Quality_Cfg\\EX8054_DM_Quality_Register_Setting.cfg";
	//TRACE("DM_Quality_Register_Setting::fileName =%s\n", fileName);

	FILE* fp = _wfopen(filePrvName, L"r");
	if (fp == NULL)
	{
		AfxMessageBox(_T("Can not open the config. file !!"));
		return -1;
	}

	const int len = 18; // 17+1
	char Line[len];
	unsigned int LineValue[len];
	USHORT RegAddress;
	int ValidDataRange, Data;


	while (fgets(Line, len, fp) != NULL)
	{
		for (int i = 0; i < len; i++)
		{
			if (Line[i] >= '0' && Line[i] <= '9')
			{
				LineValue[i] = Line[i] - '0';
			}
			else if (Line[i] >= 'A' && Line[i] <= 'F')
			{
				LineValue[i] = Line[i] - 'A' + 10;
			}
			else if (Line[i] >= 'a' && Line[i] <= 'f')
			{
				LineValue[i] = Line[i] - 'a' + 10;
			}
			else
			{
				LineValue[i] = 0;
			}
		}

		// Check Format
		if ((Line[0] == '0') && (Line[6] == ',') && (Line[11] == ',') && (Line[12] == '0'))
		{
			// valid data
			RegAddress = (LineValue[2] << 12) + (LineValue[3] << 8) + (LineValue[4] << 4) + LineValue[5];
			ValidDataRange = (LineValue[9] << 4) + LineValue[10];
			Data = (LineValue[14] << 4) + LineValue[15];

			////////////////////

			USHORT RegValue;
			USHORT NotValidDataRange;

			EtronDI_GetSensorRegister(hEtronDI, pDevSelInfo, 0xC2, RegAddress, &RegValue, FG_Address_2Byte | FG_Value_1Byte, EtronDI_SensorMode::Sensor1);

			NotValidDataRange = ~ValidDataRange;
			RegValue = RegValue & NotValidDataRange;
			RegValue |= Data;

			EtronDI_SetSensorRegister(hEtronDI, pDevSelInfo, 0xC2, RegAddress, RegValue, FG_Address_2Byte | FG_Value_1Byte, EtronDI_SensorMode::Sensor1);
			//Sleep(5); // delay time, need fine tune in the feature

			////////////////////

			USHORT RegValueCheck = 0;
			EtronDI_GetSensorRegister(hEtronDI, pDevSelInfo, 0xC2, RegAddress, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte, EtronDI_SensorMode::Sensor1);
			if (RegValue != RegValueCheck)
			{
				AfxMessageBox(_T("Set config. to Register failed !!"));
				return -1;
			}

		}
		else
		{
			AfxMessageBox(_T("The config. format is not correct !!"));
			return -1;
		}
	}

	fclose(fp);

	//AfxMessageBox(_T("Set config. to Register done !!"));
	return 0;
}

int RegisterSettings::DM_Quality_Register_Setting(void* hEtronDI, PDEVSELINFO pDevSelInfo)
{
	DEVINFORMATIONEX devinfo;
	int nRet = EtronDI_GetDeviceInfoEx(hEtronDI, pDevSelInfo, &devinfo);
	if (nRet != ETronDI_OK)
	{
		return nRet;
	}

	wchar_t *modelName = NULL;
	wchar_t *versionName = NULL;

	if (devinfo.wPID == 0x120) // EX8036
	{
		modelName = L"EX8036";
	}
	else if (devinfo.wPID == 0x121) // EX8037
	{
		modelName = L"EX8037";
	}
	else if (devinfo.wPID == 0x124) // //EX8038_M0
	{
		modelName = L"EX8038_BL3cm";
	}
	else if (devinfo.wPID == 0x131) // EX8040S
	{
		modelName = L"EX8040S";
	}
	else if (devinfo.wPID == 0x137) // EX8052
	{
		modelName = L"EX8052";
	}
	else if (devinfo.wPID == 0x138) // EX8053
	{
		modelName = L"EX8053";
	}
	else if (devinfo.wPID == 0x139) // EX8054
	{
		modelName = L"EX8054";
	}
	else if (devinfo.wPID == 0x146) // YX8059
	{
		modelName = L"YX8059";
		versionName = L"_v1.3";
	}
	else if (devinfo.wPID == 0x162) // YX8062
	{
		modelName = L"YX8062";
	}
	else
	{
		modelName = NULL;
		return -1;
	}

	CString filePrvName;
	filePrvName.Format(_T(".\\DM_Quality_Cfg\\%s_DM_Quality_Register_Setting.cfg"),modelName);
	//char *fileName = (char *)filePrvName.GetBuffer(0);
	//fileName = ".\\DM_Quality_Cfg\\EX8054_DM_Quality_Register_Setting.cfg";
	//TRACE("DM_Quality_Register_Setting::fileName =%s\n", fileName);

	FILE* fp = _wfopen(filePrvName, L"r");
	if (fp == NULL)
	{
		TCHAR buff[MAX_PATH];//
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strPath_Application = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);
		filePrvName.Format(_T("DM_Quality_Cfg\\%s_DM_Quality_Register_Setting.cfg"), modelName);
		CString filePrvName_FullPath = strPath_Application + filePrvName;
		filePrvName = filePrvName_FullPath;

		fp = _wfopen(filePrvName, L"r");
		if (fp == NULL)
		{
			AfxMessageBox(_T("Can not open the config. file !!"));
			return -1;
		}
	}

	const int len = 18; // 17+1
	char Line[len];
	unsigned int LineValue[len];
	int RegAddress, ValidDataRange, Data;

	
	while (fgets(Line, len, fp) != NULL)
	{
		for (int i = 0; i < len; i++)
		{
			if (Line[i] >= '0' && Line[i] <= '9')
			{
				LineValue[i] = Line[i] - '0';
			}
			else if (Line[i] >= 'A' && Line[i] <= 'F')
			{
				LineValue[i] = Line[i] - 'A' + 10;
			}
			else if (Line[i] >= 'a' && Line[i] <= 'f')
			{
				LineValue[i] = Line[i] - 'a' + 10;
			}
			else
			{
				LineValue[i] = 0;
			}
		}

		// Check Format
		if ((Line[0] == '0') && (Line[6] == ',') && (Line[11] == ',') && (Line[12] == '0'))
		{
			// valid data
			RegAddress = (LineValue[2] << 12) + (LineValue[3] << 8) + (LineValue[4] << 4) + LineValue[5];
			ValidDataRange = (LineValue[9] << 4) + LineValue[10];
			Data = (LineValue[14] << 4) + LineValue[15];

			////////////////////

			USHORT RegValue;
			USHORT NotValidDataRange;
			
			EtronDI_GetHWRegister(hEtronDI, pDevSelInfo, RegAddress, &RegValue, FG_Address_2Byte | FG_Value_1Byte);
			
			NotValidDataRange = ~ValidDataRange;
			RegValue = RegValue & NotValidDataRange;
			RegValue |= Data;

			EtronDI_SetHWRegister(hEtronDI, pDevSelInfo, RegAddress, RegValue, FG_Address_2Byte | FG_Value_1Byte);

			//Sleep(5); // delay time, need fine tune in the feature

			////////////////////
			
			USHORT RegValueCheck = 0;
			EtronDI_GetHWRegister(hEtronDI, pDevSelInfo, RegAddress, &RegValueCheck, FG_Address_2Byte | FG_Value_1Byte);
			if ( ( RegValue & ValidDataRange ) != ( RegValueCheck & ValidDataRange ) )
			{
				AfxMessageBox(_T("Set config. to Register failed !!"));
				return -1;
			}
			
		}
		else
		{
			AfxMessageBox(_T("The config. format is not correct !!"));
			return -1;
		}
	}
	
	fclose(fp);

	//AfxMessageBox(_T("Set config. to Register done !!"));
	return 0;
}
