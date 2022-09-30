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

#include "../Response.hpp"

class Response_Ok: public Response
{
private:

protected:
	virtual void _init();

public:
	Response_Ok(Request &request, Server &server);

	~Response_Ok();
};
