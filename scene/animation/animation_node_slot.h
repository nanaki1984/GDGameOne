/**************************************************************************/
/*  animation_node_state_machine.h                                        */
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

#include "scene/animation/animation_blend_tree.h"

class AnimationNodeSlotPlayback;

class AnimationNodeSlot : public AnimationNodeSync {
	GDCLASS(AnimationNodeSlot, AnimationNodeSync);

public:
	virtual void validate_node(const AnimationTree *p_tree, const StringName &p_path) const override;

	virtual void get_parameter_list(LocalVector<PropertyInfo> *r_list) const override;
	virtual Variant get_parameter_default_value(const StringName &p_parameter) const override;
	virtual bool is_parameter_read_only(const StringName &p_parameter) const override;

    virtual NodeTimeInfo _process(ProcessState &p_process_state, AnimationNodeInstance &p_instance, const AnimationMixer::PlaybackInfo &p_playback_info, bool p_test_only = false) override;
	virtual String get_caption() const override;

    AnimationNodeSlot();

private:
	friend AnimationNodeSlotPlayback;

    StringName playback = "playback";
};

class AnimationNodeSlotPlayback : public Resource {
	GDCLASS(AnimationNodeSlotPlayback, Resource);

    friend AnimationNodeSlot;

    AnimationNode::NodeTimeInfo _process(AnimationNode::ProcessState &p_process_state, AnimationNodeInstance &p_instance, AnimationNodeSlot* p_slot, const AnimationMixer::PlaybackInfo &p_playback_info, bool p_test_only = false);

    StringName current_state;

    float fading_time;
    Ref<Curve> fading_curve;
    float fading_pos;
    bool reset{ true };

    Ref<AnimationSnapshot> last_frame_snapshot;

    float input_xfade_time{ .25f };
    Ref<Curve> input_xfade_curve;

    struct FadingOutState {
        Ref<AnimationSnapshot> snapshot;
        float fading_time;
        Ref<Curve> fading_curve;
        float fading_pos;
        bool should_delete;

        FadingOutState() { }
        FadingOutState(const Ref<AnimationSnapshot>& p_snapshot, float p_fading_time, const Ref<Curve> &p_fading_curve) {
            snapshot = p_snapshot->duplicate();
            fading_time = p_fading_time;
            fading_curve = p_fading_curve;
            fading_pos = 0;
            should_delete = false;
        }

        FadingOutState(const FadingOutState &) = delete;
        FadingOutState(FadingOutState &&p_other) :
            snapshot(std::move(p_other.snapshot)),
            fading_time(p_other.fading_time),
            fading_curve(std::move(p_other.fading_curve)),
            fading_pos(p_other.fading_pos),
            should_delete(p_other.should_delete) {
        }

        FadingOutState& operator=(const FadingOutState &) = delete;
        FadingOutState& operator=(FadingOutState &&p_other) {
            snapshot = std::move(p_other.snapshot);
            fading_time = p_other.fading_time;
            fading_curve = std::move(p_other.fading_curve);
            fading_pos = p_other.fading_pos;
            should_delete = p_other.should_delete;
            return (*this);
        }
    };
    LocalVector<FadingOutState> fading_out_states;

    struct Request {
        StringName store_name;
        float xfade_time;
        Ref<Curve> xfade_curve;
        bool is_valid{ false };
    } last_request;

protected:
	static void _bind_methods();

public:
    void set_input_xfade_time(float p_secs);
	float get_input_xfade_time() const;

    void set_input_xfade_curve(const Ref<Curve> &p_curve);
	Ref<Curve> get_input_xfade_curve() const;

    void play(const StringName &p_store_name, float p_xfade_time, const Ref<Curve> &p_xfade_curve);

    AnimationNodeSlotPlayback();
};
