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

#define _PROTO_RESPONSE(_Name) \
		class _Name: public Response \
		{ \
		protected: \
			virtual void _init(); \
		public: \
			_Name(Request const& request, Server const& server); \
			~_Name(); \
		};

_PROTO_RESPONSE(Response_Bad_Request)
_PROTO_RESPONSE(Response_Forbidden)
_PROTO_RESPONSE(Response_Not_Found)
