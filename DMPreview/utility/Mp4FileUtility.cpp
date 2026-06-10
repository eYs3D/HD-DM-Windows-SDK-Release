#include "stdafx.h"
#include "Mp4FileUtility.h"

const int maxExtraDataSizeInMvhd = 9;
const int maxExtraDataSizeInVideoTrak = 14;
const int maxExtraDataSize = maxExtraDataSizeInMvhd + maxExtraDataSizeInVideoTrak;

int MaxMp4ExtraDataSize()
{
    return maxExtraDataSize;
}

class CAutoSeekBack
{
public:
    CAutoSeekBack(std::fstream& fileStream) : m_fileStream(fileStream)
    {
        m_originalPos = m_fileStream.tellg();
    }

    ~CAutoSeekBack()
    {
        m_fileStream.seekg(m_originalPos);
    }

private:
    std::fstream& m_fileStream;
    std::streampos m_originalPos;
};


void GetAtomSizeAndTag(std::fstream& mp4File, uint32_t& atomSize, char* tag)
{
    unsigned char buf[8] = { '\0' };
    mp4File.read((char*)buf, 8);
    mp4File.seekg(-8, std::ios::cur);

    atomSize = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
    memcpy(tag, buf + 4, 4);
}

bool SeekToNextAtom(std::fstream& mp4File)
{
    uint32_t atomSize = 0;
    char tag[5] = { '\0' };
    GetAtomSizeAndTag(mp4File, atomSize, tag);

    mp4File.seekg(atomSize, std::ios::cur);
    return (mp4File.tellg() >= 0);
}

bool SeekToMoovBox(std::fstream& mp4File)
{
    const int maxCheckSize = 8192;
    mp4File.seekg(0, std::ios::end);
    int64_t fileSize = (int64_t)mp4File.tellg();
    int64_t bufSize = fileSize > maxCheckSize ? maxCheckSize : fileSize;
    std::vector<char> buffer(bufSize, 0);

    // find at file head
    mp4File.seekg(0, std::ios::beg);
    mp4File.read(&buffer[0], bufSize);
    for (int i = 0; i < bufSize - 3; ++i)
    {
        if (buffer[i] == 'm' && buffer[i + 1] == 'o' && buffer[i + 2] == 'o' && buffer[i + 3] == 'v')
        {
            mp4File.seekg(i - 4, std::ios::beg);
            return true;
        }
    }

    // find at file tail
    mp4File.seekg(-bufSize, std::ios::end);
    mp4File.read((char*)&buffer[0], bufSize);
    for (int i = 0; i < bufSize - 3; ++i)
    {
        if (buffer[i] == 'm' && buffer[i + 1] == 'o' && buffer[i + 2] == 'o' && buffer[i + 3] == 'v')
        {
            mp4File.seekg(-bufSize + (i - 4), std::ios::end);
            return true;
        }
    }

    mp4File.seekg(0, std::ios::beg);
    return false;
}

bool AccessAtomData(std::fstream& mp4File, std::streampos endPos, std::string targetAtomTag,
    std::vector<std::pair<int64_t, int64_t>> dataPosAndSize, std::vector<std::vector<char>>& data, bool write)
{
    if (write && dataPosAndSize.size() != data.size())
    {
        return false;
    }

    CAutoSeekBack autoSeekBack(mp4File);

    do
    {
        uint32_t atomSize = 0;
        char tag[5] = { '\0' };
        GetAtomSizeAndTag(mp4File, atomSize, tag);

        std::vector<char> tempBuf;
        if (std::string(tag) == targetAtomTag)
        {
            for (size_t i = 0; i < dataPosAndSize.size(); ++i)
            {
                int64_t seekCount = (i != 0 ? dataPosAndSize[i].first - dataPosAndSize[i - 1].first - dataPosAndSize[i - 1].second 
                                            : dataPosAndSize[i].first);
                mp4File.seekg(seekCount, std::ios::cur);

                if (write)
                {
                    mp4File.write(&data[i][0], data[i].size());
                }
                else
                {
                    data.push_back(std::vector<char>(dataPosAndSize[i].second, 0));
                    mp4File.read(&data[data.size() - 1][0], dataPosAndSize[i].second);
                }
            }

            return true;
        }
    } while (mp4File.tellg() < endPos && SeekToNextAtom(mp4File));

    return false;
}

bool IsVideoTrakAtom(std::fstream& mp4File)
{
    CAutoSeekBack autoSeekBack(mp4File);

    uint32_t trakAtomSize = 0;
    char trakTag[5] = { '\0' };
    GetAtomSizeAndTag(mp4File, trakAtomSize, trakTag);
    const std::streampos trakEndPos = mp4File.tellg() + std::streampos(trakAtomSize);

    // seek to the first sub atom in trak atom
    char buf[8] = { '\0' };
    mp4File.read(buf, 8);

    do
    {
        uint32_t atomSize = 0;
        char tag[5] = { '\0' };
        GetAtomSizeAndTag(mp4File, atomSize, tag);

        if (std::string(tag) == "mdia")
        {
            // seek to the first sub atom in mdia atom
            char buf[8] = { '\0' };
            mp4File.read(buf, 8);

            std::vector<std::vector<char>> subtype;
            return (AccessAtomData(mp4File, mp4File.tellg() + std::streampos(atomSize), "hdlr",
                        { { 16, 4 } }, subtype, false) &&
                    subtype[0] == std::vector<char>({ 'v', 'i', 'd', 'e' }));
        }
    } while (mp4File.tellg() < trakEndPos &&  SeekToNextAtom(mp4File));

    return false;
}

