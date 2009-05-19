/*
 * This file is a part of TTMath Bignum Library
 * and is distributed under the (new) BSD licence.
 * Author: Tomasz Sowa <t.sowa@slimaczek.pl>
 */

/* 
 * Copyright (c) 2006-2009, Tomasz Sowa
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *    
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *    
 *  * Neither the name Tomasz Sowa nor the names of contributors to this
 *    project may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */



#ifndef headerfilettmathuint
#define headerfilettmathuint

/*!
	\file ttmathuint.h
    \brief template class UInt<uint>
*/

#include <iostream>
#include <iomanip>

#include "ttmathtypes.h"

#if defined(_MSC_VER)
	#pragma warning(disable:4127) // conditional expression is constant
#endif

/*!
    \brief a namespace for the TTMath library
*/
namespace ttmath
{

/*! 
	\brief UInt implements a big integer value without a sign

	value_size - how many bytes specify our value
		on 32bit platforms: value_size=1 -> 4 bytes -> 32 bits
		on 64bit platforms: value_size=1 -> 8 bytes -> 64 bits
	value_size = 1,2,3,4,5,6....
*/
template<uint value_size>
class UInt
{
public:

	/*!
		buffer for the integer value
		  table[0] - the lowest word of the value
	*/
	uint table[value_size];


	/*!
		this method is only for debugging purposes or when we want to make
		a table of a variable (constant) in ttmathbig.h

		it prints the table in a nice form of several columns
	*/
	void PrintTable(tostrm_t & output) const
	{
		// how many columns there'll be
		const int columns = 8;

		int c = 1;
		for(int i=value_size-1 ; i>=0 ; --i)
		{
			output	<< "0x" << std::setfill('0');
			
			#ifdef TTMATH_PLATFORM32
				output << std::setw(8);
			#else
				output << std::setw(16);
			#endif
				
			output << std::hex << table[i];
			
			if( i>0 )
			{
				output << ", ";		
			
				if( ++c > columns )
				{
					output << std::endl;
					c = 1;
				}
			}
		}
		
		output << std::dec << std::endl;
	}


	void PrintLog(const tchar_t * msg, tostrm_t & output) const
	{
		output << msg << std::endl;

		for(uint i=0 ; i<value_size ; ++i)
			output << " table[" << i << "]: " << table[i] << std::endl;
	}


	/*!
		this method returns the size of the table
	*/
	uint Size() const
	{
		return value_size;
	}


	/*!
		this method sets zero
	*/
	void SetZero()
	{
		// in the future here can be 'memset'

		for(uint i=0 ; i<value_size ; ++i)
			table[i] = 0;

		TTMATH_LOG("UInt::SetZero")
	}


	/*!
		this method sets one
	*/
	void SetOne()
	{
		SetZero();
		table[0] = 1;

		TTMATH_LOG("UInt::SetOne")
	}


	/*!
		this method sets the max value which this class can hold
		(all bits will be one)
	*/
	void SetMax()
	{
		for(uint i=0 ; i<value_size ; ++i)
			table[i] = TTMATH_UINT_MAX_VALUE;

		TTMATH_LOG("UInt::SetMax")
	}


	/*!
		this method sets the min value which this class can hold
		(for an unsigned integer value the zero is the smallest value)
	*/
	void SetMin()
	{
		SetZero();

		TTMATH_LOG("UInt::SetMin")
	}



#ifdef TTMATH_PLATFORM32

	/*!
		this method copies the value stored in an another table
		(warning: first values in temp_table are the highest words -- it's different
		from our table)

		we copy as many words as it is possible
		
		if temp_table_len is bigger than value_size we'll try to round 
		the lowest word from table depending on the last not used bit in temp_table
		(this rounding isn't a perfect rounding -- look at the description below)

		and if temp_table_len is smaller than value_size we'll clear the rest words
		in the table
	*/
	void SetFromTable(const uint * temp_table, uint temp_table_len)
	{
		uint temp_table_index = 0;
		sint i; // 'i' with a sign

		for(i=value_size-1 ; i>=0 && temp_table_index<temp_table_len; --i, ++temp_table_index)
			table[i] = temp_table[ temp_table_index ];


		// rounding mantissa
		if( temp_table_index < temp_table_len )
		{
			if( (temp_table[temp_table_index] & TTMATH_UINT_HIGHEST_BIT) != 0 )
			{
				/*
					very simply rounding
					if the bit from not used last word from temp_table is set to one
					we're rouding the lowest word in the table

					in fact there should be a normal addition but
					we don't use Add() or AddTwoInts() because these methods 
					can set a carry and then there'll be a small problem
					for optimization
				*/
				if( table[0] != TTMATH_UINT_MAX_VALUE )
					++table[0];
			}
		}

		// cleaning the rest of the mantissa
		for( ; i>=0 ; --i)
			table[i] = 0;


		TTMATH_LOG("UInt32::SetFromTable")
	}

#endif


#ifdef TTMATH_PLATFORM64
	/*!
		this method copies the value stored in an another table
		(warning: first values in temp_table are the highest words -- it's different
		from our table)

		***this method is created only on a 64bit platform***

		we copy as many words as it is possible
		
		if temp_table_len is bigger than value_size we'll try to round 
		the lowest word from table depending on the last not used bit in temp_table
		(this rounding isn't a perfect rounding -- look at the description below)

		and if temp_table_len is smaller than value_size we'll clear the rest words
		in the table

		warning: we're using 'temp_table' as a pointer at 32bit words
	*/
	void SetFromTable(const unsigned int * temp_table, uint temp_table_len)
	{
		uint temp_table_index = 0;
		sint i; // 'i' with a sign

		for(i=value_size-1 ; i>=0 && temp_table_index<temp_table_len; --i, ++temp_table_index)
		{
			table[i] = uint(temp_table[ temp_table_index ]) << 32;

			++temp_table_index;

			if( temp_table_index<temp_table_len )
				table[i] |= temp_table[ temp_table_index ];
		}


		// rounding mantissa
		if( temp_table_index < temp_table_len )
		{
			if( (temp_table[temp_table_index] & TTMATH_UINT_HIGHEST_BIT) != 0 )
			{
				/*
					very simply rounding
					if the bit from not used last word from temp_table is set to one
					we're rouding the lowest word in the table

					in fact there should be a normal addition but
					we don't use Add() or AddTwoInts() because these methods 
					can set a carry and then there'll be a small problem
					for optimization
				*/
				if( table[0] != TTMATH_UINT_MAX_VALUE )
					++table[0];
			}
		}

		// cleaning the rest of the mantissa
		for( ; i >= 0 ; --i)
			table[i] = 0;

		TTMATH_LOG("UInt64::SetFromTable")
	}

#endif





	/*!
	*
	*	basic mathematic functions
	*
	*/




	/*!
		this method adds one to the existing value
	*/
	uint AddOne()
	{
		return AddInt(1);
	}


	/*!
		this method subtracts one from the existing value
	*/
	uint SubOne()
	{
		return SubInt(1);
	}


private:


	/*!    
		an auxiliary method for moving bits into the left hand side

		this method moves only words
	*/
	void RclMoveAllWords(uint & rest_bits, uint & last_c, uint bits, uint c)
	{
		rest_bits      = bits % TTMATH_BITS_PER_UINT;
		uint all_words = bits / TTMATH_BITS_PER_UINT;
		uint mask      = ( c ) ? TTMATH_UINT_MAX_VALUE : 0;


		if( all_words >= value_size )
		{
			if( all_words == value_size && rest_bits == 0 )
				last_c = table[0] & 1;
			// else: last_c is default set to 0

			// clearing
			for(uint i = 0 ; i<value_size ; ++i)
				table[i] = mask;

			rest_bits = 0;
		}
		else
		if( all_words > 0 )  
		{
			// 0 < all_words < value_size
	
			sint first, second;
			last_c = table[value_size - all_words] & 1; // all_words is greater than 0

			// copying the first part of the value
			for(first = value_size-1, second=first-all_words ; second>=0 ; --first, --second)
				table[first] = table[second];

			// setting the rest to 'c'
			for( ; first>=0 ; --first )
				table[first] = mask;
		}

		TTMATH_LOG("UInt::RclMoveAllWords")
	}
	
public:

	/*!
		moving all bits into the left side 'bits' times
		return value <- this <- C

		bits is from a range of <0, man * TTMATH_BITS_PER_UINT>
		or it can be even bigger then all bits will be set to 'c'

		the value c will be set into the lowest bits
		and the method returns state of the last moved bit
	*/
	uint Rcl(uint bits, uint c=0)
	{
	uint last_c    = 0;
	uint rest_bits = bits;

		if( bits == 0 )
			return 0;

		if( bits >= TTMATH_BITS_PER_UINT )
			RclMoveAllWords(rest_bits, last_c, bits, c);

		if( rest_bits == 0 )
		{
			TTMATH_LOG("UInt::Rcl")
			return last_c;
		}

		// rest_bits is from 1 to TTMATH_BITS_PER_UINT-1 now
		if( rest_bits == 1 )
		{
			last_c = Rcl2_one(c);
		}
		else if( rest_bits == 2 )
		{
			// performance tests showed that for rest_bits==2 it's better to use Rcl2_one twice instead of Rcl2(2,c)
			Rcl2_one(c);
			last_c = Rcl2_one(c);
		}
		else
		{
			last_c = Rcl2(rest_bits, c);
		}

		TTMATH_LOG("UInt::Rcl")

	return last_c;
	}

private:

	/*!    
		an auxiliary method for moving bits into the right hand side

		this method moves only words
	*/
	void RcrMoveAllWords(uint & rest_bits, uint & last_c, uint bits, uint c)
	{
		rest_bits      = bits % TTMATH_BITS_PER_UINT;
		uint all_words = bits / TTMATH_BITS_PER_UINT;
		uint mask      = ( c ) ? TTMATH_UINT_MAX_VALUE : 0;


		if( all_words >= value_size )
		{
			if( all_words == value_size && rest_bits == 0 )
				last_c = (table[value_size-1] & TTMATH_UINT_HIGHEST_BIT) ? 1 : 0;
			// else: last_c is default set to 0

			// clearing
			for(uint i = 0 ; i<value_size ; ++i)
				table[i] = mask;

			rest_bits = 0;
		}
		else if( all_words > 0 )
		{
			// 0 < all_words < value_size

			uint first, second;
			last_c = (table[all_words - 1] & TTMATH_UINT_HIGHEST_BIT) ? 1 : 0; // all_words is > 0

			// copying the first part of the value
			for(first=0, second=all_words ; second<value_size ; ++first, ++second)
				table[first] = table[second];

			// setting the rest to 'c'
			for( ; first<value_size ; ++first )
				table[first] = mask;
		}

		TTMATH_LOG("UInt::RcrMoveAllWords")
	}

public:

	/*!
		moving all bits into the right side 'bits' times
		c -> this -> return value

		bits is from a range of <0, man * TTMATH_BITS_PER_UINT>
		or it can be even bigger then all bits will be set to 'c'

		the value c will be set into the highest bits
		and the method returns state of the last moved bit
	*/
	uint Rcr(uint bits, uint c=0)
	{
	uint last_c    = 0;
	uint rest_bits = bits;
	
		if( bits == 0 )
			return 0;

		if( bits >= TTMATH_BITS_PER_UINT )
			RcrMoveAllWords(rest_bits, last_c, bits, c);

		if( rest_bits == 0 )
		{
			TTMATH_LOG("UInt::Rcr")
			return last_c;
		}

		// rest_bits is from 1 to TTMATH_BITS_PER_UINT-1 now
		if( rest_bits == 1 )
		{
			last_c = Rcr2_one(c);
		}
		else if( rest_bits == 2 )
		{
			// performance tests showed that for rest_bits==2 it's better to use Rcr2_one twice instead of Rcr2(2,c)
			Rcr2_one(c);
			last_c = Rcr2_one(c);
		}
		else
		{
			last_c = Rcr2(rest_bits, c);
		}

		TTMATH_LOG("UInt::Rcr")

	return last_c;
	}


	/*!
		this method moves all bits into the left side
		(it returns value how many bits have been moved)
	*/
	uint CompensationToLeft()
	{
		uint moving = 0;

		// a - index a last word which is different from zero
		sint a;
		for(a=value_size-1 ; a>=0 && table[a]==0 ; --a);

		if( a < 0 )
		{
			// there's a value zero
			TTMATH_LOG("UInt::CompensationToLeft")
			return moving;
		}

		if( a != value_size-1 )
		{
			moving += ( value_size-1 - a ) * TTMATH_BITS_PER_UINT;

			// moving all words
			sint i;
			for(i=value_size-1 ; a>=0 ; --i, --a)
				table[i] = table[a];

			// setting the rest word to zero
			for(; i>=0 ; --i)
				table[i] = 0;
		}

		uint moving2 = FindLeadingBitInWord( table[value_size-1] );
		// moving2 is different from -1 because the value table[value_size-1]
		// is not zero

		moving2 = TTMATH_BITS_PER_UINT - moving2 - 1;
		Rcl(moving2);

		TTMATH_LOG("UInt::CompensationToLeft")

	return moving + moving2;
	}




	/*!
		this method looks for the highest set bit
		
		result:
			if 'this' is not zero:
				return value - true
				'table_id'   - the index of a word <0..value_size-1>
				'index'      - the index of this set bit in the word <0..31>

			if 'this' is zero: 
				return value - false
				both 'table_id' and 'index' are zero
	*/
	bool FindLeadingBit(uint & table_id, uint & index) const
	{
		for(table_id=value_size-1 ; table_id!=0 && table[table_id]==0 ; --table_id);

		if( table_id==0 && table[table_id]==0 )
		{
			// is zero
			index = 0;

			TTMATH_LOG("UInt::FindLeadingBit")

		return false;
		}
		
		// table[table_id] != 0
		index = FindLeadingBitInWord( table[table_id] );

		TTMATH_LOG("UInt::FindLeadingBit")

	return true;
	}
	




	/*!
		setting the 'bit_index' bit

		bit_index bigger or equal zero
	*/
	uint GetBit(uint bit_index) const
	{
		TTMATH_ASSERT( bit_index < value_size * TTMATH_BITS_PER_UINT )

		uint index = bit_index / TTMATH_BITS_PER_UINT;
		uint bit   = bit_index % TTMATH_BITS_PER_UINT;

		uint temp = table[index];
		uint res  = SetBitInWord(temp, bit);

		TTMATH_LOG("UInt::GetBit")

	return res;
	}


	/*!
		setting the 'bit_index' bit
		and returning the last state of the bit

		bit_index bigger or equal zero
	*/
	uint SetBit(uint bit_index)
	{
		TTMATH_ASSERT( bit_index < value_size * TTMATH_BITS_PER_UINT )

		uint index = bit_index / TTMATH_BITS_PER_UINT;
		uint bit   = bit_index % TTMATH_BITS_PER_UINT;
		uint res   = SetBitInWord(table[index], bit);

		TTMATH_LOG("UInt::SetBit")

	return res;
	}


	/*!
		this method performs a bitwise operation AND 
	*/
	void BitAnd(const UInt<value_size> & ss2)
	{
		for(uint x=0 ; x<value_size ; ++x)
			table[x] &= ss2.table[x];

		TTMATH_LOG("UInt::BitAnd")
	}


	/*!
		this method performs a bitwise operation OR 
	*/
	void BitOr(const UInt<value_size> & ss2)
	{
		for(uint x=0 ; x<value_size ; ++x)
			table[x] |= ss2.table[x];

		TTMATH_LOG("UInt::BitOr")
	}


