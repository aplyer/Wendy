///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL Mathematics Copyright (c) 2005 - 2011 G-Truc Creation (www.g-truc.net)
///////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2009-10-26
// Updated : 2011-06-07
// Licence : This source is under MIT License
// File    : glm/gtx/multiple.inl
///////////////////////////////////////////////////////////////////////////////////////////////////
// Dependency:
// - GLM core
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../core/_vectorize.hpp"

namespace glm
{
	//////////////////////
	// higherMultiple

	template <typename genType> 
	GLM_FUNC_QUALIFIER genType higherMultiple
	(
		genType const & Source, 
		genType const & Multiple
	)
	{
		genType Tmp = Source % Multiple;
		return Tmp ? Source + Multiple - Tmp : Source;
	}

	template <> 
	GLM_FUNC_QUALIFIER detail::thalf higherMultiple
	(
		detail::thalf const & SourceH, 
		detail::thalf const & MultipleH
	)
	{
		float Source = SourceH.toFloat();
		float Multiple = MultipleH.toFloat();

		int Tmp = int(float(Source)) % int(Multiple);
		return detail::thalf(Tmp ? Source + Multiple - float(Tmp) : Source);
	}

	template <> 
	GLM_FUNC_QUALIFIER float higherMultiple
	(	
		float const & Source, 
		float const & Multiple
	)
	{
		int Tmp = int(Source) % int(Multiple);
		return Tmp ? Source + Multiple - float(Tmp) : Source;
	}

	template <> 
	GLM_FUNC_QUALIFIER double higherMultiple
	(
		double const & Source, 
		double const & Multiple
	)
	{
		long Tmp = long(Source) % long(Multiple);
		return Tmp ? Source + Multiple - double(Tmp) : Source;
	}

	VECTORIZE_VEC_VEC(higherMultiple)

	//////////////////////
	// lowerMultiple

	template <typename genType> 
	GLM_FUNC_QUALIFIER genType lowerMultiple
	(
		genType const & Source, 
		genType const & Multiple
	)
	{
		genType Tmp = Source % Multiple;
		return Tmp ? Source - Tmp : Source;
	}

	template <> 
	GLM_FUNC_QUALIFIER detail::thalf lowerMultiple
	(
		detail::thalf const & SourceH, 
		detail::thalf const & MultipleH
	)
	{
		float Source = SourceH.toFloat();
		float Multiple = MultipleH.toFloat();

		int Tmp = int(float(Source)) % int(float(Multiple));
		return detail::thalf(Tmp ? Source - float(Tmp) : Source);
	}

	template <> 
	GLM_FUNC_QUALIFIER float lowerMultiple
	(
		float const & Source, 
		float const & Multiple
	)
	{
		int Tmp = int(Source) % int(Multiple);
		return Tmp ? Source - float(Tmp) : Source;
	}

	template <> 
	GLM_FUNC_QUALIFIER double lowerMultiple
	(
		double const & Source, 
		double const & Multiple
	)
	{
		long Tmp = long(Source) % long(Multiple);
		return Tmp ? Source - double(Tmp) : Source;
	}

	VECTORIZE_VEC_VEC(lowerMultiple)
}//namespace glm
