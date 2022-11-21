#include "Buffer.hpp"

Buffer::Buffer(const char* buffer, const size_t buffer_size) : _raw(buffer), _obfuscated(buffer, buffer_size), _raw_size(buffer_size)
{
}

Buffer::Buffer(const Buffer &src) : _raw(src.get_raw()), _obfuscated(src.get_raw(), src.get_raw_size()), _raw_size(src.get_raw_size())
{
}

Buffer::~Buffer(void) {
}

Buffer&		Buffer::operator=(const Buffer &src) 
{
	if (this != &src) 
	{
		(*this) = src;
	}
	return (*this);
}

const char*			Buffer::get_raw()		const { return this->_raw; }
const std::string	Buffer::get_obf()		const { return this->_obfuscated; }
size_t				Buffer::get_raw_size()	const { return this->_raw_size; }
size_t				Buffer::get_obf_size()	const { return this->_obfuscated.length(); }

void				Buffer::print_raw()		const 
{
	std::cerr << "[Buffer]: raw [\033[33m" << this->get_raw() << "\033[0m] end raw buffer\n";
}
void				Buffer::print_raw(const size_t size)		const 
{
	std::cerr << "[Buffer]: raw [\033[33m";
	for (size_t i = 0; i < size; ++i)
	{
		std::cerr << this->_raw[i];
	}
	std::cerr << "\033[0m] end raw buffer\n";
}


void				Buffer::print_obf()		const 
{
	std::cerr << "[Buffer]: obfuscated [\033[33;1m" << this->get_obf() << "\033[0m] end obfuscated buffer\n";
}

void				Buffer::print_raw_to_int(const size_t content_length)	const 
{
	unsigned char	*buffer = this->get_body(content_length);
	if (!buffer)
		return;

	for (size_t i = 0; i < content_length; ++i)
	{
		int	octet_value = static_cast<int>(buffer[i]);
		int	next_octet_value = 0;
		((i + 1) < content_length) && (next_octet_value = static_cast<int>(buffer[i + 1]));
		if (octet_value == 255)
		{
			if (next_octet_value == 217)
				std::cerr << "\033[31mEND JPEG SEQUENCE\n";
		}
		std::cerr << "[" << octet_value << "] ";
	}
	std::cerr << "\n";
	free(buffer);
}

unsigned char*	Buffer::get_body(const size_t content_length) const
{
	unsigned long 	start_body_obf = this->_obfuscated.find("\n\r\n") + 3;
	if (start_body_obf == std::string::npos)
	{
		std::cerr << "There is nothing inside the \n";
		return 0;
	}

	size_t			size_body = PIPE_BUF - start_body_obf;
	unsigned char	*body = 0;

	if (size_body > 0 && content_length > 0)
	{
		body = static_cast<unsigned char*>(malloc(sizeof(*body) * (size_body)));

		if (!body)
			return 0;
		::bzero(body, size_body);
		for (size_t i = 0; i < size_body; ++i)
		{
			body[i] = this->_raw[start_body_obf + i];
		}
	}
	return body;
}

bool		Buffer::has_post_request(const std::string& method) const
{
	if (this->_obfuscated.find("\n\r\n") != std::string::npos 
			&& method == "POST")
	{
		return true;
	}
	return false;
}

bool		Buffer::has_upload_request(Request::map_ss& header, const std::string& method) const
{
	//Request::map_ss				header = req.get_header();
	
	std::string					boundary = header["Content-Type"];
	//std::string					query = req.get_query();

	if (this->has_post_request(method) 
			&& boundary.find("boundary=") != std::string::npos)
	{
		return true;
	}
	return false;
}
