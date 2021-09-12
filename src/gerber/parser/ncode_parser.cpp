#include "ncode_parser.h"
#include "gerber.h"
#include <iostream>


NCodeParser::NCodeParser(Gerber& gerber) :gerber_(gerber) {

}

bool NCodeParser::Run() {
	gerber_.start_of_level_ = false;
    std::cout << "Line " << gerber_.gerber_file_.line_number_ << " - Error: N Code not implemented" << std::endl;
	return false;
}

bool NCodeParser::EndOfFile()
{
	return false;
}
