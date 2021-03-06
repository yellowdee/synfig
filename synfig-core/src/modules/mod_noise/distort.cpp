/* === S Y N F I G ========================================================= */
/*!	\file distort.cpp
**	\brief Implementation of the "Noise Distort" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007 Chris Moore
**	Copyright (c) 2011 Carlos López
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "distort.h"

#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>

#endif

/* === M A C R O S ========================================================= */

using namespace synfig;
using namespace std;
using namespace etl;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(NoiseDistort);
SYNFIG_LAYER_SET_NAME(NoiseDistort,"noise_distort");
SYNFIG_LAYER_SET_LOCAL_NAME(NoiseDistort,N_("Noise Distort"));
SYNFIG_LAYER_SET_CATEGORY(NoiseDistort,N_("Distortions"));
SYNFIG_LAYER_SET_VERSION(NoiseDistort,"0.0");
SYNFIG_LAYER_SET_CVS_ID(NoiseDistort,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

NoiseDistort::NoiseDistort():
	size(1,1)
{
	set_blend_method(Color::BLEND_STRAIGHT);
	smooth=RandomNoise::SMOOTH_COSINE;
	detail=4;
	speed=0;
	random.set_seed(time(NULL));
	turbulent=false;
	displacement=Vector(0.25,0.25);
	Layer::Vocab voc(get_param_vocab());
	Layer::fill_static(voc);
}

inline Color
NoiseDistort::color_func(const Point &point, float /*supersample*/,Context context)const
{
	Color ret(0,0,0,0);

	float x(point[0]/size[0]*(1<<detail));
	float y(point[1]/size[1]*(1<<detail));

	int i;
	Time time;
	time=speed*curr_time;
	RandomNoise::SmoothType temp_smooth(smooth);
	RandomNoise::SmoothType smooth((!speed && temp_smooth == RandomNoise::SMOOTH_SPLINE) ? RandomNoise::SMOOTH_FAST_SPLINE : temp_smooth);

	{
		Vector vect(0,0);
		for(i=0;i<detail;i++)
		{
			vect[0]=random(smooth,0+(detail-i)*5,x,y,time)+vect[0]*0.5;
			vect[1]=random(smooth,1+(detail-i)*5,x,y,time)+vect[1]*0.5;

			if(vect[0]<-1)vect[0]=-1;if(vect[0]>1)vect[0]=1;
			if(vect[1]<-1)vect[1]=-1;if(vect[1]>1)vect[1]=1;

			if(turbulent)
			{
				vect[0]=abs(vect[0]);
				vect[1]=abs(vect[1]);
			}

			x/=2.0f;
			y/=2.0f;
		}

		if(!turbulent)
		{
			vect[0]=vect[0]/2.0f+0.5f;
			vect[1]=vect[1]/2.0f+0.5f;
		}
		vect[0]=(vect[0]-0.5f)*displacement[0];
		vect[1]=(vect[1]-0.5f)*displacement[1];

		ret=context.get_color(point+vect);
	}
	return ret;
}

inline float
NoiseDistort::calc_supersample(const synfig::Point &/*x*/, float /*pw*/,float /*ph*/)const
{
	return 0.0f;
}

void
NoiseDistort::set_time(synfig::Context context, synfig::Time t)const
{
	curr_time=t;
	context.set_time(t);
}

void
NoiseDistort::set_time(synfig::Context context, synfig::Time t, const synfig::Point &point)const
{
	curr_time=t;
	context.set_time(t,point);
}

synfig::Layer::Handle
NoiseDistort::hit_check(synfig::Context context, const synfig::Point &point)const
{
	if(get_blend_method()==Color::BLEND_STRAIGHT && get_amount()>=0.5)
		return const_cast<NoiseDistort*>(this);
	if(get_amount()==0.0)
		return context.hit_check(point);
	if(color_func(point,0,context).get_a()>0.5)
		return const_cast<NoiseDistort*>(this);
	return false;
}

bool
NoiseDistort::set_param(const String & param, const ValueBase &value)
{
	if(param=="seed" && value.same_type_as(int()))
	{
		random.set_seed(value.get(int()));
		set_param_static(param, value.get_static());
		return true;
	}
	IMPORT(size);
	IMPORT(speed);
	IMPORT(smooth);
	IMPORT(detail);
	IMPORT(turbulent);
	IMPORT(displacement);
	return Layer_Composite::set_param(param,value);
}

ValueBase
NoiseDistort::get_param(const String & param)const
{
	if(param=="seed")
	{
		ValueBase ret(random.get_seed());
		ret.set_static(get_param_static(param));
		return ret;
	}
	EXPORT(size);
	EXPORT(speed);
	EXPORT(smooth);
	EXPORT(detail);
	EXPORT(displacement);
	EXPORT(turbulent);

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
NoiseDistort::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("displacement")
		.set_local_name(_("Displacement"))
		.set_description(_("How big the distortion displaces the context"))
	);

	ret.push_back(ParamDesc("size")
		.set_local_name(_("Size"))
		.set_description(_("The distance between distortions"))
	);
	ret.push_back(ParamDesc("seed")
		.set_local_name(_("RandomNoise Seed"))
		.set_description(_("Change to modify the random seed of the noise"))
	);
	ret.push_back(ParamDesc("smooth")
		.set_local_name(_("Interpolation"))
		.set_description(_("What type of interpolation to use"))
		.set_hint("enum")
		.add_enum_value(RandomNoise::SMOOTH_DEFAULT,	"nearest",	_("Nearest Neighbor"))
		.add_enum_value(RandomNoise::SMOOTH_LINEAR,	"linear",	_("Linear"))
		.add_enum_value(RandomNoise::SMOOTH_COSINE,	"cosine",	_("Cosine"))
		.add_enum_value(RandomNoise::SMOOTH_SPLINE,	"spline",	_("Spline"))
		.add_enum_value(RandomNoise::SMOOTH_CUBIC,	"cubic",	_("Cubic"))
	);
	ret.push_back(ParamDesc("detail")
		.set_local_name(_("Detail"))
		.set_description(_("Increase to obtain fine details of the noise"))
	);
	ret.push_back(ParamDesc("speed")
		.set_local_name(_("Animation Speed"))
		.set_description(_("In cycles per second"))
	);
	ret.push_back(ParamDesc("turbulent")
		.set_local_name(_("Turbulent"))
		.set_description(_("When checked produces turbulent noise"))
	);

	return ret;
}

