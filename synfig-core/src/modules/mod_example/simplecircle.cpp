/* === S Y N F I G ========================================================= */
/*!	\file simplecircle.cpp
**	\brief Implementation of the "Simple Circle" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2008 Chris Moore
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

#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>

#include "simplecircle.h"

#endif

/* === U S I N G =========================================================== */

using namespace etl;
using namespace std;
using namespace synfig;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(SimpleCircle);
SYNFIG_LAYER_SET_NAME(SimpleCircle,"simple_circle");
SYNFIG_LAYER_SET_LOCAL_NAME(SimpleCircle,N_("Simple Circle"));
SYNFIG_LAYER_SET_CATEGORY(SimpleCircle,N_("Example"));
SYNFIG_LAYER_SET_VERSION(SimpleCircle,"0.1");
SYNFIG_LAYER_SET_CVS_ID(SimpleCircle,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

SimpleCircle::SimpleCircle():
	Layer_Composite(1.0,Color::BLEND_COMPOSITE),
	color(Color::black()),
	center(0,0),
	radius(0.5)
{
	Layer::Vocab voc(get_param_vocab());
	Layer::fill_static(voc);
}

bool
SimpleCircle::set_param(const String & param, const ValueBase &value)
{
	IMPORT(color);
	IMPORT(center);
	IMPORT(radius);

	return Layer_Composite::set_param(param,value);
}

ValueBase
SimpleCircle::get_param(const String &param)const
{
	EXPORT(color);
	EXPORT(center);
	EXPORT(radius);

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
SimpleCircle::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("color")
		.set_local_name(_("Color"))
		.set_description(_("Fill color of the layer"))
	);

	ret.push_back(ParamDesc("center")
		.set_local_name(_("Center"))
		.set_description(_("Center of the circle"))
	);

	ret.push_back(ParamDesc("radius")
		.set_local_name(_("Radius"))
		.set_description(_("This is the radius of the circle"))
		.set_origin("center")
	);

	return ret;
}

Color
SimpleCircle::get_color(Context context, const Point &pos)const
{

	if((pos-center).mag()<radius)
	{
		if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
			return color;
		else
			return Color::blend(color,context.get_color(pos),get_amount(),get_blend_method());
	}
	else
		return context.get_color(pos);
}

synfig::Layer::Handle
SimpleCircle::hit_check(synfig::Context context, const synfig::Point &pos)const
{
	if((pos-center).mag()<radius)
		return const_cast<SimpleCircle*>(this);
	else
		return context.hit_check(pos);
}

/*
bool
SimpleCircle::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		// Mark our progress as starting
		if(cb && !cb->amount_complete(0,1000))
			return false;

		surface->set_wh(renddesc.get_w(),renddesc.get_h());
		surface->fill(color);

		// Mark our progress as finished
		if(cb && !cb->amount_complete(1000,1000))
			return false;

		return true;
	}

	SuperCallback supercb(cb,0,9500,10000);

	if(!context.accelerated_render(surface,quality,renddesc,&supercb))
		return false;

	int x,y;

	Surface::alpha_pen apen(surface->begin());

	apen.set_value(color);
	apen.set_alpha(get_amount());
	apen.set_blend_method(get_blend_method());

	for(y=0;y<renddesc.get_h();y++,apen.inc_y(),apen.dec_x(x))
		for(x=0;x<renddesc.get_w();x++,apen.inc_x())
			apen.put_value();

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}
*/
