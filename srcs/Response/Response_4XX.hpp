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
			_Name(Request const& request); \
			~_Name(); \
		};

_PROTO_RESPONSE(Response_Bad_Request)
_PROTO_RESPONSE(Response_Forbidden)
_PROTO_RESPONSE(Response_Not_Found)
_PROTO_RESPONSE(Response_Method_Not_Allowed)
_PROTO_RESPONSE(Response_Internal_Server_Error)
