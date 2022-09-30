/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */
/*  */

#pragma once

class Request
{
private:
	int				_fd;
	int				_complete;
	std::string		_plaintext;


	void _read_buffer();
	void _parse();

public:
	Request(int fd);

	~Request();

	int is_complete() const;
};
