#pragma once
#include <string>
#include <vector>

class GerberFile {
public:
	std::vector<char> buffer_;

	unsigned pointer_{ 0 };
	unsigned line_number_{ 1 };

	bool Load(const std::vector<char>& data);

	bool EndOfFile();
	bool SkipWhiteSpace();

	char GetChar();
	char PeekChar();
	char PeekNextChar();

	bool QueryCharUntilNotWhiteSpace(char c);
	bool QueryCharUntilEnd(char c);

	bool GetString(std::string& str);

	bool GetInteger(int& integer);
	bool GetFloat(double& number);
	bool GetCoordinate(double& number, int integer, int decimal, bool omit_trailing_zeroes);

private:
	bool IsNumber(char cur_char);
	bool GetSign();
};
