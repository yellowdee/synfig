/* === S Y N F I G ========================================================= */
/*!	\file rectangle.cpp
**	\brief Implementation of the "Rectangle" layer
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**	Copyright (c) 2007, 2008 Chris Moore
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
#include <ETL/pen>
#include <ETL/misc>

#include "rectangle.h"

#endif

/* === U S I N G =========================================================== */

using namespace etl;
using namespace std;
using namespace synfig;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(Rectangle);
SYNFIG_LAYER_SET_NAME(Rectangle,"rectangle");
SYNFIG_LAYER_SET_LOCAL_NAME(Rectangle,N_("Rectangle"));
SYNFIG_LAYER_SET_CATEGORY(Rectangle,N_("Geometry"));
SYNFIG_LAYER_SET_VERSION(Rectangle,"0.2");
SYNFIG_LAYER_SET_CVS_ID(Rectangle,"$Id$");

/* === P R O C E D U R E S ================================================= */

/*
inline int ceil_to_int(const float x) { return static_cast<int>(ceil(x)); }
inline int ceil_to_int(const double x) { return static_cast<int>(ceil(x)); }

inline int floor_to_int(const float x) { return static_cast<int>(floor(x)); }
inline int floor_to_int(const double x) { return static_cast<int>(floor(x)); }
*/

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

Rectangle::Rectangle():
	Layer_Composite(1.0,Color::BLEND_COMPOSITE),
	color(Color::black()),
	point1(0,0),
	point2(1,1),
	expand(0),
	invert(false)
{
	Layer::Vocab voc(get_param_vocab());
	Layer::fill_static(voc);
}

bool
Rectangle::set_param(const String & param, const ValueBase &value)
{
	IMPORT_PLUS(color, { if (color.get_a() == 0) { if (converted_blend_) {
					set_blend_method(Color::BLEND_ALPHA_OVER);
					color.set_a(1); } else transparent_color_ = true; } });
	IMPORT(point1);
	IMPORT(point2);
	IMPORT(expand);
	IMPORT(invert);

	return Layer_Composite::set_param(param,value);
}

ValueBase
Rectangle::get_param(const String &param)const
{
	EXPORT(color);
	EXPORT(point1);
	EXPORT(point2);
	EXPORT(expand);
	EXPORT(invert);

	EXPORT_NAME();
	EXPORT_VERSION();

	return Layer_Composite::get_param(param);
}

Layer::Vocab
Rectangle::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());

	ret.push_back(ParamDesc("color")
		.set_local_name(_("Color"))
		.set_description(_("Fill color of the layer"))
	);

	ret.push_back(ParamDesc("point1")
		.set_local_name(_("Point 1"))
		.set_box("point2")
		.set_description(_("First corner of the rectangle"))
	);

	ret.push_back(ParamDesc("point2")
		.set_local_name(_("Point 2"))
		.set_description(_("Second corner of the rectangle"))
	);

	ret.push_back(ParamDesc("expand")
		.set_is_distance()
		.set_local_name(_("Expand amount"))
	);

	ret.push_back(ParamDesc("invert")
		.set_local_name(_("Invert the rectangle"))
	);

	return ret;
}

synfig::Layer::Handle
Rectangle::hit_check(synfig::Context context, const synfig::Point &pos)const
{
	if(is_disabled())
		return context.hit_check(pos);

	Point max,min;

	max[0]=std::max(point1[0],point2[0])+expand;
	max[1]=std::max(point1[1],point2[1])+expand;
	min[0]=std::min(point1[0],point2[0])-expand;
	min[1]=std::min(point1[1],point2[1])-expand;

	bool intersect(false);

	if(	pos[0]<max[0] && pos[0]>min[0] &&
		pos[1]<max[1] && pos[1]>min[1] )
	{
		intersect=true;
	}

	if(invert)
		intersect=!intersect;

	if(intersect)
	{
		synfig::Layer::Handle tmp;
		if(get_blend_method()==Color::BLEND_BEHIND && (tmp=context.hit_check(pos)))
			return tmp;
		if(Color::is_onto(get_blend_method()) && !(tmp=context.hit_check(pos)))
			return 0;
		return const_cast<Rectangle*>(this);
	}

	return context.hit_check(pos);
}

