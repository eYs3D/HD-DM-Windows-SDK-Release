#include "stdafx.h"
#include "MultiLangUtility.h"
#include <fstream>
#include <streambuf>
#include <codecvt>
#include <cpprest/json.h>
#include <iterator>

const std::vector<char> utf8Bom = { (char)0xEF, (char)0xBB, (char)0xBF };
const std::string multiLangExt(".lang");
const std::string stringTableTag("STRINGTABLE");
const std::string mfcFieldBegin("BEGIN");
const std::string mfcFieldEnd("END");


bool IsUtf8Bom(const char* data)
{
    for (size_t i = 0; i < utf8Bom.size(); ++i)
    {
        if (data[i] != utf8Bom[i])
        {
            return false;
        }
    }

    return true;
}

void WriteUtf8Bom(std::ofstream& file)
{
    for (char ch : utf8Bom)
    {
        file << ch;
    }
}

bool FindAndMoveToStringTable(std::ifstream& rcFile)
{
    std::string lineText;
    while (std::getline(rcFile, lineText))
    {
        if (lineText == stringTableTag)
        {
            if (std::getline(rcFile, lineText))
            {
                if (lineText == mfcFieldBegin)
                {
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

bool RetrieveStrings(std::ifstream& rcFile, std::vector<std::string>& str)
{
    str.clear();
    std::string lineText;
    while (std::getline(rcFile, lineText) && lineText != mfcFieldEnd)
    {
        size_t start = lineText.find('\"');
        if (start != std::string::npos)
        {
            ++start;
            size_t end = lineText.find_last_of('\"');
            if (end != std::string::npos && end > start)
            {
                str.push_back(lineText.substr(start, end - start));
            }
        }
    }

    return !str.empty();
}

bool RetrieveStringsFromStringTable(std::experimental::filesystem::path rcFilePath, std::vector<std::string>& stringPool)
{
    std::ifstream rcFile(rcFilePath.string());
    if (!rcFile.good())
    {
        return false;
    }

    stringPool.clear();
    while (FindAndMoveToStringTable(rcFile))
    {
        std::vector<std::string> strings;
        if (RetrieveStrings(rcFile, strings) && 
            !strings.empty())
        {
            stringPool.insert(stringPool.end(), strings.begin(), strings.end());
        }
    }

    return !stringPool.empty();
}

std::wstring AnsiToUtf16(std::string ansi)
{
    int utf16Size = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), (int)ansi.size(), nullptr, 0);
    std::wstring utf16(utf16Size, 0);
    MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, (wchar_t*)utf16.c_str(), utf16Size);

    return utf16;
}

std::string AnsiToUtf8(std::string ansi)
{
    return Utf16ToUtf8(AnsiToUtf16(ansi));
}

std::string Utf16ToAnsi(std::wstring utf16)
{
    int ansiSize = WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), (int)utf16.size(), nullptr, 0, nullptr, nullptr);
    std::string ansi(ansiSize, 0);
    WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), (int)utf16.size(), (char*)ansi.c_str(), ansiSize, nullptr, nullptr);

    return ansi;
}

std::string Utf16ToUtf8(std::wstring utf16)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(utf16);
}

std::string Utf8ToAnsi(std::string utf8)
{
    return Utf16ToAnsi(Utf8ToUtf16(utf8));
}

std::wstring Utf8ToUtf16(std::string utf8)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(utf8);
}

// this version generate only one line in the file, hard to read by people, 
// but all file text content is generated by cpprest lib, and it handle escape characters automatically.
void CreateEmptyMultiLangFileInCpprest(std::experimental::filesystem::path filePath, std::vector<std::string>& stringPool)
{
    std::vector<web::json::value> strArray;
    for (auto& str : stringPool)
    {
        web::json::value strObj;
        strObj[L"from"] = web::json::value::string(AnsiToUtf16(str));
        strObj[L"to"] = web::json::value::string(L"");
        strArray.push_back(strObj);
    }

    web::json::value jsonObj;
    jsonObj[L"StringMap"] = web::json::value::array(strArray);
    std::string utf8Content = Utf16ToUtf8(jsonObj.serialize());

    std::ofstream file(filePath);
    if (file.good())
    {
        WriteUtf8Bom(file);
        file << utf8Content.c_str();
    }
}