	/*!
		this method performs a bitwise operation XOR 
	*/
	void BitXor(const UInt<value_size> & ss2)
	{
		for(uint x=0 ; x<value_size ; ++x)
			table[x] ^= ss2.table[x];

		TTMATH_LOG("UInt::BitXor")
	}


	/*!
		this method performs a bitwise operation NOT
	*/
	void BitNot()
	{
		for(uint x=0 ; x<value_size ; ++x)
			table[x] = ~table[x];

		TTMATH_LOG("UInt::BitNot")
	}


	/*!
		this method performs a bitwise operation NOT but only
		on the range of <0, leading_bit>

		for example:
			BitNot2(8) = BitNot2( 1000(bin) ) = 111(bin) = 7
	*/
	void BitNot2()
	{
	uint table_id, index;

		if( FindLeadingBit(table_id, index) )
		{
			for(uint x=0 ; x<table_id ; ++x)
				table[x] = ~table[x];

			uint mask  = TTMATH_UINT_MAX_VALUE;
			uint shift = TTMATH_BITS_PER_UINT - index - 1;

			if(shift)
				mask >>= shift;

			table[table_id] ^= mask;
		}
		else
			table[0] = 1;


		TTMATH_LOG("UInt::BitNot2")
	}



	/*!
	 *
	 * Multiplication
	 *
	 *
	*/

public:

	/*!
		multiplication: this = this * ss2

		it returns a carry if it has been
	*/
	uint MulInt(uint ss2)
	{
	uint r2,r1;

		UInt<value_size> u( *this );
		SetZero();

		for(uint x1=0 ; x1<value_size ; ++x1)
		{
			MulTwoWords(u.table[x1], ss2, &r2, &r1 );
			
			
			if( x1 <= value_size - 2 )
			{
				if( AddTwoInts(r2,r1,x1) )
					return 1;
			}
			else
			{
				// last iteration:
				// x1 = value_size - 1;

				if( r2 )
					return 1;

				if( AddInt(r1, x1) )
					return 1;
			}
		}

		TTMATH_LOG("UInt::MulInt(uint)")

	return 0;
	}

	/*!
		multiplication: result = this * ss2

		we're using this method only when result_size is greater than value_size
		if so there will not be a carry
	*/
	template<uint result_size>
	uint MulInt(uint ss2, UInt<result_size> & result)
	{
	uint r2,r1;
	uint x1size=value_size;
	uint x1start=0;

		if( value_size >= result_size )
			return 1;

		result.SetZero();

		if( value_size > 2 )
		{	
			// if the value_size is smaller than or equal to 2
			// there is no sense to set x1size and x1start to another values

			for(x1size=value_size ; x1size>0 && table[x1size-1]==0 ; --x1size);

			if( x1size==0 )
			{
				TTMATH_LOG("UInt::MulInt(uint, UInt<>)")
				return 0;
			}

			for(x1start=0 ; x1start<x1size && table[x1start]==0 ; ++x1start);
		}

		for(uint x1=x1start ; x1<x1size ; ++x1)
		{
			MulTwoWords(table[x1], ss2, &r2, &r1 );
			result.AddTwoInts(r2,r1,x1);
		}


		TTMATH_LOG("UInt::MulInt(uint, UInt<>)")

	return 0;
	}



	/*!
		the multiplication 'this' = 'this' * ss2
	*/
	uint Mul(const UInt<value_size> & ss2, uint algorithm = 2)
	{
		switch( algorithm )
		{
		case 1:
			return Mul1(ss2);

		case 2:
		default:
			return Mul2(ss2);
		}
	}


	/*!
		the multiplication 'result' = 'this' * ss2

		since the 'result' is twice bigger than 'this' and 'ss2' 
		this method never returns a carry
	*/
	void MulBig(const UInt<value_size> & ss2,
				UInt<value_size*2> & result, 
				uint algorithm = 2)
	{
		switch( algorithm )
		{
		case 1:
			return Mul1Big(ss2, result);

		case 2:
		default:
			return Mul2Big(ss2, result);
		}
	}



	/*!
		the first version of the multiplication algorithm
	*/

	/*!
		multiplication: this = this * ss2

		it returns carry if it has been
	*/
	uint Mul1(const UInt<value_size> & ss2)
	{
	TTMATH_REFERENCE_ASSERT( ss2 )

	UInt<value_size> ss1( *this );
	SetZero();	

		for(uint i=0; i < value_size*TTMATH_BITS_PER_UINT ; ++i)
		{
			if( Add(*this) )
			{
				TTMATH_LOG("UInt::Mul1")
				return 1;
			}

			if( ss1.Rcl(1) )
				if( Add(ss2) )
				{
					TTMATH_LOG("UInt::Mul1")
					return 1;
				}
		}

		TTMATH_LOG("UInt::Mul1")

	return 0;
	}

	
	/*!
		multiplication: result = this * ss2

		result is twice bigger than 'this' and 'ss2'
		this method never returns carry			
	*/
	void Mul1Big(const UInt<value_size> & ss2_, UInt<value_size*2> & result)
	{
	UInt<value_size*2> ss2;
	uint i;

		// copying *this into result and ss2_ into ss2
		for(i=0 ; i<value_size ; ++i)
		{
			result.table[i] = table[i];
			ss2.table[i]    = ss2_.table[i];
		}

		// cleaning the highest bytes in result and ss2
		for( ; i < value_size*2 ; ++i)
		{
			result.table[i] = 0;
			ss2.table[i]    = 0;
		}

		// multiply
		// (there will not be a carry)
		result.Mul1( ss2 );

		TTMATH_LOG("UInt::Mul1Big")
	}



	/*!
		the second version of the multiplication algorithm

		this algorithm is similar to the 'schoolbook method' which is done by hand
	*/

	/*!
		multiplication: this = this * ss2

		it returns carry if it has been
	*/
	uint Mul2(const UInt<value_size> & ss2)
	{
	UInt<value_size*2> result;
	uint i;

		Mul2Big(ss2, result);
	
		// copying result
		for(i=0 ; i<value_size ; ++i)
			table[i] = result.table[i];

		// testing carry
		for( ; i<value_size*2 ; ++i)
			if( result.table[i] != 0 )
				return 1;

		TTMATH_LOG("UInt::Mul2")

	return 0;
	}


	/*!
		multiplication: result = this * ss2

		result is twice bigger than this and ss2
		this method never returns carry			
	*/
	void Mul2Big(const UInt<value_size> & ss2, UInt<value_size*2> & result)
	{
	uint r2,r1;
	uint x1size=value_size, x2size=value_size;
	uint x1start=0, x2start=0;

		result.SetZero();

		if( value_size > 2 )
		{	
			// if the value_size is smaller than or equal to 2
			// there is no sense to set x1size (and others) to another values

			for(x1size=value_size ; x1size>0 && table[x1size-1]==0 ; --x1size);
			for(x2size=value_size ; x2size>0 && ss2.table[x2size-1]==0 ; --x2size);

			if( x1size==0 || x2size==0 )
			{
				TTMATH_LOG("UInt::Mul2Big")
				return;
			}

			for(x1start=0 ; x1start<x1size && table[x1start]==0 ; ++x1start);
			for(x2start=0 ; x2start<x2size && ss2.table[x2start]==0 ; ++x2start);
		}

		for(uint x1=x1start ; x1<x1size ; ++x1)
		{
			for(uint x2=x2start ; x2<x2size ; ++x2)
			{
				MulTwoWords(table[x1], ss2.table[x2], &r2, &r1);
				result.AddTwoInts(r2,r1,x2+x1);
				// here will never be a carry
			}
		}

		TTMATH_LOG("UInt::Mul2Big")
	}




	/*!
	 *
	 * Division
	 *
	 *
	*/
	
public:


	/*!
		division by one unsigned word

		returns 1 when divisor is zero
	*/
	uint DivInt(uint divisor, uint * remainder = 0)
	{
		if( divisor == 0 )
		{
			if( remainder )
				*remainder = 0; // this is for convenience, without it the compiler can report that 'remainder' is uninitialized

			TTMATH_LOG("UInt::DivInt")

		return 1;
		}

		if( divisor == 1 )
		{
			if( remainder )
				*remainder = 0;

			TTMATH_LOG("UInt::DivInt")

		return 0;
		}

		UInt<value_size> dividend(*this);
		SetZero();
		
		sint i;  // i must be with a sign
		uint r = 0;

		// we're looking for the last word in ss1
		for(i=value_size-1 ; i>0 && dividend.table[i]==0 ; --i);

		for( ; i>=0 ; --i)
			DivTwoWords(r, dividend.table[i], divisor, &table[i], &r);

		if( remainder )
			*remainder = r;

		TTMATH_LOG("UInt::DivInt")

	return 0;
	}

	uint DivInt(uint divisor, uint & remainder)
	{
		return DivInt(divisor, &remainder);
	}



	/*!
		division this = this / ss2
		
		return values:
			 0 - ok
			 1 - division by zero
			'this' will be the quotient
			'remainder' - remainder
	*/
	uint Div(	const UInt<value_size> & divisor,
				UInt<value_size> * remainder = 0,
				uint algorithm = 3)
	{
		switch( algorithm )
		{
		case 1:
			return Div1(divisor, remainder);

		case 2:
			return Div2(divisor, remainder);

		case 3:
		default:
			return Div3(divisor, remainder);
		}
	}

	uint Div(const UInt<value_size> & divisor, UInt<value_size> & remainder, uint algorithm = 3)
	{
		return Div(divisor, &remainder, algorithm);
	}



private:

	/*!
		return values:
		0 - none has to be done
		1 - division by zero
		2 - division should be made
	*/
	uint Div_StandardTest(	const UInt<value_size> & v,
							uint & m, uint & n,
							UInt<value_size> * remainder = 0)
	{
		switch( Div_CalculatingSize(v, m, n) )
		{
		case 4: // 'this' is equal v
			if( remainder )
				remainder->SetZero();

			SetOne();
			TTMATH_LOG("UInt::Div_StandardTest")
			return 0;

		case 3: // 'this' is smaller than v
			if( remainder )
				*remainder = *this;

			SetZero();
			TTMATH_LOG("UInt::Div_StandardTest")
			return 0;

		case 2: // 'this' is zero
			if( remainder )
				remainder->SetZero();

			SetZero();
			TTMATH_LOG("UInt::Div_StandardTest")
			return 0;

		case 1: // v is zero
			TTMATH_LOG("UInt::Div_StandardTest")
			return 1;
		}

		TTMATH_LOG("UInt::Div_StandardTest")

	return 2;
	}



	/*!
		return values:
		0 - ok 
			'm' - is the index (from 0) of last non-zero word in table ('this')
			'n' - is the index (from 0) of last non-zero word in v.table
		1 - v is zero 
		2 - 'this' is zero
		3 - 'this' is smaller than v
		4 - 'this' is equal v

		if the return value is different than zero the 'm' and 'n' are undefined
	*/
	uint Div_CalculatingSize(const UInt<value_size> & v, uint & m, uint & n)
	{
		for(n = value_size-1 ; n!=0 && v.table[n]==0 ; --n);

		if( n==0 && v.table[n]==0 )
			return 1;

		for(m = value_size-1 ; m!=0 && table[m]==0 ; --m);

		if( m==0 && table[m]==0 )
			return 2;

		if( m < n )
			return 3;
		else
		if( m == n )
		{
			uint i;
			for(i = n ; i!=0 && table[i]==v.table[i] ; --i);
			
			if( table[i] < v.table[i] )
				return 3;
			else
			if (table[i] == v.table[i] )
				return 4;
		}

	return 0;
	}


public:

	/*!
		the first division's algorithm
		radix 2
	*/
	uint Div1(const UInt<value_size> & divisor, UInt<value_size> * remainder = 0)
	{
	uint m,n, test;

		test = Div_StandardTest(divisor, m, n, remainder);
		if( test < 2 )
			return test;

		if( !remainder )
		{
			UInt<value_size> rem;
	
		return Div1_Calculate(divisor, rem);
		}

	return Div1_Calculate(divisor, *remainder);
	}


private:


	uint Div1_Calculate(const UInt<value_size> & divisor, UInt<value_size> & rest)
	{
	TTMATH_REFERENCE_ASSERT( divisor )
	
	sint loop;
	sint c;

		rest.SetZero();
		loop = value_size * TTMATH_BITS_PER_UINT;
		c = 0;

		
	div_a:
		c = Rcl(1, c);
		c = rest.Add(rest,c);
		c = rest.Sub(divisor,c);

		c = !c;

		if(!c)
			goto div_d;


	div_b:
		--loop;
		if(loop)
			goto div_a;

		c = Rcl(1, c);
		TTMATH_LOG("UInt::Div1_Calculate")
		return 0;


	div_c:
		c = Rcl(1, c);
		c = rest.Add(rest,c);
		c = rest.Add(divisor);

		if(c)
			goto div_b;


	div_d:
		--loop;
		if(loop)
			goto div_c;

		c = Rcl(1, c);
		c = rest.Add(divisor);

		TTMATH_LOG("UInt::Div1_Calculate")

	return 0;
	}
	

public:


	/*!
		the second division algorithm

		return values:
			0 - ok
			1 - division by zero
	*/
	uint Div2(const UInt<value_size> & divisor, UInt<value_size> * remainder = 0)
	{
		TTMATH_REFERENCE_ASSERT( divisor )

		uint bits_diff;
		uint status = Div2_Calculate(divisor, remainder, bits_diff);
		if( status < 2 )
			return status;

		if( CmpBiggerEqual(divisor) )
		{
			Div2(divisor, remainder);
			SetBit(bits_diff);
		}
		else
		{
			if( remainder )
				*remainder = *this;

			SetZero();
			SetBit(bits_diff);
		}

		TTMATH_LOG("UInt::Div2")

	return 0;
	}


	uint Div2(const UInt<value_size> & divisor, UInt<value_size> & remainder)
	{
		return Div2(divisor, &remainder);
	}


private:

	/*!
		return values:
			0 - we've calculated the division
			1 - division by zero
			2 - we have to still calculate

	*/
	uint Div2_Calculate(const UInt<value_size> & divisor, UInt<value_size> * remainder,
															uint & bits_diff)
	{
	uint table_id, index;
	uint divisor_table_id, divisor_index;

		uint status = Div2_FindLeadingBitsAndCheck(	divisor, remainder,
													table_id, index,
													divisor_table_id, divisor_index);

		if( status < 2 )
		{
			TTMATH_LOG("UInt::Div2_Calculate")
			return status;
		}
		
		// here we know that 'this' is greater than divisor
		// then 'index' is greater or equal 'divisor_index'
		bits_diff = index - divisor_index;

		UInt<value_size> divisor_copy(divisor);
		divisor_copy.Rcl(bits_diff, 0);

		if( CmpSmaller(divisor_copy, table_id) )
		{
			divisor_copy.Rcr(1);
			--bits_diff;
		}

		Sub(divisor_copy, 0);

		TTMATH_LOG("UInt::Div2_Calculate")

	return 2;
	}


