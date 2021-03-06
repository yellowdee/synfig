/* === S Y N F I G ========================================================= */
/*!	\file synfigapp/instance.h
**	\brief writeme
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
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

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_APP_INSTANCE_H
#define __SYNFIG_APP_INSTANCE_H

/* === H E A D E R S ======================================================= */

#include "action.h"
#include <ETL/handle>
#include <synfig/canvas.h>
#include <synfig/string.h>
#include <list>
#include <sigc++/signal.h>
#include <sigc++/object.h>
#include "action_system.h"
#include "selectionmanager.h"
#include "cvs.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfigapp {

class CanvasInterface;


class Instance : public Action::System , public CVSInfo
{
	friend class PassiveGrouper;
	/*
 -- ** -- P U B L I C   T Y P E S ---------------------------------------------
	*/

public:

	typedef std::list< etl::handle<CanvasInterface> > CanvasInterfaceList;

	using etl::shared_object::ref;
	using etl::shared_object::unref;

	/*
 -- ** -- P U B L I C  D A T A ------------------------------------------------
	*/

public:

	/*
 -- ** -- P R I V A T E   D A T A ---------------------------------------------
	*/

private:
	//! Handle for root canvas
	synfig::Canvas::Handle canvas_;


	CanvasInterfaceList canvas_interface_list_;

	sigc::signal<void> signal_filename_changed_;
	sigc::signal<void> signal_saved_;
	etl::handle<SelectionManager> selection_manager_;

protected:
	Instance(etl::handle<synfig::Canvas>);

	/*
 -- ** -- P U B L I C   M E T H O D S -----------------------------------------
	*/

public:

	~Instance();

	void set_selection_manager(const etl::handle<SelectionManager> &sm) { assert(sm); selection_manager_=sm; }
	void unset_selection_manager() { selection_manager_=new NullSelectionManager(); }
	const etl::handle<SelectionManager> &get_selection_manager() { return selection_manager_; }



	etl::handle<CanvasInterface> find_canvas_interface(synfig::Canvas::Handle canvas);

	synfig::Canvas::Handle get_canvas()const { return canvas_; }

	//! Saves the instance to filename_
	bool save()const;

	bool save_as(const synfig::String &filename);

public:	// Interfaces to internal information
	sigc::signal<void>& signal_filename_changed() { return signal_filename_changed_; }
	sigc::signal<void>& signal_saved() { return signal_saved_; }

	CanvasInterfaceList & canvas_interface_list() { return canvas_interface_list_; }
	const CanvasInterfaceList & canvas_interface_list()const { return canvas_interface_list_; }

	synfig::String get_file_name()const;

	void set_file_name(const synfig::String &name);

public:


public:	// Constructor interfaces
	static etl::handle<Instance> create(etl::handle<synfig::Canvas> canvas);
}; // END class Instance

etl::handle<Instance> find_instance(etl::handle<synfig::Canvas> canvas);

bool is_editable(synfig::ValueNode::Handle value_node);

}; // END namespace studio

/* === E N D =============================================================== */

#endif
