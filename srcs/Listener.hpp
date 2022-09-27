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

class Listener
{
private:
	int				fd;

public:
	Listener();
	Listener(Listener const &src);

	~Listener();

	Listener	&operator=(Listener const &src);
};
