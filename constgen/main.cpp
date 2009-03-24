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

/*
  this simple program is used to make constants which then are put into ttmathbig.h
*/


#include "../ttmath/ttmath.h"
#include <iostream>


void CalcPi()
{
	ttmath::Big<1,400> pi;
	
	// 3100 digits after commna, taken from: http://zenwerx.com/pi.php
	pi = "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679"
	"8214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196"
	"4428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273"
	"7245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094"
	"3305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912"
	"9833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132"
	"0005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235"
	"4201995611212902196086403441815981362977477130996051870721134999999837297804995105973173281609631859"
	"5024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303"
	"5982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989"
	"3809525720106548586327886593615338182796823030195203530185296899577362259941389124972177528347913151"
	"5574857242454150695950829533116861727855889075098381754637464939319255060400927701671139009848824012"
	"8583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912"
	"9331367702898915210475216205696602405803815019351125338243003558764024749647326391419927260426992279"
	"6782354781636009341721641219924586315030286182974555706749838505494588586926995690927210797509302955"
	"3211653449872027559602364806654991198818347977535663698074265425278625518184175746728909777727938000"
	"8164706001614524919217321721477235014144197356854816136115735255213347574184946843852332390739414333"
	"4547762416862518983569485562099219222184272550254256887671790494601653466804988627232791786085784383"
	"8279679766814541009538837863609506800642251252051173929848960841284886269456042419652850222106611863"
	"0674427862203919494504712371378696095636437191728746776465757396241389086583264599581339047802759009"
	"9465764078951269468398352595709825822620522489407726719478268482601476990902640136394437455305068203"
	"4962524517493996514314298091906592509372216964615157098583874105978859597729754989301617539284681382"
	"6868386894277415599185592524595395943104997252468084598727364469584865383673622262609912460805124388"
	"4390451244136549762780797715691435997700129616089441694868555848406353422072225828488648158456028506"
	"0168427394522674676788952521385225499546667278239864565961163548862305774564980355936345681743241125"
	"1507606947945109659609402522887971089314566913686722874894056010150330861792868092087476091782493858"
	"9009714909675985261365549781893129784821682998948722658804857564014270477555132379641451523746234364"
	"5428584447952658678210511413547357395231134271661021359695362314429524849371871101457654035902799344"
	"0374200731057853906219838744780847848968332144571386875194350643021845319104848100537061468067491927"
	"8191197939952061419663428754440643745123718192179998391015919561814675142691239748940907186494231961"
	"5679452080951465502252316038819301420937621378559566389377870830390697920773467221825625996615014215";

	std::cout << "---------------- PI ----------------" << std::endl;
	pi.mantissa.PrintTable(std::cout);
}


void CalcE()
{
	ttmath::Big<1,400> e;
	ttmath::uint steps;

	// macro CONSTANTSGENERATOR has to be defined	
	e.ExpSurrounding0(1, &steps);
	std::cout << "---------------- e ----------------" << std::endl;
	e.mantissa.PrintTable(std::cout);
	
	std::cout << "ExpSurrounding0(1): " << steps << " iterations" << std::endl;
}


void CalcLn(int x)
{
	ttmath::Big<1,400> ln;
	ttmath::uint steps;

	// macro CONSTANTSGENERATOR has to be defined	
	ln.LnSurrounding1(x, &steps);
	std::cout << "---------------- ln(" << x << ") ----------------" << std::endl;
	ln.mantissa.PrintTable(std::cout);
	
	std::cout << "LnSurrounding1(" << x << "): " << steps << " iterations" << std::endl;
}



int main()
{
	CalcPi();
	CalcE();
	CalcLn(2);
	CalcLn(10);
		
return 0;
}