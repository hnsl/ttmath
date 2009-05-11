/*
 * This file is a part of TTMath Bignum Library
 * and is distributed under the PNG licence.
 * Author: Christian Kaiser <chk@online.de>
 */

/* 
	Copyright (c) 2009 Christian Kaiser

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.

		2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		distribution.
 */

#ifndef headerfilettmathmathttconfig
#define headerfilettmathmathttconfig
#pragma once

#include <sstream>

namespace ttmath
{

#if defined(_MSC_VER)
	#if defined(_UNICODE)
		typedef	wchar_t					tchar_t;
		typedef	std::wstring			tstr_t;
		typedef std::wostringstream		tostrstrm_t;
		typedef std::wostream			tostrm_t;
		typedef std::wistream			tistrm_t;
	#else
		typedef	char					tchar_t;
		typedef	std::string				tstr_t;
		typedef std::ostringstream		tostrstrm_t;
		typedef std::ostream			tostrm_t;
		typedef std::istream			tistrm_t;
	#endif
	
	#if defined(_UNICODE)
		#define __TEXT(quote) 	L ## quote
	  #else
		#define __TEXT(quote) 	quote
	#endif
	#define TTMATH_TEXT(quote) 	__TEXT(quote)
	
	#if defined(_MT)
		class 	clsCrit
			{
			private:
				mutable CRITICAL_SECTION 		_Crit;

												clsCrit(const clsCrit&) // inhibit copy (easy mistake to do; use clsCritObj instead!!!)
													{
													}
				clsCrit&						operator=(const clsCrit& rhs); // inhibit assignment
			public:
												clsCrit(void)
													{
													::InitializeCriticalSection(&_Crit);
													}
				virtual							~clsCrit(void)
													{
													::DeleteCriticalSection(&_Crit);
													}

				void							Enter(void) const
													{
													::EnterCriticalSection(&_Crit);
													}
				void							Leave(void) const
													{
													::LeaveCriticalSection(&_Crit);
													}
			};

		class 	clsCritObj
			{
			private:
				const clsCrit&					_Crit;
				
				clsCritObj&						operator=(const clsCritObj& rhs); // not applicable
			public:
												clsCritObj(const clsCrit& Sync)
													: _Crit(Sync)
													{
													_Crit.Enter();
													}
												~clsCritObj(void)
													{
													_Crit.Leave();
													}
			};
		#define TTMATH_IMPLEMENT_THREADSAFE_OBJ					\
			private:											\
				clsCrit CritSect;								\
			public:												\
				operator clsCrit&()								\
				{												\
					return(CritSect);							\
				}
		#define TTMATH_USE_THREADSAFE_OBJ(c)	clsCritObj	lock(c)
	#endif
#else // not MS compiler
	typedef	char					tchar_t;
	typedef	std::string				tstr_t;
	typedef std::ostringstream		tostrstrm_t;
	typedef std::ostream			tostrm_t;
	typedef std::istream			tistrm_t;
#endif

#if !defined(TTMATH_IMPLEMENT_THREADSAFE_OBJ)
	#define TTMATH_IMPLEMENT_THREADSAFE_OBJ		/* */
	#define TTMATH_USE_THREADSAFE_OBJ(c)		/* */
#endif

} // namespace

#endif // headerfilettmathmathttconfig
