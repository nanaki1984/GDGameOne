/**************************************************************************/
/*  animation_node_slot.cpp                                               */
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

#include "animation_node_slot.h"

#include "core/object/class_db.h"

void AnimationNodeSlot::validate_node(const AnimationTree *p_tree, const StringName &p_path) const {
	AnimationNodeSync::validate_node(p_tree, p_path);

	const String playback_path = String(p_path) + String(playback);
	Ref<AnimationNodeSlotPlayback> pb = p_tree->get(playback_path);
	if (pb.is_null()) {
		add_validation_error(p_tree, p_path, vformat(RTR("No playback resource set at path: %s."), playback_path));
	}
}

void AnimationNodeSlot::get_parameter_list(LocalVector<PropertyInfo> *r_list) const {
	AnimationNodeSync::get_parameter_list(r_list);
	r_list->push_back(PropertyInfo(Variant::OBJECT, playback, PROPERTY_HINT_RESOURCE_TYPE, AnimationNodeSlotPlayback::get_class_static(), PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_ALWAYS_DUPLICATE)); // Don't store this object in .tres, it always needs to be made as unique object.
}

Variant AnimationNodeSlot::get_parameter_default_value(const StringName &p_parameter) const {
	Variant ret = AnimationNodeSync::get_parameter_default_value(p_parameter);
	if (ret != Variant()) {
		return ret;
	}

	if (p_parameter == playback) {
		Ref<AnimationNodeSlotPlayback> p;
		p.instantiate();
		return p;
	}

    return Variant();
}

bool AnimationNodeSlot::is_parameter_read_only(const StringName &p_parameter) const {
	if (AnimationNodeSync::is_parameter_read_only(p_parameter)) {
		return true;
	}

	if (p_parameter == playback) {
		return true;
	}
	return false;
}

AnimationNode::NodeTimeInfo AnimationNodeSlot::_process(ProcessState &p_process_state, AnimationNodeInstance &p_instance, const AnimationMixer::PlaybackInfo &p_playback_info, bool p_test_only) {
	Ref<AnimationNodeSlotPlayback> playback_new = p_instance.get_parameter(playback);
	ERR_FAIL_COND_V(playback_new.is_null(), AnimationNode::NodeTimeInfo());
    return playback_new->_process(p_process_state, p_instance, this, p_playback_info, p_test_only);
}

String AnimationNodeSlot::get_caption() const {
    return "Slot";
}

AnimationNodeSlot::AnimationNodeSlot() {
    add_input("input");
}

void AnimationNodeSlotPlayback::_xfade(float p_xfade_time, const Ref<Curve> &p_xfade_curve, bool p_reset, const StringName &p_next_state, bool p_canceled) {
    fading_time = p_xfade_time;
    fading_curve = p_xfade_curve;
    fading_pos = .0f;
    reset = p_reset;

    fading_out_states.push_back(FadingOutState{ last_frame_snapshot, fading_time, fading_curve });

    auto previous_state = current_state;
    current_state = p_next_state;

    if (!previous_state.is_empty()) {
        emit_signal(SceneStringName(state_finished), this, previous_state, p_canceled);
    }

    if (!current_state.is_empty()) {
        emit_signal(SceneStringName(state_started), this, current_state);
    }
}

