/* En-Decrypt.h	v1.0a Thu July 6 2017 July 6 14:10:20 Chris Ke */
/*

Module: En-Decrypt.h

Funtion:
	Encryption and Decryption header file
	
Version:
	v1.0a	Thursday	July	6 2017 14:10:20 Chris Ke	 Edit level 1
	
Copyright notice:
	This file copyright (C) 2017 by

	eYs3D an Etron company

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D an Etron company.
	
Author:
	Chris Ke, eYs3D an Etron company.	July 2017
	
Revision history:
	Thu July 6 2017 July 6 14:10:20 Chris Ke
	Module created.
	
*/

#ifdef __ANDROID__
#define eys64 off64_t
#define eyslseek64 lseek64
#define eyslog(...) LOGI(__VA_ARGS__)
#elif __linux__
#include <sys/types.h>
#include <unistd.h>
#define eys64 off64_t
#define eyslseek64 lseek64
#define eyslog(...) printf(__VA_ARGS__)
#else
#define eys64 __int64
#define eyslseek64 _lseeki64
#define eyslog(...) printf(__VA_ARGS__)
#endif


#define EtronDI_EN_DECRYPT_INPUT_FILE_NULL		-100
#define EtronDI_EN_DECRYPT_OPEN_FILE_FAIL		-101
#define EtronDI_EN_DECRYPT_GET_SIZE_FAIL		-102
#define EtronDI_EN_DECRYPT_MORE_4G_SIZE_FAIL	-103
#define EtronDI_EN_DECRYPT_PARSE_ORDER_FAIL	-104
#define EtronDI_EN_DECRYPT_WRITE_FAIL			-105
#define EtronDI_EN_DECRYPT_FOUND_FAIL			-106
#define EtronDI_EN_DECRYPT_INVALID_ENCRYPT	-107
#define EtronDI_ENCRYPT_FILE_TYPE_FAIL			-108
#define EtronDI_DECRYPT_FILE_TYPE_FAIL			-109
#define EtronDI_EN_DECRYPT_FILE_TYPE_FAIL		-110
#define EtronDI_EN_DECRYPT_4BYTE_WRITE_FAIL		-111

int En_DecryptionMP4(const char *filename, bool bEncrypt);