bool AccessExtraDataInVideoTrackTkhdAtom(std::fstream& mp4File, std::vector<char>& dataInTrak, bool write)
{
    if (write && (dataInTrak.size() > maxExtraDataSizeInVideoTrak || dataInTrak.empty()))
    {
        return false;
    }

    CAutoSeekBack autoSeekBack(mp4File);

    uint32_t trakAtomSize = 0;
    char trakTag[5] = { '\0' };
    GetAtomSizeAndTag(mp4File, trakAtomSize, trakTag);
    const std::streampos trakEndPos = mp4File.tellg() + std::streampos(trakAtomSize);

    // seek to the first sub atom in trak atom
    char buf[8] = { '\0' };
    mp4File.read(buf, 8);

    std::vector<std::pair<int64_t, int64_t>> tkhdDataPosAndSize;
    std::vector<std::vector<char>> tkhdData;
    if (write)
    {
        if (dataInTrak.size() <= 4)
        {
            tkhdDataPosAndSize = { { 24, dataInTrak.size() } };
        }
        else if (dataInTrak.size() <= 12)
        {
            tkhdDataPosAndSize = { { 24, 4 }, { 32, dataInTrak.size() - 4 } };
        }
        else
        {
            tkhdDataPosAndSize = { { 24, 4 }, { 32, 8 }, { 46, dataInTrak.size() - 12 } };
        }

        int64_t baseIndex = 0;
        for (auto posAndSize : tkhdDataPosAndSize)
        {
            tkhdData.push_back(std::vector<char>(posAndSize.second, 0));
            memcpy(&tkhdData[tkhdData.size() - 1][0], &dataInTrak[baseIndex], posAndSize.second);
            baseIndex += posAndSize.second;
        }
    }
    else
    {
        tkhdDataPosAndSize = { { 24, 4 }, { 32, 8 }, {46, 2} };
    }

    if (!AccessAtomData(mp4File, trakEndPos, "tkhd", tkhdDataPosAndSize, tkhdData, write))
    {
        return false;
    }
    
    if (!write)
    {
        dataInTrak.resize(maxExtraDataSizeInVideoTrak);
        int64_t baseIndex = 0;
        for (auto pieceOfData : tkhdData)
        {
            memcpy(&dataInTrak[baseIndex], &pieceOfData[0], pieceOfData.size());
            baseIndex += pieceOfData.size();
        }
    }

    return true;
}

//
// extra data is stored in the reserved bytes of moov box.
// mvhd (10 bytes): first bytes is the data length + 9 bytes
// trak - tkhd (4 bytes + 8 bytes + 2 bytes)
//
bool AccessMp4FileExtraData(std::string filename, std::vector<char>& extraData, bool write)
{
    if (write && (extraData.size() > maxExtraDataSize || extraData.empty()))
    {
        return false;
    }

    std::fstream mp4File(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!mp4File.good())
    {
        return false;
    }

    if (!SeekToMoovBox(mp4File))
    {
        return false;
    }

    uint32_t moovSize = 0;
    char moovTag[5] = { '\0' };
    GetAtomSizeAndTag(mp4File, moovSize, moovTag);
    const std::streampos moovEndPos = mp4File.tellg() + std::streampos(moovSize);

    // seek to the first atom in moov box
    char buf[8] = { '\0' };
    mp4File.read(buf, 8);

    std::vector<std::pair<int64_t, int64_t>> mvhdDataPosAndSize;
    std::vector<std::vector<char>> mvhdData;
    if (write)
    {
        mvhdData.push_back({ (char)extraData.size() });
        size_t size = (extraData.size() > maxExtraDataSizeInMvhd ? maxExtraDataSizeInMvhd : extraData.size());
        mvhdData.push_back(std::vector<char>(size, 0));
        memcpy(&mvhdData[mvhdData.size() - 1][0], &extraData[0], size);

        mvhdDataPosAndSize = { { 34, 1 },{ 35, size } };
    }
    else
    {
        mvhdDataPosAndSize = { { 34, 1 },{ 35, maxExtraDataSizeInMvhd } };
    }

    if (!AccessAtomData(mp4File, moovEndPos, "mvhd", mvhdDataPosAndSize, mvhdData, write))
    {
        return false;
    }

    if (!write)
    {
        extraData.resize(mvhdData[0][0]);
        int mvhdReadSize = (mvhdData[0][0] > maxExtraDataSizeInMvhd ? maxExtraDataSizeInMvhd : mvhdData[0][0]);
        memcpy(&extraData[0], &mvhdData[1][0], mvhdReadSize);
    }

    if (mvhdData[0][0] <= maxExtraDataSizeInMvhd)
    {
        return true;
    }

    bool videoTrakFound = false;
    do
    {
        uint32_t atomSize = 0;
        char tag[5] = { '\0' };
        GetAtomSizeAndTag(mp4File, atomSize, tag);

        if (std::string(tag) == "trak" && IsVideoTrakAtom(mp4File))// data is only stored in video trak
        {
            std::vector<char> dataInTrak;
            if (write)
            {
                dataInTrak.resize(extraData.size() - maxExtraDataSizeInMvhd, 0);
                memcpy(&dataInTrak[0], &extraData[maxExtraDataSizeInMvhd], extraData.size() - maxExtraDataSizeInMvhd);
            }

            if (AccessExtraDataInVideoTrackTkhdAtom(mp4File, dataInTrak, write))
            {
                if (!write)
                {
                    memcpy(&extraData[maxExtraDataSizeInMvhd], &dataInTrak[0], extraData.size() - maxExtraDataSizeInMvhd);
                }
                
                return true;
            }

            break;
        }
    } while (mp4File.tellg() < moovEndPos && SeekToNextAtom(mp4File));

    return false;
}