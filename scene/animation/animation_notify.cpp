/**************************************************************************/
/*  animation_notify.cpp                                                  */
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

#include "animation_notify.h"

#include "core/object/class_db.h"
#include "scene/resources/animation.h"

AnimationNotifyContext::AnimationNotifyContext(AnimationNode::ProcessState &p_process_state, AnimationNodeInstance &p_instance, double p_start_time, double p_end_time) {
    tree = p_process_state.tree;
    path = p_instance.path;
    animation = p_instance.cached_animation;
    previous_time = p_start_time;
    current_time = p_end_time;
}

void AnimationNotifyContextWrapper::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tree"), &AnimationNotifyContextWrapper::get_tree);
	ClassDB::bind_method(D_METHOD("get_path"), &AnimationNotifyContextWrapper::get_path);
	ClassDB::bind_method(D_METHOD("get_animation"), &AnimationNotifyContextWrapper::get_animation);
	ClassDB::bind_method(D_METHOD("get_previous_time"), &AnimationNotifyContextWrapper::get_previous_time);
	ClassDB::bind_method(D_METHOD("get_current_time"), &AnimationNotifyContextWrapper::get_current_time);
}

AnimationNotifyBase::AnimationNotifyBase() {
    current_context = memnew(AnimationNotifyContextWrapper);
}

AnimationNotifyBase::~AnimationNotifyBase() {
    memdelete(current_context);
    current_context = nullptr;
}

void AnimationNotifyBase::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_weight_threshold"), &AnimationNotifyBase::get_weight_threshold);
	ClassDB::bind_method(D_METHOD("set_weight_threshold", "weight_threshold"), &AnimationNotifyEvent::set_weight_threshold);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "weight_threshold", PROPERTY_HINT_RANGE, "0,1,0.0001"), "set_weight_threshold", "get_weight_threshold");
}

void AnimationNotifyBase::_set_current_context(const AnimationNotifyContext &p_context) const {
    CRASH_COND(!current_context);
    current_context->context = p_context;
}

double AnimationNotifyEvent::get_event_time(const Ref<Animation> &p_animation) const {
    return p_animation->get_marker_time(marker);
}

void AnimationNotifyEvent::notify(const AnimationNotifyContext &p_context, double p_delta) const {
    _set_current_context(p_context);
    GDVIRTUAL_CALL(_notify, _get_current_context(), p_delta);
}

AnimationNotifyEvent::AnimationNotifyEvent() {
    type = TYPE_EVENT;
}

void AnimationNotifyEvent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_marker"), &AnimationNotifyEvent::get_marker);
	ClassDB::bind_method(D_METHOD("set_marker", "marker"), &AnimationNotifyEvent::set_marker);

	ClassDB::bind_method(D_METHOD("get_event_time", "animation"), &AnimationNotifyEvent::get_event_time);

    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "marker"), "set_marker", "get_marker");

    GDVIRTUAL_BIND(_notify, "context", "delta");
}

double AnimationNotifyState::get_state_begin_time(const Ref<Animation> &p_animation) const {
    return p_animation->get_marker_time(marker_begin);
}

double AnimationNotifyState::get_state_end_time(const Ref<Animation> &p_animation) const {
    return p_animation->get_marker_time(marker_end);
}

void AnimationNotifyState::notify_begin(const AnimationNotifyContext &p_context, int64_t p_unique_id, double p_delta) const {
    _set_current_context(p_context);
    GDVIRTUAL_CALL(_notify_begin, _get_current_context(), p_unique_id, p_delta);
}

void AnimationNotifyState::notify_process(const AnimationNotifyContext &p_context, int64_t p_unique_id, double p_delta) const {
    _set_current_context(p_context);
    GDVIRTUAL_CALL(_notify_process, _get_current_context(), p_unique_id, p_delta);
}

void AnimationNotifyState::notify_end(const AnimationNotifyContext &p_context, int64_t p_unique_id, double p_delta) const {
    _set_current_context(p_context);
    GDVIRTUAL_CALL(_notify_end, _get_current_context(), p_unique_id, p_delta);
}

void AnimationNotifyState::notify_cancel(AnimationTree *tree, int64_t p_unique_id, double p_delta) const {
    GDVIRTUAL_CALL(_notify_cancel, tree, p_unique_id, p_delta);
}

AnimationNotifyState::AnimationNotifyState() {
    type = TYPE_STATE;
}

void AnimationNotifyState::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_marker_begin"), &AnimationNotifyState::get_marker_begin);
	ClassDB::bind_method(D_METHOD("set_marker_begin", "marker_begin"), &AnimationNotifyState::set_marker_begin);
	ClassDB::bind_method(D_METHOD("get_marker_end"), &AnimationNotifyState::get_marker_end);
	ClassDB::bind_method(D_METHOD("set_marker_end", "marker_end"), &AnimationNotifyState::set_marker_end);

	ClassDB::bind_method(D_METHOD("get_state_begin_time", "animation"), &AnimationNotifyState::get_state_begin_time);
	ClassDB::bind_method(D_METHOD("get_state_end_time", "animation"), &AnimationNotifyState::get_state_end_time);

    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "marker_begin"), "set_marker_begin", "get_marker_begin");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "marker_end"), "set_marker_end", "get_marker_end");

    GDVIRTUAL_BIND(_notify_begin, "context", "unique_id", "delta");
    GDVIRTUAL_BIND(_notify_process, "context", "unique_id", "delta");
    GDVIRTUAL_BIND(_notify_end, "context", "unique_id", "delta");
    GDVIRTUAL_BIND(_notify_cancel, "tree", "unique_id", "delta");
}

void AnimationNotifyQueue::push_event(AnimationNotifyEvent *p_event, const AnimationNotifyContext &p_context) {
    events_queue.push_back({ p_event, p_context });
}

void AnimationNotifyQueue::keep_alive_state(AnimationNotifyState *p_state, const AnimationNotifyContext &p_context) {
    states_queue.push_back({ p_state, p_context, false });
}

void AnimationNotifyQueue::end_state(AnimationNotifyState *p_state, const AnimationNotifyContext &p_context) {
    states_queue.push_back({ p_state, p_context, true });
}

void AnimationNotifyQueue::flush(double p_delta) {
    for (auto &it : events_queue) {
        if (it.event.is_valid()) {
            it.event->notify(it.context, p_delta);
        }
    }
    events_queue.clear();

    for (auto &it : states_queue) {
        if (it.state.is_valid()) {
            int32_t state_index = -1;
            for (uint32_t i = 0; i < known_states.size(); ++i) {
                if (known_states[i].state == it.state) {
                    state_index = int32_t(i);
                    break;
                }
            }

            int64_t state_id;
            if (state_index == -1) {
                state_id = next_unique_state_id++;
                it.state->notify_begin(it.context, state_id, p_delta);

                if (!it.should_end) {
                    known_states.push_back({ it.state, state_id });
                }
            } else {
                state_id = known_states[state_index].id;
            }

            it.state->notify_process(it.context, state_id, p_delta);

            if (it.should_end) {
                it.state->notify_end(it.context, state_id, p_delta);

                if (state_index != -1) {
                    known_states.remove_at_unordered(state_index);
                }
            } else {
                alive_states.push_back({ it.state, state_id });
            }
        }
    }
    states_queue.clear();

    for (auto &it : known_states) {
        if (!alive_states.has(it)) {
            it.state->notify_cancel(tree, it.id, p_delta);
        }
    }

    known_states.clear();
    SWAP(known_states, alive_states);
}