	/*!
		return values:
			0 - we've calculated the division
			1 - division by zero
			2 - we have to still calculate
	*/
	uint Div2_FindLeadingBitsAndCheck(	const UInt<value_size> & divisor,
										UInt<value_size> * remainder,
										uint & table_id, uint & index,
										uint & divisor_table_id, uint & divisor_index)
	{
		if( !divisor.FindLeadingBit(divisor_table_id, divisor_index) )
		{
			// division by zero
			TTMATH_LOG("UInt::Div2_FindLeadingBitsAndCheck")
			return 1;
		}

		if(	!FindLeadingBit(table_id, index) )
		{
			// zero is divided by something
			
			SetZero();

			if( remainder )
				remainder->SetZero();

			TTMATH_LOG("UInt::Div2_FindLeadingBitsAndCheck")

		return 0;
		}
	
		divisor_index += divisor_table_id * TTMATH_BITS_PER_UINT;
		index         += table_id         * TTMATH_BITS_PER_UINT;

		if( divisor_table_id == 0 )
		{
			// dividor has only one 32-bit word

			uint r;
			DivInt(divisor.table[0], &r);

			if( remainder )
			{
				remainder->SetZero();
				remainder->table[0] = r;
			}

			TTMATH_LOG("UInt::Div2_FindLeadingBitsAndCheck")

		return 0;
		}
	

		if( Div2_DivisorGreaterOrEqual(	divisor, remainder,
										table_id, index,
										divisor_table_id, divisor_index) )
		{
			TTMATH_LOG("UInt::Div2_FindLeadingBitsAndCheck")
			return 0;
		}


		TTMATH_LOG("UInt::Div2_FindLeadingBitsAndCheck")

	return 2;
	}


	/*!
		return values:
			true if divisor is equal or greater than 'this'
	*/
	bool Div2_DivisorGreaterOrEqual(	const UInt<value_size> & divisor,
										UInt<value_size> * remainder, 
										uint table_id, uint index,
										uint /*divisor_table_id*/, uint divisor_index  )
	{
		if( divisor_index > index )
		{
			// divisor is greater than this

			if( remainder )
				*remainder = *this;

			SetZero();

			TTMATH_LOG("UInt::Div2_DivisorGreaterOrEqual")

		return true;
		}

		if( divisor_index == index )
		{
			// table_id == divisor_table_id as well

			uint i;
			for(i = table_id ; i!=0 && table[i]==divisor.table[i] ; --i);
			
			if( table[i] < divisor.table[i] )
			{
				// divisor is greater than 'this'

				if( remainder )
					*remainder = *this;

				SetZero();

				TTMATH_LOG("UInt::Div2_DivisorGreaterOrEqual")

			return true;
			}
			else
			if( table[i] == divisor.table[i] )
			{
				// divisor is equal 'this'

				if( remainder )
					remainder->SetZero();

				SetOne();

				TTMATH_LOG("UInt::Div2_DivisorGreaterOrEqual")

			return true;
			}
		}

		TTMATH_LOG("UInt::Div2_DivisorGreaterOrEqual")

	return false;
	}


public:

	/*!
		the third division algorithm

		this algorithm is described in the following book:
			"The art of computer programming 2" (4.3.1 page 272)
			Donald E. Knuth 
	*/
	uint Div3(const UInt<value_size> & v, UInt<value_size> * remainder = 0)
	{
	TTMATH_REFERENCE_ASSERT( v )

	uint m,n, test;

		test = Div_StandardTest(v, m, n, remainder);
		if( test < 2 )
			return test;

		if( n == 0 )
		{
			uint r;
			DivInt( v.table[0], &r );

			if( remainder )
			{
				remainder->SetZero();
				remainder->table[0] = r;
			}

			TTMATH_LOG("UInt::Div3")

		return 0;
		}


		// we can only use the third division algorithm when 
		// the divisor is greater or equal 2^32 (has more than one 32-bit word)
		++m;
		++n;
		m = m - n; 
		Div3_Division(v, remainder, m, n);

		TTMATH_LOG("UInt::Div3")

	return 0;
	}



private:


	void Div3_Division(UInt<value_size> v, UInt<value_size> * remainder, uint m, uint n)
	{
	TTMATH_ASSERT( n>=2 )

	UInt<value_size+1> uu, vv;
	UInt<value_size> q;
	uint d, u_value_size, u0, u1, u2, v1, v0, j=m;	
	
		u_value_size = Div3_Normalize(v, n, d);

		if( j+n == value_size )
			u2 = u_value_size;
		else
			u2 = table[j+n];

		Div3_MakeBiggerV(v, vv);

		for(uint i = j+1 ; i<value_size ; ++i)
			q.table[i] = 0;

		for (;;)
		{
			u1 = table[j+n-1];
			u0 = table[j+n-2];
			v1 = v.table[n-1];
			v0 = v.table[n-2];

			uint qp = Div3_Calculate(u2,u1,u0, v1,v0);

			Div3_MakeNewU(uu, j, n, u2);
			Div3_MultiplySubtract(uu, vv, qp);
			Div3_CopyNewU(uu, j, n);

			q.table[j] = qp;

			// the next loop
			if( j-- == 0 )
				break;

			u2 = table[j+n];
		}

		if( remainder )
			Div3_Unnormalize(remainder, n, d);

	*this = q;

	TTMATH_LOG("UInt::Div3_Division")
	}


	void Div3_MakeNewU(UInt<value_size+1> & uu, uint j, uint n, uint u_max)
	{
	uint i;

		for(i=0 ; i<n ; ++i, ++j)
			uu.table[i] = table[j];

		// 'n' is from <1..value_size> so and 'i' is from <0..value_size>
		// then table[i] is always correct (look at the declaration of 'uu')
		uu.table[i] = u_max;

		for( ++i ; i<value_size+1 ; ++i)
			uu.table[i] = 0;

		TTMATH_LOG("UInt::Div3_MakeNewU")
	}


	void Div3_CopyNewU(const UInt<value_size+1> & uu, uint j, uint n)
	{
	uint i;

		for(i=0 ; i<n ; ++i)
			table[i+j] = uu.table[i];

		if( i+j < value_size )
			table[i+j] = uu.table[i];

		TTMATH_LOG("UInt::Div3_CopyNewU")
	}


	/*!
		we're making the new 'vv' 
		the value is actually the same but the 'table' is bigger (value_size+1)
	*/
	void Div3_MakeBiggerV(const UInt<value_size> & v, UInt<value_size+1> & vv)
	{
		for(uint i=0 ; i<value_size ; ++i)
			vv.table[i] = v.table[i];

		vv.table[value_size] = 0;

		TTMATH_LOG("UInt::Div3_MakeBiggerV")
	}
	

	/*!
		we're moving all bits from 'v' into the left side of the n-1 word
		(the highest bit at v.table[n-1] will be equal one,
		the bits from 'this' we're moving the same times as 'v')

		return values:
		  d - how many times we've moved
		  return - the next-left value from 'this' (that after table[value_size-1])
	*/
	uint Div3_Normalize(UInt<value_size> & v, uint n, uint & d)
	{
		// v.table[n-1] is != 0

		uint bit  = (uint)FindLeadingBitInWord(v.table[n-1]);
		uint move = (TTMATH_BITS_PER_UINT - bit - 1);
		uint res  = table[value_size-1];
		d         = move;

		if( move > 0 )
		{
			v.Rcl(move, 0);
			Rcl(move, 0);
			res = res >> (bit + 1);
		}
		else
		{
			res = 0;
		}

		TTMATH_LOG("UInt::Div3_Normalize")

	return res;
	}


	void Div3_Unnormalize(UInt<value_size> * remainder, uint n, uint d)
	{
		for(uint i=n ; i<value_size ; ++i)
			table[i] = 0;

		Rcr(d,0);

		*remainder = *this;

		TTMATH_LOG("UInt::Div3_Unnormalize")
	}