bool
Rectangle::is_solid_color()const
{
	return Layer_Composite::is_solid_color() ||
		(get_blend_method() == Color::BLEND_COMPOSITE &&
		 get_amount() == 1.0f &&
		 color.get_a() == 1.0f);
}

Color
Rectangle::get_color(Context context, const Point &pos)const
{
	if(is_disabled())
		return context.get_color(pos);

	Point max,min;

	max[0]=std::max(point1[0],point2[0])+expand;
	max[1]=std::max(point1[1],point2[1])+expand;
	min[0]=std::min(point1[0],point2[0])-expand;
	min[1]=std::min(point1[1],point2[1])-expand;

/**************************
// This is darco's old-old-old feathered box code
// it produces really nice feathered edges
	if(feather!=0.0)
	{
		if(	pos[0]<=max[0]-feather/2.0 && pos[0]>=min[0]+feather/2.0 &&
			pos[1]<=max[1]-feather/2.0 && pos[1]>=min[1]+feather/2.0 )
		{
			if(invert)
				return (*context).GetColor(context,pos);
			else
				return color;
		}

		if(	pos[0]>=max[0]+feather/2.0 || pos[0]<=min[0]-feather/2.0 ||
			pos[1]>=max[1]+feather/2.0 || pos[1]<=min[1]-feather/2.0 )
		{
			if(invert)
				return color;
			else
				return (*context).GetColor(context,pos);
		}

		Color::unit alpha=1000000;
		Color::unit alpha2=1000000;

		if(max[0]-pos[0]+feather/2.0<alpha)
			alpha=max[0]-pos[0]+feather/2.0;
		if(pos[0]-min[0]+feather/2.0<alpha)
			alpha=pos[0]-min[0]+feather/2.0;

		if(max[1]-pos[1]+feather/2.0<alpha2)
			alpha2=max[1]-pos[1]+feather/2.0;
		if(pos[1]-min[1]+feather/2.0<alpha2)
			alpha2=pos[1]-min[1]+feather/2.0;


		if(alpha<=feather && alpha2<=feather)
		{
			alpha=feather-alpha;
			alpha2=feather-alpha2;

			alpha=sqrt(alpha*alpha+alpha2*alpha2);

			if(alpha>=feather)
			{
				if(invert)
					return color;
				else
					return (*context).GetColor(context,pos);
			}

			alpha=feather-alpha;
		}
		else
		{
			alpha=(alpha<alpha2)?alpha:alpha2;
		}

		alpha/=feather;

		if(invert)
			alpha=1.0-alpha;

		return Color::blend(color,context.get_color(pos),alpha,get_blend_method());
	}

*****************/

	if(	pos[0]<max[0] && pos[0]>min[0] &&
		pos[1]<max[1] && pos[1]>min[1] )
	{
		// inside the expanded rectangle
		if(invert)
			return Color::blend(Color::alpha(),context.get_color(pos),get_amount(),get_blend_method());

		if(is_solid_color())
			return color;

		return Color::blend(color,context.get_color(pos),get_amount(),get_blend_method());
	}
	else
	{
		// outside the expanded rectangle
		if(!invert)
			return Color::blend(Color::alpha(),context.get_color(pos),get_amount(),get_blend_method());

		if(is_solid_color())
			return color;

		return Color::blend(color,context.get_color(pos),get_amount(),get_blend_method());
	}
}