AnimationNode::NodeTimeInfo AnimationNodeSlotPlayback::_process(AnimationNode::ProcessState &p_process_state, AnimationNodeInstance &p_instance, AnimationNodeSlot* p_slot, const AnimationMixer::PlaybackInfo &p_playback_info, bool p_test_only) {
    if (!p_test_only) {
        if (last_request.is_valid) {
            auto xfade_curve = std::move(last_request.xfade_curve);
            last_request.is_valid = false;

            if (last_request.reset || current_state != last_request.store_name) {
                _xfade(last_request.xfade_time, xfade_curve, true, last_request.store_name, true);
            }
        } else if (stop_requested) {
            stop_requested = false;

            if (!current_state.is_empty()) {
                _xfade(input_xfade_time, input_xfade_curve, !p_slot->is_using_sync(), { }, true);
            }
        }
    }

    float p_delta = p_playback_info.delta;

    if (!p_test_only) {
        for (auto& fading_out_state : fading_out_states) {
            if (Animation::is_greater_or_equal_approx(fading_out_state.fading_pos, fading_out_state.fading_time)) {
                fading_out_state.should_delete = true;
                continue;
            }

            float blend = MIN(1.f, fading_out_state.fading_pos / fading_out_state.fading_time);
            if (fading_out_state.fading_curve.is_valid()) {
                blend = CLAMP(fading_out_state.fading_curve->sample(blend), .0f, 1.f);
            }
            blend = 1.f - blend;

            fading_out_state.fading_pos += p_delta;

            p_slot->blend_snapshot(p_process_state, p_instance, fading_out_state.snapshot.ptr(), blend);
        }

        for (int32_t i = fading_out_states.size() - 1; i >= 0; --i) {
            if (fading_out_states[i].should_delete) {
                fading_out_states.remove_at(i);
            }
        }
    }

    AnimationMixer::PlaybackInfo pi = p_playback_info;

    const bool store_reset = reset;
    if (reset) {
        pi.time = 0;
        pi.seeked = true;
        pi.is_external_seeking = false;

        if (!p_test_only) {
            reset = false;
        }
    }

    if (!p_test_only) {
        if (fading_time > 0) {
            fading_pos += p_delta;

            pi.weight = MIN(1.f, fading_pos / fading_time);
            if (fading_curve.is_valid()) {
                pi.weight = CLAMP(fading_curve->sample(pi.weight), .0f, 1.f);
            }

            if (Animation::is_greater_or_equal_approx(fading_pos, fading_time)) {
                fading_curve.unref();
                fading_time = 0;
            }
        } else {
            pi.weight = 1.0;
        }
    }

    bool back_to_input = false;
    AnimationNode::NodeAnimCache* cache{ nullptr };
    AnimationNode::NodeTimeInfo nti;

    if (current_state.is_empty()) {
        nti = p_slot->blend_input(p_process_state, p_instance, 0, pi, AnimationNode::FILTER_IGNORE, p_slot->is_using_sync(), p_test_only, &cache);
    } else {
        if (!p_test_only && p_slot->is_using_sync()) {
            AnimationMixer::PlaybackInfo sync_pi = p_playback_info;
            sync_pi.weight = 0;
            p_slot->blend_input(p_process_state, p_instance, 0, sync_pi);
        }

        nti = p_slot->blend_store(p_process_state, p_instance, current_state, store_reset, pi.weight, p_test_only, &cache);
        if (nti.get_remain() == 0) {
            back_to_input = true;
        }
    }

    if (!p_test_only) {
        if (cache) {
            last_frame_snapshot->set_anim_cache(*cache, pi.weight);
        } else {
            last_frame_snapshot->clear();
        }

        if (back_to_input) {
            _xfade(input_xfade_time, input_xfade_curve, !p_slot->is_using_sync(), { });
        }
    }

    return nti;
}

void AnimationNodeSlotPlayback::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_input_xfade_time", "secs"), &AnimationNodeSlotPlayback::set_input_xfade_time);
	ClassDB::bind_method(D_METHOD("get_input_xfade_time"), &AnimationNodeSlotPlayback::get_input_xfade_time);

	ClassDB::bind_method(D_METHOD("set_input_xfade_curve", "curve"), &AnimationNodeSlotPlayback::set_input_xfade_curve);
	ClassDB::bind_method(D_METHOD("get_input_xfade_curve"), &AnimationNodeSlotPlayback::get_input_xfade_curve);

    ClassDB::bind_method(D_METHOD("play", "store_name", "xfade_time", "xfade_curve", "reset"), &AnimationNodeSlotPlayback::play, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("stop"), &AnimationNodeSlotPlayback::stop);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "input_xfade_time", PROPERTY_HINT_RANGE, "0,240,0.01,suffix:s"), "set_input_xfade_time", "get_input_xfade_time");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "input_xfade_curve", PROPERTY_HINT_RESOURCE_TYPE, Curve::get_class_static()), "set_input_xfade_curve", "get_input_xfade_curve");

    ADD_SIGNAL(MethodInfo(SceneStringName(state_started), PropertyInfo(Variant::OBJECT, "playback"), PropertyInfo(Variant::STRING_NAME, "state")));
	ADD_SIGNAL(MethodInfo(SceneStringName(state_finished), PropertyInfo(Variant::OBJECT, "playback"), PropertyInfo(Variant::STRING_NAME, "state"), PropertyInfo(Variant::BOOL, "canceled")));    
}

void AnimationNodeSlotPlayback::set_input_xfade_time(float p_secs) {
	input_xfade_time = MAX(.0f, p_secs);
	emit_changed();
}

float AnimationNodeSlotPlayback::get_input_xfade_time() const {
    return input_xfade_time;
}

void AnimationNodeSlotPlayback::set_input_xfade_curve(const Ref<Curve> &p_curve) {
	input_xfade_curve = p_curve;
	emit_changed();
}

Ref<Curve> AnimationNodeSlotPlayback::get_input_xfade_curve() const {
    return input_xfade_curve;
}

void AnimationNodeSlotPlayback::play(const StringName &p_store_name, float p_xfade_time, const Ref<Curve> &p_xfade_curve, bool p_reset) {
    if (!p_store_name.is_empty()) {
        last_request.store_name = p_store_name;
        last_request.xfade_time = MAX(.0f, p_xfade_time);
        last_request.xfade_curve = p_xfade_curve;
        last_request.reset = p_reset;
        last_request.is_valid = true;

        stop_requested = false;
    }
}

void AnimationNodeSlotPlayback::stop() {
    if (last_request.is_valid) {
        last_request.xfade_curve.unref();
        last_request.is_valid = false;
    } else {
        stop_requested = true;
    }
}

AnimationNodeSlotPlayback::AnimationNodeSlotPlayback() {
    last_frame_snapshot.instantiate();
}