	uint Div3_Calculate(uint u2, uint u1, uint u0, uint v1, uint v0)
	{	
	UInt<2> u_temp;
	uint rp;
	bool next_test;

		TTMATH_ASSERT( v1 != 0 )

		u_temp.table[1] = u2;
		u_temp.table[0] = u1;
		u_temp.DivInt(v1, &rp);

		TTMATH_ASSERT( u_temp.table[1]==0 || u_temp.table[1]==1 )

		do
		{
			bool decrease = false;

			if( u_temp.table[1] == 1 )
				decrease = true;
			else
			{
				UInt<2> temp1, temp2;

				UInt<2>::MulTwoWords(u_temp.table[0], v0, temp1.table+1, temp1.table);
				temp2.table[1] = rp;
				temp2.table[0] = u0;

				if( temp1 > temp2 )
					decrease = true;
			}

			next_test = false;

			if( decrease )
			{
				u_temp.SubOne();

				rp += v1;

				if( rp >= v1 ) // it means that there wasn't a carry (r<b from the book)
					next_test = true;
			}
		}
		while( next_test );

		TTMATH_LOG("UInt::Div3_Calculate")

	return u_temp.table[0];
	}



	void Div3_MultiplySubtract(	UInt<value_size+1> & uu,
								const UInt<value_size+1> & vv, uint & qp)
	{
		// D4 (in the book)

		UInt<value_size+1> vv_temp(vv);
		vv_temp.MulInt(qp);

		if( uu.Sub(vv_temp) )  
		{
			// there was a carry
			
			//
			// !!! this part of code was not tested
			//

			--qp;
			uu.Add(vv);

			// can be a carry from this additions but it should be ignored 
			// because it cancels with the borrow from uu.Sub(vv_temp)
		}

		TTMATH_LOG("UInt::Div3_MultiplySubtract")
	}






public:


	/*!
		power this = this ^ pow
		binary algorithm (r-to-l)

		return values:
		0 - ok
		1 - carry or
		2 - incorrect argument (0^0)
	*/
	uint Pow(UInt<value_size> pow)
	{
		if(pow.IsZero() && IsZero())
			// we don't define zero^zero
			return 2;

		UInt<value_size> start(*this), start_temp;
		UInt<value_size> result;
		result.SetOne();

		while( !pow.IsZero() )
		{
			if( pow.table[0] & 1 )
				if( result.Mul(start) )
				{
					TTMATH_LOG("UInt::Pow(UInt<>)")
					return 1;
				}

			start_temp = start;
			// in the second Mul algorithm we can use start.Mul(start) directly (there is no TTMATH_ASSERT_REFERENCE there)
			if( start.Mul(start_temp) )
			{
				TTMATH_LOG("UInt::Pow(UInt<>)")
				return 1;
			}

			pow.Rcr2_one(0);
		}

		*this = result;

		TTMATH_LOG("UInt::Pow(UInt<>)")

	return 0;
	}



	/*!
		this method sets n first bits to value zero

		For example:
		let n=2 then if there's a value 111 (bin) there'll be '100' (bin)
	*/
	void ClearFirstBits(uint n)
	{
		if( n >= value_size*TTMATH_BITS_PER_UINT )
		{
			SetZero();
			TTMATH_LOG("UInt::ClearFirstBits")
			return;
		}

		uint * p = table;

		// first we're clearing the whole words
		while( n >= TTMATH_BITS_PER_UINT )
		{
			*p++ = 0;
			n   -= TTMATH_BITS_PER_UINT;
		}

		if( n == 0 )
		{
			TTMATH_LOG("UInt::ClearFirstBits")
			return;
		}

		// and then we're clearing one word which has left
		// mask -- all bits are set to one
		uint mask = TTMATH_UINT_MAX_VALUE;

		mask = mask << n;

		(*p) &= mask;

		TTMATH_LOG("UInt::ClearFirstBits")
	}


	/*!
		this method returns true if the highest bit of the value is set
	*/
	bool IsTheHighestBitSet() const
	{
		return (table[value_size-1] & TTMATH_UINT_HIGHEST_BIT) != 0;
	}


	/*!
		this method returns true if the lowest bit of the value is set
	*/
	bool IsTheLowestBitSet() const
	{
		return (table[0] & 1) != 0;
	}


	/*!
		this method returns true if the value is equal zero
	*/
	bool IsZero() const
	{
		for(uint i=0 ; i<value_size ; ++i)
			if(table[i] != 0)
				return false;

	return true;
	}






	/*!
	*
	*	conversion method
	*
	*/



	/*!
		this static method converts one character into its value

		for example:
			1 -> 1
			8 -> 8
			A -> 10
			f -> 15

		this method don't check whether c is correct or not
	*/
	static uint CharToDigit(uint c)
	{
		if(c>='0' && c<='9')
			return c-'0';

		if(c>='a' && c<='z')
			return c-'a'+10;

	return c-'A'+10;
	}


	/*!
		this method changes a character 'c' into its value
		(if there can't be a correct value it returns -1)

		for example:
		c=2, base=10 -> function returns 2
		c=A, base=10 -> function returns -1
		c=A, base=16 -> function returns 10
	*/
	static sint CharToDigit(uint c, uint base)
	{
		if( c>='0' && c<='9' )
			c=c-'0';
		else
		if( c>='a' && c<='z' )
			c=c-'a'+10;
		else
		if( c>='A' && c<='Z' )
			c=c-'A'+10;
		else
			return -1;


		if( c >= base )
			return -1;


	return sint(c);
	}


	/*!
		this method converts a digit into a tchar_t
		digit should be from <0,F>
		(we don't have to get a base)
		
		for example:
			1  -> 1
			8  -> 8
			10 -> A
			15 -> F
	*/
	static tchar_t DigitToChar(uint digit)
	{
		if( digit < 10 )
			return (tchar_t)(digit + '0');

	return((tchar_t)(digit - 10 + 'A'));
	}


	/*!
		this method converts an UInt<another_size> type to this class

		this operation has mainly sense if the value from p is 
		equal or smaller than that one which is returned from UInt<value_size>::SetMax()

		it returns a carry if the value 'p' is too big
	*/
	template<uint argument_size>
	uint FromUInt(const UInt<argument_size> & p)
	{
		uint min_size = (value_size < argument_size)? value_size : argument_size;
		uint i;

		for(i=0 ; i<min_size ; ++i)
			table[i] = p.table[i];


		if( value_size > argument_size )
		{	
			// 'this' is longer than 'p'

			for( ; i<value_size ; ++i)
				table[i] = 0;
		}
		else
		{
			for( ; i<argument_size ; ++i)
				if( p.table[i] != 0 )
				{
					TTMATH_LOG("UInt::FromUInt(UInt<>)")
					return 1;
				}
		}

		TTMATH_LOG("UInt::FromUInt(UInt<>)")

	return 0;
	}


	/*!
		this method converts the uint type to this class
	*/
	void FromUInt(uint value)
	{
		for(uint i=1 ; i<value_size ; ++i)
			table[i] = 0;

		table[0] = value;

		TTMATH_LOG("UInt::FromUInt(uint)")
	}


	/*!
		this operator converts an UInt<another_size> type to this class

		it doesn't return a carry
	*/
	template<uint argument_size>
	UInt<value_size> & operator=(const UInt<argument_size> & p)
	{
		FromUInt(p);

		TTMATH_LOG("UInt::operator=(UInt<argument_size>)")

		return *this;
	}

	/*!
		the assignment operator
	*/
	UInt<value_size> & operator=(const UInt<value_size> & p)
	{
		FromUInt(p);

		TTMATH_LOG("UInt::operator=(UInt<>)")

		return *this;
	}


	/*!
		this method converts the uint type to this class
	*/
	UInt<value_size> & operator=(uint i)
	{
		FromUInt(i);

		TTMATH_LOG("UInt::operator=(uint)")

	return *this;
	}


	/*!
		a constructor for converting the uint to this class
	*/
	UInt(uint i)
	{
		FromUInt(i);

		TTMATH_LOG("UInt::UInt(uint)")
	}


