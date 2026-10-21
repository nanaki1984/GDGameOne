/**************************************************************************/
/*  physics_intersect_shape_result_3d.cpp                                 */
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

#include "physics_cast_result_3d.h"

#include "core/object/class_db.h"

PhysicsCastResult3D::PhysicsCastResult3D(int p_max_hits) {
	ERR_FAIL_COND(p_max_hits < 0);
	result.resize(p_max_hits);
}

int PhysicsCastResult3D::get_max_hits() const {
	return result.size();
}

void PhysicsCastResult3D::set_max_hits(int p_max_hits) {
	ERR_FAIL_COND(p_max_hits < 0);
	result.resize(p_max_hits);
	hit_count = MIN(hit_count, static_cast<int>(result.size()));
}

int PhysicsCastResult3D::get_hit_count() const {
	return hit_count;
}

Vector3 PhysicsCastResult3D::get_position(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].position;
}

Vector3 PhysicsCastResult3D::get_normal(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].normal;
}

Vector3 PhysicsCastResult3D::get_contact_point(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].contact_point;
}

Vector3 PhysicsCastResult3D::get_contact_normal(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].contact_normal;
}

bool PhysicsCastResult3D::from_inside(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, false);
    return result[p_hit_index].from_inside();
}

float PhysicsCastResult3D::get_penetration_depth(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, .0f);
    return result[p_hit_index].penetration_depth;
}

Vector3 PhysicsCastResult3D::get_other_contact_point(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].get_other_contact_point();
}

RID PhysicsCastResult3D::get_collider_rid(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, RID());
    return result[p_hit_index].rid;
}

ObjectID PhysicsCastResult3D::get_collider_id(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, ObjectID());
    return result[p_hit_index].collider_id;
}

Object *PhysicsCastResult3D::get_collider(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, nullptr);
    return result[p_hit_index].get_collider();
}

int PhysicsCastResult3D::get_collider_shape(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, 0);
    return result[p_hit_index].shape;
}

int PhysicsCastResult3D::get_collider_face_index(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, 0);
    return result[p_hit_index].face_index;
}

void PhysicsCastResult3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_max_hits"), &PhysicsCastResult3D::get_max_hits);
	ClassDB::bind_method(D_METHOD("set_max_hits", "max_hits"), &PhysicsCastResult3D::set_max_hits);
	ClassDB::bind_method(D_METHOD("get_hit_count"), &PhysicsCastResult3D::get_hit_count);

	ClassDB::bind_method(D_METHOD("get_position", "hit_index"), &PhysicsCastResult3D::get_position, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_normal", "hit_index"), &PhysicsCastResult3D::get_normal, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_contact_point", "hit_index"), &PhysicsCastResult3D::get_contact_point, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_contact_normal", "hit_index"), &PhysicsCastResult3D::get_contact_normal, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("from_inside", "hit_index"), &PhysicsCastResult3D::from_inside, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_penetration_depth", "hit_index"), &PhysicsCastResult3D::get_penetration_depth, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_other_contact_point", "hit_index"), &PhysicsCastResult3D::get_other_contact_point, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_collider_rid", "hit_index"), &PhysicsCastResult3D::get_collider_rid, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_collider_id", "hit_index"), &PhysicsCastResult3D::get_collider_id, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_collider", "hit_index"), &PhysicsCastResult3D::get_collider, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_collider_shape", "hit_index"), &PhysicsCastResult3D::get_collider_shape, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_collider_face_index", "hit_index"), &PhysicsCastResult3D::get_collider_face_index, DEFVAL(0));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_hits"), "set_max_hits", "get_max_hits");
}
