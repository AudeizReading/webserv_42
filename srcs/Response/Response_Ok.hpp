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
	Response_Ok(Request const& request, Server const& serv, Location const& location);

	~Response_Ok();
};
