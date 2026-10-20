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

#include "physics_cast_ray_multi_result_3d.h"

#include "core/object/class_db.h"

PhysicsCastRayMultiResult3D::PhysicsCastRayMultiResult3D(int p_max_hits) {
	ERR_FAIL_COND(p_max_hits < 0);
	result.resize(p_max_hits);
}

int PhysicsCastRayMultiResult3D::get_max_hits() const {
	return result.size();
}

void PhysicsCastRayMultiResult3D::set_max_hits(int p_max_hits) {
	ERR_FAIL_COND(p_max_hits < 0);
	result.resize(p_max_hits);
	hit_count = MIN(hit_count, static_cast<int>(result.size()));
}

int PhysicsCastRayMultiResult3D::get_hit_count() const {
	return hit_count;
}

Vector3 PhysicsCastRayMultiResult3D::get_position(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].position;
}

Vector3 PhysicsCastRayMultiResult3D::get_normal(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, Vector3());
    return result[p_hit_index].normal;
}

RID PhysicsCastRayMultiResult3D::get_collider_rid(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, RID());
    return result[p_hit_index].rid;
}

ObjectID PhysicsCastRayMultiResult3D::get_collider_id(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, ObjectID());
    return result[p_hit_index].collider_id;
}

Object *PhysicsCastRayMultiResult3D::get_collider(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, nullptr);
    return result[p_hit_index].get_collider();
}

int PhysicsCastRayMultiResult3D::get_collider_shape(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, 0);
    return result[p_hit_index].shape;
}

int PhysicsCastRayMultiResult3D::get_collider_face_index(int p_hit_index) const {
	ERR_FAIL_INDEX_V(p_hit_index, hit_count, 0);
    return result[p_hit_index].face_index;
}

void PhysicsCastRayMultiResult3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_max_hits"), &PhysicsCastRayMultiResult3D::get_max_hits);
	ClassDB::bind_method(D_METHOD("set_max_hits", "max_hits"), &PhysicsCastRayMultiResult3D::set_max_hits);
	ClassDB::bind_method(D_METHOD("get_hit_count"), &PhysicsCastRayMultiResult3D::get_hit_count);

	ClassDB::bind_method(D_METHOD("get_position", "hit_index"), &PhysicsCastRayMultiResult3D::get_position);
	ClassDB::bind_method(D_METHOD("get_normal", "hit_index"), &PhysicsCastRayMultiResult3D::get_normal);
	ClassDB::bind_method(D_METHOD("get_collider_rid", "hit_index"), &PhysicsCastRayMultiResult3D::get_collider_rid);
	ClassDB::bind_method(D_METHOD("get_collider_id", "hit_index"), &PhysicsCastRayMultiResult3D::get_collider_id);
	ClassDB::bind_method(D_METHOD("get_collider", "hit_index"), &PhysicsCastRayMultiResult3D::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_shape", "hit_index"), &PhysicsCastRayMultiResult3D::get_collider_shape);
	ClassDB::bind_method(D_METHOD("get_collider_face_index", "hit_index"), &PhysicsCastRayMultiResult3D::get_collider_face_index);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_hits"), "set_max_hits", "get_max_hits");
}