	/*!
		this method converts the sint type to this class

		we provide operator(sint) and the constructor(sint) in order to allow
		the programmer do that:
			UInt<..> type = 10;

		this constant 10 has the int type (signed int), if we don't give such
		operators and constructors the compiler will not compile the program,
		because it has to make a conversion and doesn't know into which type
		(the UInt class has operator=(const tchar_t*), operator=(uint) etc.)
	*/
	UInt<value_size> & operator=(sint i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt::operator=(sint)")

	return *this;
	}


	/*!
		a constructor for converting the sint to this class

		look at the description of UInt::operator=(sint)
	*/
	UInt(sint i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt::UInt(sint)")
	}



#ifdef TTMATH_PLATFORM64

	/*!
		in 64bit platforms we must define additional operators and contructors
		in order to allow a user initializing the objects in this way:
			UInt<...> type = 20;
		or
			UInt<...> type; 
			type = 30;

		decimal constants such as 20, 30 etc. are integer literal of type int,
		if the value is greater it can even be long int,
		0 is an octal integer of type int
		(ISO 14882 p2.13.1 Integer literals)
	*/

	/*!
		this operator converts the unsigned int type to this class

		***this operator is created only on a 64bit platform***
		it takes one argument of 32bit
	*/
	UInt<value_size> & operator=(unsigned int i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt64::operator=(unsigned int)")

	return *this;
	}


	/*!
		a constructor for converting the unsigned int to this class

		***this constructor is created only on a 64bit platform***
		it takes one argument of 32bit
	*/
	UInt(unsigned int i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt64::UInt(unsigned int)")
	}


	/*!
		an operator for converting the signed int to this class

		***this constructor is created only on a 64bit platform***
		it takes one argument of 32bit

		look at the description of UInt::operator=(sint)
	*/
	UInt<value_size> & operator=(signed int i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt64::operator=(signed int)")

	return *this;
	}


	/*!
		a constructor for converting the signed int to this class

		***this constructor is created only on a 64bit platform***
		it takes one argument of 32bit

		look at the description of UInt::operator=(sint)
	*/
	UInt(signed int i)
	{
		FromUInt(uint(i));

		TTMATH_LOG("UInt64::UInt(signed int)")
	}


#endif





	/*!
		a constructor for converting a string to this class (with the base=10)
	*/
	UInt(const tchar_t * s)
	{
		FromString(s);

		TTMATH_LOG("UInt::UInt(const tchar_t *)")
	}


	/*!
		a constructor for converting a string to this class (with the base=10)
	*/
	UInt(const tstr_t & s)
	{
		FromString( s.c_str() );
	}


	/*!
		a default constructor

		we don't clear table etc.
	*/
	UInt()
	{
	}

	/*!
		a copy constructor
	*/
	UInt(const UInt<value_size> & u)
	{
		FromUInt(u);

		TTMATH_LOG("UInt::UInt(UInt<>)")
	}



	/*!
		a template for producting constructors for copying from another types
	*/
	template<uint argument_size>
	UInt(const UInt<argument_size> & u)
	{
		// look that 'size' we still set as 'value_size' and not as u.value_size
		FromUInt(u);

		TTMATH_LOG("UInt::UInt(UInt<argument_size>)")
	}




	/*!
		a destructor
	*/
	~UInt()
	{
	}


	/*!
		this method returns the lowest value from table

		we must be sure when we using this method whether the value
		will hold in an uint type or not (the rest value from the table must be zero)
	*/
	uint ToUInt() const
	{
		return table[0];
	}


	/*!	
		this method converts the value to a string with a base equal 'b'
	*/
	void ToString(tstr_t & result, uint b = 10) const
	{
	UInt<value_size> temp( *this );
	tchar_t character;
	uint rem;

		result.clear();

		if( b<2 || b>16 )
			return;

		do
		{
			temp.DivInt(b, &rem);
			character = DigitToChar( rem );
			result.insert(result.begin(), character);
		}
		while( !temp.IsZero() );

	return;
	}




	/*
		this method's ommiting any white characters from the string
	*/
	static void SkipWhiteCharacters(const tchar_t * & c)
	{
		while( (*c==' ' ) || (*c=='\t') || (*c==13 ) || (*c=='\n') )
			++c;
	}


	/*!
		this method converts a string into its value
		it returns carry=1 if the value will be too big or an incorrect base 'b' is given

		string is ended with a non-digit value, for example:
			"12" will be translated to 12
			as well as:
			"12foo" will be translated to 12 too

		existing first white characters will be ommited

		if the value from s is too large the rest digits will be skipped

		after_source (if exists) is pointing at the end of the parsed string

		value_read (if exists) tells whether something has actually been read (at least one digit)
	*/
	uint FromString(const tchar_t * s, uint b = 10, const tchar_t ** after_source = 0, bool * value_read = 0)
	{
	UInt<value_size> base( b );
	UInt<value_size> temp;
	sint z;
	uint c = 0;

		SetZero();
		temp.SetZero();
		SkipWhiteCharacters(s);

		if( after_source )
			*after_source = s;

		if( value_read )
			*value_read = false;

		if( b<2 || b>16 )
			return 1;


		for( ; (z=CharToDigit(*s, b)) != -1 ; ++s)
		{
			if( value_read )
				*value_read = true;

			if( c == 0 )
			{
				temp.table[0] = z;

				c += Mul(base);
				c += Add(temp);
			}
		}		

		if( after_source )
			*after_source = s;

		TTMATH_LOG("UInt::FromString")

	return (c==0)? 0 : 1;
	}



	/*!
		this method converts a string into its value

		(it returns carry=1 if the value will be too big or an incorrect base 'b' is given)
	*/
	uint FromString(const tstr_t & s, uint b = 10)
	{
		return FromString( s.c_str(), b );
	}



	/*!
		this operator converts a string into its value (with base = 10)
	*/
	UInt<value_size> & operator=(const tchar_t * s)
	{
		FromString(s);

		TTMATH_LOG("UInt::operator=(const tchar_t *)")

	return *this;
	}


	/*!
		this operator converts a string into its value (with base = 10)
	*/
	UInt<value_size> & operator=(const tstr_t & s)
	{
		FromString( s.c_str() );

	return *this;
	}




	/*!
	*
	*	methods for comparing
	*
	*/


	/*!
		this method returns true if 'this' is smaller than 'l'

		'index' is an index of the first word from will be the comparison performed
		(note: we start the comparison from back - from the last word, when index is -1 /default/
		it is automatically set into the last word)
		I introduced it for some kind of optimization made in the second division algorithm (Div2)
	*/
	bool CmpSmaller(const UInt<value_size> & l, sint index = -1) const
	{
	sint i;

		if( index==-1 || index>=sint(value_size) )
			i = value_size - 1;
		else
			i = index;


		for( ; i>=0 ; --i)
		{
			if( table[i] != l.table[i] )
				return table[i] < l.table[i];
		}

	// they're equal
	return false;
	}



	/*!
		this method returns true if 'this' is bigger than 'l'

		'index' is an index of the first word from will be the comparison performed
		(note: we start the comparison from back - from the last word, when index is -1 /default/
		it is automatically set into the last word)

		I introduced it for some kind of optimization made in the second division algorithm (Div2)
	*/
	bool CmpBigger(const UInt<value_size> & l, sint index = -1) const
	{
	sint i;

		if( index==-1 || index>=sint(value_size) )
			i = value_size - 1;
		else
			i = index;


		for( ; i>=0 ; --i)
		{
			if( table[i] != l.table[i] )
				return table[i] > l.table[i];
		}

	// they're equal
	return false;
	}


	/*!
		this method returns true if 'this' is equal 'l'

		'index' is an index of the first word from will be the comparison performed
		(note: we start the comparison from back - from the last word, when index is -1 /default/
		it is automatically set into the last word)
	*/
	bool CmpEqual(const UInt<value_size> & l, sint index = -1) const
	{
	sint i;

		if( index==-1 || index>=sint(value_size) )
			i = value_size - 1;
		else
			i = index;


		for( ; i>=0 ; --i)
			if( table[i] != l.table[i] )
				return false;

	return true;
	}



