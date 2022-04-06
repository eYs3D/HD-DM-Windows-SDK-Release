/* En-Decrypt.cpp		Thursday	July	6 2017 13:56:00 Chris Ke */
/*

Module:	En-Decrypt.cpp

Function:
	Do encryption or decryption on a MP4(H.264) file

Version:
	v1.0a	Thursday	July	6 2017 13:56:00 Chris Ke	 Edit level 1

Copyright:
	This file copyright (C) 2017 by

	eYs3D an Etron company

	An unpublished work.  All rights reserved.

	This file is proprietary information, and may not be disclosed or
	copied without the prior permission of eYs3D an Etron company.

Author:
	Chris Ke, eYs3D an Etron company.	July 2017

Revision hostory:
	v1.0a Thursday	July	6 2017 13:56:00 Chris Ke
	Module created

	v1.1a Tuesday	July 18 2017 11:10:30 Chris Ke
	Add file type to identify encryp or decrypt process, also increase security

	v1.2a Tuesday	July 18 2017 17:31:10 Chris Ke
	fix bug of fwrite fails after fread, have to close and re-open file

	v1.3 a Wednesday	July 19 2017 09:53:18 Chris Ke
	refine the prasing order even moov tag occured before mdat tag

	v1.4 a Thursday	Thursday 20 2017 09:41:36 Chris Ke
	encrypt or decrypt the first 4 bytes of MP4 file (for prevent VLC to play)

*/

#include "stdafx.h"
#include <sys/stat.h>
#include <stdlib.h>
#ifdef __ANDROID__
#include <io.h>
#include "eSPDI.h"
#elif __linux__
#include <sys/io.h>
#include <stdio.h>
#include <errno.h>
#include "eSPDI.h"
#else
#include <io.h>
#include "eSPDI_ErrCode.h"
#endif
#include "En-Decrypt.h"

#define mdat_Hex 0x7461646D
#define ftyp_Hex	0x70797466
#define ftye_Hex	0x65797466
#define encrypt_key 0xf0f0f0f0
#define four_byte_key	0xA5B60f0f

/*
Name : fssek_64()

Function : 
	A 64 bit seek function to seek a file less than 4G

Definition :
	int fseek_64(FILE *stream, eys64 offset, int origin)

Description :
	This is a unified function for Windows and Linux platform that can seek a file
	position from the beginning or ending by input a offset value.

Return :
	0 : successful
	errno : file io error number
*/
int fseek_64(FILE *stream, eys64 offset, int origin)
{
	if (feof(stream)) {
		rewind(stream);
	}
	else {
		setvbuf(stream, NULL, _IONBF, 0); //set stream unbuffered
	}

	int fd = _fileno(stream);
	if (eyslseek64(fd, offset, origin) == -1)
	{
		eyslog("_lseeki64() failed with %d\n", errno);
		return errno;
	}
	return 0;
}

/*
Name : IsValidEncryptedMP4()

Function :
	To check this is a valid encryped MP4 file that was generate by APC_En_DecryptionMP4

Definition :
	bool IsValidEncryptedMP4(FILE *FilePtr)

Description:
	This function checks the input MP4 file has a 'ftye' file type because this tag/atom was
	generated by our APC_En_DecryptionMP4(xx, true) API replacing 'ftyp' with 'ftye'

Returns:
	true		is a valid encrypted MP4
	false		an invalid MP4
*/
bool IsValidEncryptedMP4(FILE *FilePtr)
{
    FILE *file_ptr = FilePtr;
    unsigned char ch;
    unsigned long ltemp;

    for (int i = 0; i < 256; i++)
    {
        fread(&ch, sizeof(unsigned char), 1, file_ptr);
        if (ch == 0x66) // this is 'f'
        {
            ltemp = 0L;
            fread(&ltemp, 3, 1, file_ptr);
            if (ltemp == 0x657974) // this is 'tye'
                return true;
        }
    }
    return false;
}

