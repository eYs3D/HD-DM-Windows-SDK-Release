#pragma once
#include <experimental/filesystem>
#include <string>
#include <vector>
#include <map>


std::wstring AnsiToUtf16(std::string ansi);
std::string AnsiToUtf8(std::string ansi);
std::string Utf16ToAnsi(std::wstring utf16);
std::string Utf16ToUtf8(std::wstring utf16);
std::string Utf8ToAnsi(std::string utf8);
std::wstring Utf8ToUtf16(std::string utf8);
bool RetrieveStringsFromStringTable(std::experimental::filesystem::path rcFilePath, std::vector<std::string>& stringPool);
void CreateEmptyMultiLangFileInCpprest(std::experimental::filesystem::path filePath, std::vector<std::string>& stringPool);
void CreateEmptyMultiLangFile(std::experimental::filesystem::path filePath, std::vector<std::string>& stringPool);
void UpdateNewStringToMultiLangFiles(std::experimental::filesystem::path folderPath, std::vector<std::string>& stringPool, 
    std::string englishFile = "");// "to" string in english file will be set to the same as "from" string
bool RetrieveMultiLangStringMapping(std::experimental::filesystem::path filePath, std::map<std::string, std::wstring>& stringPool);
bool FlushMultiLangStringMapping(std::experimental::filesystem::path filePath, std::map<std::string, std::wstring>& stringPool);
bool UpdateRefinedStringToRcFile(std::experimental::filesystem::path rcFilePath, std::map<std::string, std::string>& stringPool);
std::string ReplaceJsonEscapeChar(const std::string& str);
std::wstring ReplaceJsonEscapeChar(const std::wstring& str);
void Utf16PoolToAnsiPool(const std::map<std::string, std::wstring>& utf16StringPool,
    std::map<std::string, std::string>& ansiStringPool);