#pragma once

#include <stdexcept>

namespace Vision
{
	public class VisionRuntimeException : public std::runtime_error
	{
	public:
		explicit VisionRuntimeException( const std::string& error_message ) :
			std::runtime_error(error_message), _errstr(error_message) {}
 	private:
		std::string _errstr;
 	};
}
