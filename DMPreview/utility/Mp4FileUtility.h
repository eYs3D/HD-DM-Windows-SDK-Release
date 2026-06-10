#pragma once

int MaxMp4ExtraDataSize();
bool AccessMp4FileExtraData(std::string filename, std::vector<char>& extraData, bool write);