std::string ReplaceJsonEscapeChar(const std::string& str)
{
    std::string ret;
    for (auto ch : str)
    {
        switch (ch)
        {
        case '\"': ret += "\\\""; break;
        case '\\': ret += "\\\\"; break;
        //case '/': ret += "\\/"; break; // in spec, '/' is a escape character, but cpprest lib seems not handle it
        case '\b': ret += "\\b"; break;
        case '\f': ret += "\\f"; break;
        case '\n': ret += "\\n"; break;
        case '\r': ret += "\\r"; break;
        case '\t': ret += "\\t"; break;
        default: ret += ch;
        }
    }

    return ret;
}

std::wstring ReplaceJsonEscapeChar(const std::wstring& str)
{
    std::wstring ret;
    for (auto ch : str)
    {
        switch (ch)
        {
        case L'\"': ret += L"\\\""; break;
        case L'\\': ret += L"\\\\"; break;
        //case L'/': ret += L"\\/"; break; // in spec, '/' is a escape character, but cpprest lib seems not handle it
        case L'\b': ret += L"\\b"; break;
        case L'\f': ret += L"\\f"; break;
        case L'\n': ret += L"\\n"; break;
        case L'\r': ret += L"\\r"; break;
        case L'\t': ret += L"\\t"; break;
        default: ret += ch;
        }
    }

    return ret;
}

void CreateEmptyMultiLangFile(std::experimental::filesystem::path filePath, std::vector<std::string>& stringPool)
{
    std::ofstream file(filePath, std::ios::out);
    if (file.good())
    {
        WriteUtf8Bom(file);
        file << u8"{ \"StringMap\" : [" << std::endl;
        for (size_t i = 0, size = stringPool.size(); i < size; ++i)
        {
            web::json::value jsonStr = web::json::value::string(AnsiToUtf16(ReplaceJsonEscapeChar(stringPool[i])));
            file << u8"{" << std::endl
                << u8"\"from\" : \"" << Utf16ToUtf8(jsonStr.as_string()).c_str() << u8"\"," << std::endl
                << u8"\"to\" : \"\"" << std::endl
                << (i + 1 != size ? u8"}," : u8"}") << std::endl;
        }
        file << u8"] }";
    }
}

void UpdateNewStringToMultiLangFiles(std::experimental::filesystem::path folderPath, std::vector<std::string>& stringPool, 
    std::string englishFile)
{
    for (auto iter : std::experimental::filesystem::directory_iterator(folderPath))
    {
        if (!std::experimental::filesystem::is_directory(iter.path()) && 
            iter.path().extension().string() == multiLangExt)
        {
            bool changed = false;
            std::map<std::string, std::wstring> stringsInFile;
            if (RetrieveMultiLangStringMapping(iter.path(), stringsInFile))
            {
                for (auto& str : stringPool)
                {
                    if (stringsInFile.find(str) == stringsInFile.end())
                    {
                        if (!englishFile.empty() && iter.path().filename().string() == englishFile)
                        {
                            stringsInFile[str] = AnsiToUtf16(str);
                        }
                        else
                        {
                            stringsInFile[str] = L"";
                        }

                        changed = true;
                    }
                }
            }

            if (changed)
            {
                FlushMultiLangStringMapping(iter.path(), stringsInFile);
            }
        }
    }
}

bool RetrieveMultiLangStringMapping(std::experimental::filesystem::path filePath, std::map<std::string, std::wstring>& stringPool)
{
    stringPool.clear();

    std::ifstream file(filePath.string());
    if (file.good())
    {
        //remove utf-8 bom (byte order mark)
        char firstChars[3] = { (char)file.get(), (char)file.get(), (char)file.get() };
        if (!IsUtf8Bom(firstChars))
        {
            file.seekg(0);
        }

        std::error_code errCode;
        web::json::value contentInJson = web::json::value::parse(file, errCode);
        if (errCode.value() > 0)
        {
            auto eValue = errCode.value();
            auto eMsg = errCode.message();
            std::ostringstream msg;
            msg << "parsing json format file(" << filePath.string().c_str() << ") failed. error code: " << eValue << " (" << eMsg << ")." << std::endl;
            OutputDebugStringA(msg.str().c_str());

            return false;
        }

        auto iter = contentInJson.as_object().find(L"StringMap");
        if (iter != contentInJson.as_object().cend())
        {
            for (auto& strObj : iter->second.as_array())
            {
                auto from = Utf16ToAnsi(strObj.as_object().find(L"from")->second.as_string());
                auto to = strObj.as_object().find(L"to")->second.as_string();
                if (!to.empty())
                {
                    stringPool[from] = to;
                }
            }

            return true;
        }
    }
    
    return false;
}

