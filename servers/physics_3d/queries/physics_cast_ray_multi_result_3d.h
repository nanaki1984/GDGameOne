/**************************************************************************/
/*  physics_intersect_shape_result_3d.h                                   */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/object/ref_counted.h"
#include "servers/physics_3d/physics_server_3d_types.h"

class PhysicsCastRayMultiResult3D : public RefCounted {
	GDCLASS(PhysicsCastRayMultiResult3D, RefCounted);

	friend class PhysicsDirectSpaceState3D;

	LocalVector<PS3DT::RayResult> result;
	int hit_count = 0;

protected:
	static void _bind_methods();

public:
	PhysicsCastRayMultiResult3D(int p_max_hits = 32);

	int get_max_hits() const;
	void set_max_hits(int p_max_hits);

	int get_hit_count() const;

    Vector3 get_position(int p_hit_index) const;
	Vector3 get_normal(int p_hit_index) const;
	RID get_collider_rid(int p_hit_index) const;
	ObjectID get_collider_id(int p_hit_index) const;
	Object *get_collider(int p_hit_index) const;
	int get_collider_shape(int p_hit_index) const;
	int get_collider_face_index(int p_hit_index) const;
};