bool
Rectangle::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	if(is_disabled())
		return context.accelerated_render(surface,quality,renddesc,cb);

	const Point	tl(renddesc.get_tl());
	const Point br(renddesc.get_br());

	const int	w(renddesc.get_w());
	const int	h(renddesc.get_h());

	// Width and Height of a pixel
	const Real pw = (br[0] - tl[0]) / w;
	const Real ph = (br[1] - tl[1]) / h;

	Point max(point1),min(point2);




	/*

	if(invert)
	{
		max=context.get_bounding_rect().get_max();
		min=context.get_bounding_rect().get_min();
	}
	else
	{
		max=context.get_full_bounding_rect().get_max();
		min=context.get_full_bounding_rect().get_min();
	}
	*/





	if((min[0] > max[0]) ^ (pw < 0))swap(min[0],max[0]);
	if((min[1] > max[1]) ^ (ph < 0))swap(min[1],max[1]);

	if(min[0] > max[0])
	{
		min[0]+=expand;
		max[0]-=expand;
	}
	else
	{
		min[0]-=expand;
		max[0]+=expand;
	}

	if(min[1] > max[1])
	{
		min[1]+=expand;
		max[1]-=expand;
	}
	else
	{
		min[1]-=expand;
		max[1]+=expand;
	}

	if(invert)
	{
		int left(floor_to_int((min[0]-tl[0])/pw));
		int right(ceil_to_int((max[0]-tl[0])/pw));
		int top(floor_to_int((min[1]-tl[1])/ph));
		int bottom(ceil_to_int((max[1]-tl[1])/ph));

		float left_edge((min[0]-tl[0])/pw-float(left));
		float right_edge(float(right)-(max[0]-tl[0])/pw);
		float top_edge((min[1]-tl[1])/ph-float(top));
		float bottom_edge(float(bottom)-(max[1]-tl[1])/ph);

		if(top<0)top=0,top_edge=0;
		if(left<0)left=0,left_edge=0;
		if(bottom>h)bottom=h,bottom_edge=0;
		if(right>w)right=w,right_edge=0;

		if(is_solid_color())
		{
			Surface subimage;
			RendDesc desc(renddesc);
			desc.set_flags(0);

			//fill the surface with the background color initially
			surface->set_wh(w,h);
			surface->fill(color);

			// Check for the case where there is nothing to render
			if (right <= left || bottom <= top)
				return true;

			desc.set_subwindow(left,top,right-left,bottom-top);

			// Render what is behind us
			if(!context.accelerated_render(&subimage,quality,desc,cb))
			{
				if(cb)cb->error(strprintf(__FILE__"%d: Accelerated Renderer Failure",__LINE__));
				return false;
			}

			Surface::pen pen(surface->get_pen(left,top));

			subimage.blit_to(pen);
		}
		else
		{
			if(!context.accelerated_render(surface,quality,renddesc,cb))
			{
				if(cb)cb->error(strprintf(__FILE__"%d: Accelerated Renderer Failure",__LINE__));
				return false;
			}

			Surface subimage;

			// Check for the case where there is something to render
			if (right > left && bottom > top)
			{
				// save a copy of the overlapping region from surface into subimage
				subimage.set_wh(right-left,bottom-top);
				Surface::pen subimage_pen(subimage.begin());
				surface->blit_to(subimage_pen,left,top,right-left,bottom-top);
			}

			// fill surface with the rectangle's color
			Surface::alpha_pen surface_pen(surface->begin(),get_amount(),get_blend_method());
			surface->fill(color,surface_pen,w,h);

			if (subimage)
			{
				// copy the saved overlapping region back from subimage into surface
				Surface::pen pen(surface->get_pen(left,top));
				subimage.blit_to(pen);
			}
			else
				// if there's no overlapping region, return now of the following code corrupts memory
				return true;
		}

		Surface::alpha_pen pen;

		if(bottom-1>=0 && bottom_edge)
		{
			pen=Surface::alpha_pen(surface->get_pen(left,bottom-1),get_amount()*bottom_edge,get_blend_method());
			surface->fill(color,pen,right-left,1);
		}

		if(right-1>=0 && right_edge)
		{
			pen=Surface::alpha_pen(surface->get_pen(right-1,top),get_amount()*right_edge,get_blend_method());
			surface->fill(color,pen,1,bottom-top);
		}

		if(left>=0 && left_edge)
		{
			pen=Surface::alpha_pen(surface->get_pen(left,top),get_amount()*left_edge,get_blend_method());
			surface->fill(color,pen,1,bottom-top);
		}

		if(top>=0 && top_edge)
		{
			pen=Surface::alpha_pen(surface->get_pen(left,top),get_amount()*top_edge,get_blend_method());
			surface->fill(color,pen,right-left,1);
		}

		return true;
	}

	// not inverted

	int left(ceil_to_int((min[0]-tl[0])/pw));
	int right(floor_to_int((max[0]-tl[0])/pw));
	int top(ceil_to_int((min[1]-tl[1])/ph));
	int bottom(floor_to_int((max[1]-tl[1])/ph));

	float left_edge(float(left)-(min[0]-tl[0])/pw);
	float right_edge((max[0]-tl[0])/pw-float(right));
	float top_edge(float(top)-(min[1]-tl[1])/ph);
	float bottom_edge((max[1]-tl[1])/ph-float(bottom));

	if(top<=0)top=0,top_edge=0;
	if(left<=0)left=0,left_edge=0;
	if(bottom>=h)bottom=h,bottom_edge=0;
	if(right>=w)right=w,right_edge=0;