/*
Name : EnDecryptedFirst4Bytes

Function :
	To encrypt/decrypt the first 4 bytes of MP4 file

Definition :
	int EnDecryptedFirst4Bytes(FILE *file_ptr, unsigned long FourbyteVal)

Description :
	This function encrypt or decrypt the first 4 bytes of input file using a XOR
	operation with the four_byte_key

Returns :
	APC_EN_DECRYPT_4BYTE_WRITE_FAIL	encrypt or decrypt fails
	APC_OK	success
*/
int EnDecryptedFirst4Bytes(FILE *file_ptr, unsigned long FourbyteVal)
{
	unsigned long ltemp = 0;
	size_t w_size;

	fseek_64(file_ptr, 0, SEEK_SET);
	ltemp = FourbyteVal ^ four_byte_key;
	for (int j = 1; j <= 4; j++)
	{
		unsigned char temp;
		fseek_64(file_ptr, 4 - j, SEEK_SET);
		temp = (unsigned char)((ltemp >> (j - 1) * 8) & 0xff);
		w_size = fwrite(&temp, sizeof(unsigned char), 1, file_ptr);
		if (!w_size)
		{
			eyslog("4Byte fwrite() failed with write size(%d) %d\n", (int)w_size, ferror(file_ptr));
			fclose(file_ptr);
			return APC_EN_DECRYPT_4BYTE_WRITE_FAIL;
		}
	}
	return APC_OK;
}

/*
Name : EnDecryptedFileType()

Function :
	To encrypt the ftyp tag/atom of MP4 file

Definition:
	int EnDecryptedFileType(FILE *file_ptr, bool bEncrypt, unsigned long *pos)

Description :
	This function keeps the ftyp tag position of the input MP4 file
	
Returns:
	APC_EN_DECRYPT_FILE_TYPE_FAIL	can not find valid file type tag
	APC_OK	success
*/
int EnDecryptedFileType(FILE *file_ptr, bool bEncrypt, unsigned long *pos)
{
    unsigned char ch;
    unsigned long ltemp;
    bool bSuccess = false;

    for (int i = 0; i < 256; i++)
    {
        fseek_64(file_ptr, i, SEEK_SET);
        fread(&ch, sizeof(unsigned char), 1, file_ptr);
        if (ch == 0x66) // this is 'f'
        {
            ltemp = 0L;
            fread(&ltemp, 3, 1, file_ptr);
            /* encrypt : change ftyp to ftye */
            if ((ltemp == 0x707974 && bEncrypt) ||
                (ltemp == 0x657974 && !bEncrypt))
            {
                *pos = i + 3;
                bSuccess = true;
                break;
            }
        }
    }
    if (bSuccess) return APC_OK;
    else return APC_EN_DECRYPT_FILE_TYPE_FAIL;
}