	/*!
		this method returns true if 'this' is smaller than or equal 'l'

		'index' is an index of the first word from will be the comparison performed
		(note: we start the comparison from back - from the last word, when index is -1 /default/
		it is automatically set into the last word)
	*/
	bool CmpSmallerEqual(const UInt<value_size> & l, sint index=-1) const
	{
	sint i;

		if( index==-1 || index>=sint(value_size) )
			i = value_size - 1;
		else
			i = index;


		for( ; i>=0 ; --i)
		{
			if( table[i] != l.table[i] )
				return table[i] < l.table[i];
		}

	// they're equal
	return true;
	}



	/*!
		this method returns true if 'this' is bigger than or equal 'l'

		'index' is an index of the first word from will be the comparison performed
		(note: we start the comparison from back - from the last word, when index is -1 /default/
		it is automatically set into the last word)
	*/
	bool CmpBiggerEqual(const UInt<value_size> & l, sint index=-1) const
	{
	sint i;

		if( index==-1 || index>=sint(value_size) )
			i = value_size - 1;
		else
			i = index;


		for( ; i>=0 ; --i)
		{
			if( table[i] != l.table[i] )
				return table[i] > l.table[i];
		}

	// they're equal
	return true;
	}


	/*
		operators for comparising
	*/

	bool operator<(const UInt<value_size> & l) const
	{
		return CmpSmaller(l);
	}


	bool operator>(const UInt<value_size> & l) const
	{
		return CmpBigger(l);
	}


	bool operator==(const UInt<value_size> & l) const
	{
		return CmpEqual(l);
	}


	bool operator!=(const UInt<value_size> & l) const
	{
		return !operator==(l);
	}


	bool operator<=(const UInt<value_size> & l) const
	{
		return CmpSmallerEqual(l);
	}

	bool operator>=(const UInt<value_size> & l) const
	{
		return CmpBiggerEqual(l);
	}


	/*!
	*
	*	standard mathematical operators 
	*
	*/

	UInt<value_size> operator-(const UInt<value_size> & p2) const
	{
	UInt<value_size> temp(*this);

		temp.Sub(p2);

	return temp;
	}

	UInt<value_size> & operator-=(const UInt<value_size> & p2)
	{
		Sub(p2);

		TTMATH_LOG("UInt::operator-=")

	return *this;
	}

	UInt<value_size> operator+(const UInt<value_size> & p2) const
	{
	UInt<value_size> temp(*this);

		temp.Add(p2);

	return temp;
	}

	UInt<value_size> & operator+=(const UInt<value_size> & p2)
	{
		Add(p2);

		TTMATH_LOG("UInt::operator+=")

	return *this;
	}


	UInt<value_size> operator*(const UInt<value_size> & p2) const
	{
	UInt<value_size> temp(*this);

		temp.Mul(p2);

	return temp;
	}


	UInt<value_size> & operator*=(const UInt<value_size> & p2)
	{
		Mul(p2);

		TTMATH_LOG("UInt::operator*=")

	return *this;
	}


	UInt<value_size> operator/(const UInt<value_size> & p2) const
	{
	UInt<value_size> temp(*this);

		temp.Div(p2);

	return temp;
	}


	UInt<value_size> & operator/=(const UInt<value_size> & p2)
	{
		Div(p2);

		TTMATH_LOG("UInt::operator/=")

	return *this;
	}


	UInt<value_size> operator%(const UInt<value_size> & p2) const
	{
	UInt<value_size> temp(*this);
	UInt<value_size> remainder;
	
		temp.Div( p2, remainder );

	return remainder;
	}


	UInt<value_size> & operator%=(const UInt<value_size> & p2)
	{
	UInt<value_size> temp(*this);
	UInt<value_size> remainder;
	
		temp.Div( p2, remainder );

		operator=(remainder);

		TTMATH_LOG("UInt::operator%=")

	return *this;
	}


	/*!
		Prefix operator e.g ++variable
	*/
	UInt<value_size> & operator++()
	{
		AddOne();

		TTMATH_LOG("UInt::operator++")

	return *this;
	}

	/*!
		Postfix operator e.g variable++
	*/
	UInt<value_size> operator++(int)
	{
	UInt<value_size> temp( *this );

		AddOne();

		TTMATH_LOG("UInt::operator++(int)")

	return temp;
	}


	UInt<value_size> & operator--()
	{
		SubOne();

		TTMATH_LOG("UInt::operator--")

	return *this;
	}


	UInt<value_size> operator--(int)
	{
	UInt<value_size> temp( *this );

		SubOne();

		TTMATH_LOG("UInt::operator--(int)")

	return temp;
	}





	/*!
	*
	*	input/output operators for standard streams
	*	
	*	(they are very simple, in the future they should be changed)
	*
	*/

	friend tostrm_t & operator<<(tostrm_t & s, const UInt<value_size> & l)
	{
	tstr_t ss;

		l.ToString(ss);
		s << ss;

	return s;
	}



	friend tistrm_t & operator>>(tistrm_t & s, UInt<value_size> & l)
	{
	tstr_t ss;
	
	// tchar_t for operator>>
	tchar_t z;
	
		// operator>> omits white characters if they're set for ommiting
		s >> z;

		// we're reading only digits (base=10)
		while( s.good() && CharToDigit(z, 10)>=0 )
		{
			ss += z;
			z = s.get();
		}

		// we're leaving the last readed character
		// (it's not belonging to the value)
		s.unget();

		l.FromString(ss);

		TTMATH_LOG("UInt::operator>>")

	return s;
	}



	/*
		following methods are defined in:
			ttmathuint_x86.h
			ttmathuint_x86_64.h
			ttmathuint_noasm.h
	*/

#ifdef TTMATH_NOASM
	static uint AddTwoWords(uint a, uint b, uint carry, uint * result);
	static uint SubTwoWords(uint a, uint b, uint carry, uint * result);

#ifdef TTMATH_PLATFORM64

	union uint_
	{
		struct 
		{
			unsigned int low;  // 32 bit 
			unsigned int high; // 32 bit
		} u_;

		uint u;                // 64 bit
	};


	static void DivTwoWords2(uint a,uint b, uint c, uint * r, uint * rest);
	static uint DivTwoWordsNormalize(uint_ & a_, uint_ & b_, uint_ & c_);
	static uint DivTwoWordsUnnormalize(uint u, uint d);
	static unsigned int DivTwoWordsCalculate(uint_ u_, unsigned int u3, uint_ v_);
	static void MultiplySubtract(uint_ & u_, unsigned int & u3, unsigned int & q, uint_ v_);

#endif // TTMATH_PLATFORM64
#endif // TTMATH_NOASM


private:
public: // !!! chwilowo public
	uint Rcl2_one(uint c);
	uint Rcr2_one(uint c);
	uint Rcl2(uint bits, uint c);
	uint Rcr2(uint bits, uint c);

public:
	
	uint Add(const UInt<value_size> & ss2, uint c=0);
	uint AddInt(uint value, uint index = 0);
	uint AddTwoInts(uint x2, uint x1, uint index);
	uint Sub(const UInt<value_size> & ss2, uint c=0);
	uint SubInt(uint value, uint index = 0);
	static sint FindLeadingBitInWord(uint x);
	static uint SetBitInWord(uint & value, uint bit);
	static void MulTwoWords(uint a, uint b, uint * result_high, uint * result_low);
	static void DivTwoWords(uint a,uint b, uint c, uint * r, uint * rest);
};


} //namespace

#if defined(_MSC_VER)
	#pragma warning(default:4127) // conditional expression is constant
#endif


#include "ttmathuint_x86.h"
#include "ttmathuint_x86_64.h"
#include "ttmathuint_noasm.h"

#endif
