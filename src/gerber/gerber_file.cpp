#include "gerber_file.h"
#include <glog/logging.h>


bool GerberFile::Load(const std::vector<char>& data)
{
	buffer_ = data;
	pointer_ = 0;
	line_number_ = 0;

	return true;
}

bool GerberFile::EndOfFile()
{
	return pointer_ >= buffer_.size();
}

bool GerberFile::SkipWhiteSpace() {
	while (!EndOfFile()) {
		switch (buffer_[pointer_]) {
		case '\n':
			line_number_++;
		case ' ':
		case '\t':
		case '\r':
			pointer_++;
			break;

		default:
			return false;
		}
	}

	return true;
}

char GerberFile::GetChar()
{
	return buffer_[pointer_++];
}

char GerberFile::PeekChar() {
	return buffer_[pointer_];
}

char GerberFile::PeekNextChar() {
	return buffer_[pointer_ + 1];
}

bool GerberFile::QueryCharUntilNotWhiteSpace(char c)
{
	if (SkipWhiteSpace() || PeekChar() != c) {
		return false;
	}

	++pointer_;
	return true;
}

bool GerberFile::QueryCharUntilEnd(char c)
{
	while (!EndOfFile()) {
		if (PeekChar() == c) {
			return true;
		}

		++pointer_;
	}

	return false;
}

bool GerberFile::GetInteger(int& integer) {
	unsigned i = pointer_;

	SkipWhiteSpace();

	bool negative = GetSign();
	integer = 0;
	while (!EndOfFile()) {
		const char cur_char = buffer_[pointer_];
		if (IsNumber(cur_char)) {
			integer *= 10;
			integer += cur_char - '0';
			pointer_++;
		}
		else {
			if (negative) {
				integer *= -1;
			}

			return (pointer_ > i);
		}
	}

	pointer_ = i;
	return false;
}

bool GerberFile::IsNumber(char cur_char)
{
	return cur_char >= '0' && cur_char <= '9';
}

bool GerberFile::GetFloat(double& number) {
	unsigned i = pointer_;

	SkipWhiteSpace();

	bool negative = GetSign();
	int integer = 0;
	while (!EndOfFile()) {
		auto cur_char = buffer_[pointer_];
		if (IsNumber(cur_char)) {
			integer *= 10;
			integer += cur_char - '0';
			pointer_++;
		}
		else {
			break;
		}
	}

	number = integer;
	if (!EndOfFile() && buffer_[pointer_] == '.') {
		pointer_++;

		double scale = 0.1;
		while (!EndOfFile()) {
			auto cur_char = buffer_[pointer_];
			if (IsNumber(cur_char)) {
				number += (cur_char - '0') * scale;
				scale *= 0.1;
				pointer_++;
			}
			else {
				break;
			}
		}
	}

	if (!EndOfFile()) {
		if (negative) {
			number *= -1.0;
		}

		return (pointer_ > i);
	}

	return false;
}

bool GerberFile::GetSign()
{
	if (EndOfFile()) {
		return false;
	}

	if (buffer_[pointer_] == '-') {
		pointer_++;
		return true;
	}

	if (buffer_[pointer_] == '+') {
		pointer_++;
	}

	return false;
}

bool GerberFile::GetCoordinate(double& number, int integer, int decimal, bool omit_trailing_zeroes) {
	unsigned i = pointer_;

	SkipWhiteSpace();

	auto sign = GetSign();
	number = 0;
	int n = 0;
	while (!EndOfFile()) {
		auto cur_char = buffer_[pointer_];
		if (IsNumber(cur_char)) {
			number *= 10;
			number += cur_char - '0';
			pointer_++;
			n++;
		}
		else if (cur_char == '.') {
			pointer_ = i;
			return GetFloat(number);
		}
		else {
			if (sign) {
				number *= -1;
			}

			if (omit_trailing_zeroes) {
				for (int j = 0; j < (integer + decimal - n); j++) {
					number *= 10;
				}
			}

			for (int j = 0; j < decimal; j++) {
				number /= 10;
			}

			if (!n) {
				LOG(WARNING) << "Line " << line_number_ << " - Warning: Ignoring ill-formed coordinate";
			}

			return true;
		}
	}

	return false;
}

bool GerberFile::GetString(std::string& str) {
	while (!EndOfFile()) {
		SkipWhiteSpace();

		switch (PeekChar()) {
		case 0:
			LOG(ERROR) << "Line " << line_number_ << " - Error: Null in name not allowed";
			return false;
		case '*':
		case ',':
			return true;
		default:
			str.push_back(GetChar());
			break;
		}
	}

	return false;
}