bool FlushMultiLangStringMapping(std::experimental::filesystem::path filePath, std::map<std::string, std::wstring>& stringPool)
{
    std::ofstream file(filePath, std::ios::out);
    if (file.good())
    {
        WriteUtf8Bom(file);
        file << u8"{ \"StringMap\" : [" << std::endl;
        size_t poolSize = stringPool.size();
        for (auto strMap : stringPool)
        {
            --poolSize;
            web::json::value jsonFrom = web::json::value::string(AnsiToUtf16(ReplaceJsonEscapeChar(strMap.first)));
            web::json::value jsonTo = web::json::value::string(ReplaceJsonEscapeChar(strMap.second));
            file << u8"{" << std::endl
                << u8"\"from\" : \"" << Utf16ToUtf8(jsonFrom.as_string()).c_str() << u8"\"," << std::endl
                << u8"\"to\" : \"" << Utf16ToUtf8(jsonTo.as_string()).c_str() << u8"\"" << std::endl
                << (poolSize != 0 ? u8"}," : u8"}") << std::endl;
        }
        file << u8"] }";

        return true;
    }

    return false;
}

void ReplaceStrings(std::map<std::string, std::string>& stringPool, std::ifstream& rcFile, 
    std::ostringstream& newRcContent)
{
    std::string lineText;
    while (std::getline(rcFile, lineText) && lineText != mfcFieldEnd)
    {
        size_t start = lineText.find('\"');
        if (start != std::string::npos)
        {
            ++start;
            size_t end = lineText.find_last_of('\"');
            if (end != std::string::npos && end > start)
            {
                newRcContent << lineText.substr(0, start);

                std::string str = lineText.substr(start, end - start);
                auto iter = stringPool.find(str);
                if (iter != stringPool.end() && !iter->second.empty())
                {
                    newRcContent << iter->second.c_str();
                }
                else
                {
                    newRcContent << str.c_str();
                }

                newRcContent << lineText.substr(end) << std::endl;
            }
        }
        else
        {
            newRcContent << lineText << std::endl;
        }
    }
    newRcContent << lineText << std::endl;// write mfcFieldEnd
}

bool UpdateRefinedStringToRcFile(std::experimental::filesystem::path rcFilePath, std::map<std::string, std::string>& stringPool)
{
    std::ostringstream newRcContent;
    {
        std::ifstream rcFile(rcFilePath.string());
        if (!rcFile.good())
        {
            return false;
        }

        std::string lineText;
        while (std::getline(rcFile, lineText))
        {
            newRcContent << lineText << std::endl;
            if (lineText == stringTableTag)
            {
                if (std::getline(rcFile, lineText))
                {
                    newRcContent << lineText << std::endl;
                    if (lineText == mfcFieldBegin)
                    {
                        ReplaceStrings(stringPool, rcFile, newRcContent);
                    }
                }
            }
        }
    }

    std::ofstream newRcFile(rcFilePath.string());
    if (newRcFile.good())
    {
        newRcFile << newRcContent.str().c_str();
        return true;
    }

    return false;
}

void Utf16PoolToAnsiPool(const std::map<std::string, std::wstring>& utf16StringPool,
    std::map<std::string, std::string>& ansiStringPool)
{
    ansiStringPool.clear();
    std::transform(utf16StringPool.cbegin(), utf16StringPool.cend(), std::inserter(ansiStringPool, ansiStringPool.end()),
        [](const std::pair<std::string, std::wstring>& iter)
    {
        return std::pair<std::string, std::string>(iter.first, Utf16ToAnsi(iter.second));
    });
}