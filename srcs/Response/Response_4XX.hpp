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

class Response_Bad_Request: public Response
{
private:

protected:
	virtual void _init();

public:
	Response_Bad_Request(Request &request, Server &server);

	~Response_Bad_Request();
};