/*
Name : APC_EncryptionMP4()

Function :
	To encrypt a MP4(H.264) file by modify the length of 'mdat' tag/atom with XOR operation.

Definition :
	int APC_API APC_En_DecryptionMP4(const char *filename, bool bEncrypt)
	
Description :
	The APC_EncryptionMP4 file is called after a MP4 file was generated and it need be
	encrypted, this function will open the original video file and search the 'mdat' tag, once
	it was found then this API will read the 4 byte length of the mdat and apply a simplied
	XOR operation on this length data. The decryption operation also have to do a XOR
	operation on this data to get a correct mdat length then this MP4 can be played.

Returns:
	APC_EN_DECRYPT_INPUT_FILE_NULL	null input file detected
	APC_EN_DECRYPT_OPEN_FILE_FAIL	can not open MP4 file
	APC_EN_DECRYPT_GET_SIZE_FAIL		can not get file size by C stat API
	APC_EN_DECRYPY_MORE_4G_SIZE_FAIL can not support more than 4G file input
	APC_EN_DECRYPT_PARSE_ORDER_FAIL	can not find the parsing order of MP4 
	APC_EN_DECRYPT_WRITE_FAIL		write encrypted code fail
	APC_EN_DECRYPT_FOUND_FAIL		can not find the mdat metadata tag of MP4 file
	
*/
int En_DecryptionMP4(const char *filename, bool bEncrypt)
{
    FILE *file_ptr, *file_ptr1;
    bool bEncryptSuccess = false;
    unsigned long filetype_pos;

    if (!filename)
    {
    eyslog("Cannot get file!\n");
    return APC_EN_DECRYPT_INPUT_FILE_NULL;
    }
	errno_t err = fopen_s(&file_ptr1, filename, "rb+");
	if (err != 0) {
        eyslog("Error: Cannot get file_ptr! Error = %d\n", err);
        return APC_EN_DECRYPT_OPEN_FILE_FAIL;
	}
	file_ptr = file_ptr1; //Why need two file_ptr?

    struct stat st;
    int ret_stat = stat(filename, &st);
    if (ret_stat != 0)
    {
        eyslog("Error: Cannot get ret_stat!\n");
        return APC_EN_DECRYPT_GET_SIZE_FAIL;
    }
    unsigned long file_size = st.st_size;
    if (file_size > 0xffffffff)
    {
        eyslog("Error: File size too large!\n");
        fclose(file_ptr);
        return APC_EN_DECRYPT_MORE_4G_SIZE_FAIL;
    }

    /* check input file is a valid encrypted MP4 before doing decryption */
    if (!bEncrypt)
    {
        if(!IsValidEncryptedMP4(file_ptr1))
            return APC_EN_DECRYPT_INVALID_ENCRYPT;
    }

    /* 
    check moov at the beginning or end
    we search 0x2000 bytes from file beginning
    if we hit "mdat" then it means "moov" at file end or
    we should hit "moov" that means "moov" at file beginning
    */
    unsigned char *cell = new unsigned char[1];
    bool mdat_at_beginning = false;
    bool parsing_order_found = false;
    unsigned long orig_moov_len = 0;
	unsigned long four_byte_val = 0;
	bool found_moov_tag = false;
    int k;
	
    for (k = 0; k < 0x2000; k++)
    {
        fread(cell, sizeof(unsigned char), 1, file_ptr1);
        if (cell[0] == 0x6D) // this is 'm'
        {
            unsigned long ltemp = 0L;
            fread(&ltemp, 3, 1, file_ptr1);
            if (ltemp == 0x766F6F) // this is 'oov'
            {
                mdat_at_beginning = false;
                fseek_64(file_ptr1, k - 4, SEEK_SET);
                for (int m = 3; m >= 0; m--)
                {
                    fread(cell, sizeof(unsigned char), 1, file_ptr1);
                    orig_moov_len += cell[0] << m * 8;
                }
                eyslog("Tag moov Position at beginning position : %d, len = 0x%08x\n", k, orig_moov_len);
				found_moov_tag = true;
                parsing_order_found = true;
                break;
            }
            else if (ltemp == 0x746164) // this is 'dat'
            {
                mdat_at_beginning = true;
                eyslog("Tag mdat Position at beginning position : %d (moov at end)\n", k);
                parsing_order_found = true;
                break;
            }
            else
                fseek(file_ptr1, k, SEEK_SET);
        }
    }
    if (!parsing_order_found)
    {
        eyslog("Error : can not find parsing order\n");
        fclose(file_ptr1);
        delete cell;

        return APC_EN_DECRYPT_PARSE_ORDER_FAIL;
    }

    /* refined the parsing order if moov length is small and moov occured before mdat */
    if (found_moov_tag && ((k + orig_moov_len) < (file_size / 2)))
        mdat_at_beginning = true;

	/* read first 4 bytes value */
	fseek_64(file_ptr, 0, SEEK_SET);
	for (k = 3; k >= 0; k--)
	{
		fread(cell, sizeof(unsigned char), 1, file_ptr);
        four_byte_val += cell[0] << k * 8;
	}

	/* read the ftyp tag position */
    if (EnDecryptedFileType(file_ptr, bEncrypt, &filetype_pos) != APC_OK)
    {
        eyslog("EnDecryptedFileType failed\n");
        return APC_EN_DECRYPT_FILE_TYPE_FAIL;
    }

    /*
    Parsing file and find position of mdat (6D 64 61 74)
    */
    int j;
    unsigned char findCell;
    unsigned long findFullCell;
    unsigned long tag_mdat = 0L;
    unsigned long orig_mdat_len = 0;
    unsigned long encrypt_mdat_len = 0;
    unsigned char ch1;
    for (unsigned long i = 10; i < file_size; i+=1)
    {
        /* searching 'm' */
        mdat_at_beginning == true ? fseek_64(file_ptr, i, SEEK_SET) : fseek_64(file_ptr, -((eys64)i), SEEK_END);
        fread(&findCell, sizeof(unsigned char), 1, file_ptr);
        //eyslog("i=%lld %c\n", i, findCell);

        if (findCell == 0x6D)
        {
            /* searching 'mdat' */
            mdat_at_beginning == true ? fseek_64(file_ptr, i, SEEK_SET) : fseek_64(file_ptr, -((eys64)i), SEEK_END);
            /* have to set inital value or some phone will get wrong value */
            findFullCell = 0L;
            fread(&findFullCell, 4, 1, file_ptr);
            //eyslog("findFullCell:0x%x", findFullCell);
            if (findFullCell == mdat_Hex)
            {
                /* save the tag position */
                tag_mdat = i;
                eyslog("Tag MDAT Position: %d\n", tag_mdat);

                /* save 4 byte(the length of mdat box) before mdat tag position */
                mdat_at_beginning == true ? fseek_64(file_ptr, i - 4, SEEK_SET) : fseek_64(file_ptr, -((eys64)i) - 4, SEEK_END);
                for (j = 3; j >= 0; j--)
                {
                    fread(cell, sizeof(unsigned char), 1, file_ptr);
                    orig_mdat_len += cell[0] << j * 8;
                }
                eyslog("original length of mdat atom = 0x%08x\n", orig_mdat_len);

                /* do encryption or decryption */
                encrypt_mdat_len = orig_mdat_len ^ encrypt_key;
                eyslog("encrypted length of mdat atom = 0x%08x\n", encrypt_mdat_len);

                fclose(file_ptr);
                //file_ptr = fopen(filename, "rb+");
				err = fopen_s(&file_ptr, filename, "rb+");
				if (err != 0) {
					eyslog("Error: Open file %s failed! Error = %d\n", filename, err);
					return APC_EN_DECRYPT_OPEN_FILE_FAIL;
				}
                size_t w_size;

				/* below functions begins to write the encrypted or decrypted values */
				/* 
				have to write after all read operations were done due to fwrite will
				fails if we fread again
				*/
				/* 1st : 4 bytes */
				int bRet = EnDecryptedFirst4Bytes(file_ptr, four_byte_val);
				if (bRet != APC_OK)
					return bRet;
                
				/* 2nd : ftyp tag */
				if (bEncrypt)
                {
                    fseek_64(file_ptr, filetype_pos, SEEK_SET);
                    ch1 = 0x65;
                    w_size = fwrite(&ch1, sizeof(unsigned char), 1, file_ptr);
                    if (!w_size)
                    {
                        eyslog("EnFileType fwrite() failed with write size(%d) %d\n", (int)w_size, ferror(file_ptr));
                        return APC_ENCRYPT_FILE_TYPE_FAIL;
                    }
                }
                else
                {
                    fseek_64(file_ptr, filetype_pos, SEEK_SET);
                    ch1 = 0x70;
                    w_size = fwrite(&ch1, sizeof(unsigned char), 1, file_ptr);
                    if (!w_size)
                    {
                        eyslog("DeFileType fwrite() failed with write size(%d) %d\n", (int)w_size, ferror(file_ptr));
                        return APC_DECRYPT_FILE_TYPE_FAIL;
                    }
                }
				/* 3rd : mdat tag */
				for (j = 1; j <= 4; j++)
				{
                   unsigned char temp;
                   mdat_at_beginning == true ? fseek_64(file_ptr, tag_mdat - j, SEEK_SET) : fseek_64(file_ptr, -((eys64)tag_mdat) - j, SEEK_END);
                   temp = (unsigned char)((encrypt_mdat_len >> (j - 1) * 8) & 0xff);
                   w_size = fwrite(&temp, sizeof(unsigned char), 1, file_ptr);
                   if (!w_size)
                   {
                       eyslog("fwrite() failed with write size(%d) %d\n", (int)w_size, ferror(file_ptr));
						delete cell;
                       fclose(file_ptr);

                       return APC_EN_DECRYPT_WRITE_FAIL;
                   	}
                }
                bEncryptSuccess = true;
				
                break;
            }
        }
    }

    delete cell;
    fclose(file_ptr);

    if (bEncryptSuccess)
	  return APC_OK;
    else
        return APC_EN_DECRYPT_FOUND_FAIL;
}
