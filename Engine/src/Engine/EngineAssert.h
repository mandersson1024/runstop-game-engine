#pragma once
#include "Logger.h"

#ifdef NDEBUG
	#define ENGINE_ASSERT(expression, reason)
#else
#define ENGINE_ASSERT(expression, reason) { if(!(expression)) \
		{ \
			ENGINE_LOG_DEBUG_ERROR("Assertion Failed: ({}), \'{}\'", #expression, reason); \
			 __debugbreak(); \
		} }
#endif