Color
NoiseDistort::get_color(Context context, const Point &point)const
{
	const Color color(color_func(point,0,context));

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
		return color;
	else
		return Color::blend(color,context.get_color(point),get_amount(),get_blend_method());
}

Rect
NoiseDistort::get_bounding_rect(Context context)const
{
	if(is_disabled())
		return Rect::zero();

	if(Color::is_onto(get_blend_method()))
		return context.get_full_bounding_rect();

	Rect bounds(context.get_full_bounding_rect().expand_x(displacement[0]).expand_y(displacement[1]));

	return bounds;
}

/*
bool
NoiseDistort::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	SuperCallback supercb(cb,0,9500,10000);

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		surface->set_wh(renddesc.get_w(),renddesc.get_h());
	}
	else
	{
		if(!context.accelerated_render(surface,quality,renddesc,&supercb))
			return false;
		if(get_amount()==0)
			return true;
	}


	int x,y;

	Surface::pen pen(surface->begin());
	const Real pw(renddesc.get_pw()),ph(renddesc.get_ph());
	Point pos;
	Point tl(renddesc.get_tl());
	const int w(surface->get_w());
	const int h(surface->get_h());

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
			for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
				pen.put_value(color_func(pos,calc_supersample(pos,pw,ph),context));
	}
	else
	{
		for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
			for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
				pen.put_value(Color::blend(color_func(pos,calc_supersample(pos,pw,ph),context),pen.get_value(),get_amount(),get_blend_method()));
	}

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}
*/
