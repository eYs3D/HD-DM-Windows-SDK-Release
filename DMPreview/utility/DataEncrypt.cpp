#include "stdafx.h"
#include "DataEncrypt.h"


CDataEncrypt::CDataEncrypt(BYTE key)
: m_key(key)
{
}

CDataEncrypt::~CDataEncrypt()
{
}

bool CDataEncrypt::Encrypt(const std::vector<BYTE>& input, std::vector<BYTE>& output)
{
    if (input.empty())
    {
        return false;
    }

    output.clear();
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(input[i] ^ m_key);
    }

    return true;
}

bool CDataEncrypt::Decrypt(const std::vector<BYTE>& input, std::vector<BYTE>& output)
{
    return Encrypt(input, output);
}

bool CDataEncrypt::Encrypt(const std::string& input, std::string& output)
{
    std::vector<BYTE> inVector(input.begin(), input.end());
    std::vector<BYTE> outVector;
    if (!Encrypt(inVector, outVector))
    {
        return false;
    }

    std::vector<BYTE> outCharVector;
    for (BYTE data : outVector)
    {
        outCharVector.push_back('A' + ((data >> 4) & 0xf));
        outCharVector.push_back('A' + (data & 0xf));
    }
    outCharVector.push_back('\0');

    output = (char*)&outCharVector[0];
    
    return true;
}

bool CDataEncrypt::Decrypt(const std::string& input, std::string& output)
{
    if (input.size() % 2 != 0)
    {
        return false;
    }

    std::vector<BYTE> inVector;
    for (size_t i = 0; i < input.size(); i += 2)
    {
        inVector.push_back(((input[i] - 'A') << 4) | (input[i + 1] - 'A'));
    }

    std::vector<BYTE> outVector;
    if (!Decrypt(inVector, outVector))
    {
        return false;
    }
    outVector.push_back('\0');

    output = (char*)&outVector[0];

    return true;
}

bool CDataEncrypt::Encrypt(const std::string& input1, const std::string& input2, std::string& output)
{
    if (input1.empty() || input2.empty())
    {
        return false;
    }

    auto& longString = input1.size() > input2.size() ? input1 : input2;
    auto& shortString = input1.size() <= input2.size() ? input1 : input2;

    std::vector<char> disorderBuf(longString.size() * 2, 0);
    for (size_t i = 0, shortStart = (longString.size() - shortString.size()) / 2;
        i < longString.size(); ++i)
    {
        disorderBuf[i * 2] = longString.at(i);
        if (i >= shortStart && i - shortStart < shortString.size())
        {
            disorderBuf[i * 2 + 1] = shortString.at(i - shortStart);
        }
    }

    std::string disorderString;
    for (auto c : disorderBuf)
    {
        if (c != '\0')
        {
            disorderString += c;
        }
    }
    // insert "_<shortStringSize>_" into the disordered string, 
    // if input2 is longer, this value will be negative
    int shortStringSize = (int)shortString.size() * (input1.size() > input2.size() ? 1 : -1);
    disorderString.insert(disorderString.size() / 2, std::string("_") + std::to_string(shortStringSize) + "_");

    return Encrypt(disorderString, output);
}

bool CDataEncrypt::Decrypt(const std::string& input, std::string& output1, std::string& output2)
{
    std::string disorderString;
    if (!Decrypt(input, disorderString) || disorderString.size() < 5)
    {
        return false;
    }
    
    size_t secondTagPos = disorderString.find_first_of('_', disorderString.size() / 2);
    if (secondTagPos == std::string::npos)
    {
        return false;
    }

    size_t firstTagPos = disorderString.find_last_of('_', secondTagPos - 1);
    if (firstTagPos == std::string::npos)
    {
        return false;
    }

    int shortSize = std::stoi(disorderString.substr(firstTagPos + 1, secondTagPos - firstTagPos - 1));
    size_t shortStringSize = (size_t)(shortSize > 0 ? shortSize : -shortSize);
    disorderString.erase(firstTagPos, secondTagPos + 1 - firstTagPos);// remove "_<shortStringSize>_" in the disordered string
    size_t longStringSize = disorderString.size() - shortStringSize;

    std::string longString;
    std::string shortString;
    size_t shortStart = (longStringSize - shortStringSize) / 2 + 1;
    size_t shortEnd = shortStart - 1 + shortStringSize * 2;
    for (size_t i = 0; i < disorderString.size(); ++i)
    {
        if (i < shortStart || i > shortEnd || 
            (i - shortStart) % 2 != 0)
        {
            longString += disorderString.at(i);
        }
        else
        {
            shortString += disorderString.at(i);
        }
    }

    if (shortSize > 0)
    {
        output1 = longString;
        output2 = shortString;
    }
    else
    {
        output1 = shortString;
        output2 = longString;
    }
    
    return true;
}