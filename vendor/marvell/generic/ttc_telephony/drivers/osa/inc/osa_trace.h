/*--------------------------------------------------------------------*/
// OSA trace header file
/*--------------------------------------------------------------------*/

#ifndef _OSA_TRACE_H_
#define _OSA_TRACE_H_

/*--------------------------------------------------------------------*/

#ifdef ENV_SYMBIAN

	#ifndef OSA_DEBUG

		#define OSA_TRACE(fmt)
		#define OSA_TRACE1(fmt, val1)
		#define OSA_TRACE2(fmt, val1, val2)
		#define OSA_TRACE3(fmt, val1, val2, val3)
		#define OSA_TRACE4(fmt, val1, val2, val3, val4)

// Custom assert macro

		#define OSA_ASSERT(cond)

	#else

		#define OSA_TRACE(fmt)	\
	RDebug::Print(_L(fmt))

		#define OSA_TRACE1(fmt, val1) \
	RDebug::Print(_L(fmt), val1)

		#define OSA_TRACE2(fmt, val1, val2) \
	RDebug::Print(_L(fmt), val1, val2)

		#define OSA_TRACE3(fmt, val1, val2, val3) \
	RDebug::Print(_L(fmt), val1, val2, val3)

		#define OSA_TRACE4(fmt, val1, val2, val3, val4)	\
	RDebug::Print(_L(fmt), val1, val2, val3, val4)

// Custom assert macro

		#define OSA_ASSERT(cond)																\
																								\
	if (cond == 0)																		    \
	{																				    \
		RDebug::Print(_L("*** OSA assert ***, line %d in %s" ), __LINE__, __FILE__);	\
		while (1) {}																	\
	}

	#endif

#endif  // ENV_SYMBIAN

#endif  // _OSA_TRACE_H_