/*
	top = std::max(0,top);
	left = std::max(0,left);
	bottom = std::min(h,bottom);
	right = std::min(w,right);
*/

	// optimization - if the whole tile is covered by this rectangle,
	// and the rectangle is a solid color, we don't need to render
	// what's behind us
	if (is_solid_color() && top == 0 && left == 0 && bottom == h && right == w)
	{
		surface->set_wh(w,h);
		surface->fill(color);
		return true;
	}

	// Render what is behind us
	if(!context.accelerated_render(surface,quality,renddesc,cb))
	{
		if(cb)cb->error(strprintf(__FILE__"%d: Accelerated Renderer Failure",__LINE__));
		return false;
	}

	// In the case where there is nothing to render...
	if (right < left || bottom < top)
		return true;

	Surface::alpha_pen pen;

	if(right-left>0&&bottom-top>0)
	{
		if(is_solid_color())
			surface->fill(color,left,top,right-left,bottom-top);
		else
		{
			pen=Surface::alpha_pen(surface->get_pen(left,top),get_amount(),get_blend_method());
			surface->fill(color,pen,right-left,bottom-top);
		}
	}

	if(bottom<surface->get_h() && bottom_edge>=0.0001)
	{
		pen=Surface::alpha_pen(surface->get_pen(left,bottom),get_amount()*bottom_edge,get_blend_method());
		surface->fill(color,pen,right-left,1);
	}

	if(right<surface->get_w() && right_edge>=0.0001)
	{
		pen=Surface::alpha_pen(surface->get_pen(right,top),get_amount()*right_edge,get_blend_method());
		surface->fill(color,pen,1,bottom-top);
	}

	if(left>0 && left_edge>=0.0001)
	{
		pen=Surface::alpha_pen(surface->get_pen(left-1,top),get_amount()*left_edge,get_blend_method());
		surface->fill(color,pen,1,bottom-top);
	}

	if(top>0 && top_edge>=0.0001)
	{
		pen=Surface::alpha_pen(surface->get_pen(left,top-1),get_amount()*top_edge,get_blend_method());
		surface->fill(color,pen,right-left,1);
	}


	return true;
}

Rect
Rectangle::get_bounding_rect()const
{
	if(invert)
		return Rect::full_plane();

	Point max(point1),min(point2);
	if((min[0] > max[0]))swap(min[0],max[0]);
	if((min[1] > max[1]))swap(min[1],max[1]);
	if(min[0] > max[0])
	{
		min[0]+=expand;
		max[0]-=expand;
	}
	else
	{
		min[0]-=expand;
		max[0]+=expand;
	}

	if(min[1] > max[1])
	{
		min[1]+=expand;
		max[1]-=expand;
	}
	else
	{
		min[1]-=expand;
		max[1]+=expand;
	}

	Rect bounds(min,max);

	return bounds;
}

Rect
Rectangle::get_full_bounding_rect(Context context)const
{
	if(invert)
	{
		if(is_solid_color() && color.get_a()==0)
		{
			Point max(point1),min(point2);
			if((min[0] > max[0]))swap(min[0],max[0]);
			if((min[1] > max[1]))swap(min[1],max[1]);
			if(min[0] > max[0])
			{
				min[0]+=expand;
				max[0]-=expand;
			}
			else
			{
				min[0]-=expand;
				max[0]+=expand;
			}

			if(min[1] > max[1])
			{
				min[1]+=expand;
				max[1]-=expand;
			}
			else
			{
				min[1]-=expand;
				max[1]+=expand;
			}

			Rect bounds(min,max);

			return bounds & context.get_full_bounding_rect();
		}
		return Rect::full_plane();
	}

	return Layer_Composite::get_full_bounding_rect(context);
}
