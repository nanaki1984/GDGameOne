/**************************************************************************/
/*  animation_notify.h                                                    */
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

#include "scene/animation/animation_tree.h"

class Animation;
class AnimationTree;

class AnimationNotifyEvent;
class AnimationNotifyState;

struct AnimationNotifyContext {
    AnimationTree *tree{ nullptr };
    StringName path;
    Ref<Animation> animation;
    double previous_time;
    double current_time;

    AnimationNotifyContext() { }
    AnimationNotifyContext(AnimationNode::ProcessState &p_process_state, AnimationNodeInstance &p_instance, double p_start_time, double p_end_time);
};

class AnimationNotifyContextWrapper : public Object {
    GDCLASS(AnimationNotifyContextWrapper, Object)

    friend class AnimationNotifyBase;

    AnimationNotifyContext context;

public:
    inline AnimationTree *get_tree() const { return context.tree; }
    inline StringName get_path() const { return context.path; }
    inline Ref<Animation> get_animation() const { return context.animation; }
    inline double get_previous_time() const { return context.previous_time; }
    inline double get_current_time() const { return context.current_time; }

protected:
    static void _bind_methods();
};

class AnimationNotifyBase : public Resource {
    GDCLASS(AnimationNotifyBase, Resource)

public:
    enum Type {
        TYPE_EVENT,
        TYPE_STATE,

        TYPE_MAX
    };

private:
    mutable AnimationNotifyContextWrapper *current_context{ nullptr };

public:
    AnimationNotifyBase();
    ~AnimationNotifyBase();

    _FORCE_INLINE_ Type get_type() const { return type; }

protected:
    _FORCE_INLINE_ AnimationNotifyContextWrapper *_get_current_context() const { return current_context; }
    void _set_current_context(const AnimationNotifyContext &p_context) const;

    Type type{ TYPE_MAX };
};

class AnimationNotifyEvent : public AnimationNotifyBase {
    GDCLASS(AnimationNotifyEvent, AnimationNotifyBase)

    StringName marker;

public:
    inline StringName get_marker() const { return marker; }
    inline void set_marker(StringName p_marker) { marker = p_marker; }

    double get_event_time(const Ref<Animation> &p_animation) const;

    virtual void notify(const AnimationNotifyContext &p_context, double p_delta) const;

	GDVIRTUAL2C(_notify, AnimationNotifyContextWrapper*, double);

    AnimationNotifyEvent();

protected:
    static void _bind_methods();
};

class AnimationNotifyState : public AnimationNotifyBase {
    GDCLASS(AnimationNotifyState, AnimationNotifyBase)

    StringName marker_begin;
    StringName marker_end;

public:
    inline StringName get_marker_begin() const { return marker_begin; }
    inline void set_marker_begin(StringName p_marker_begin) { marker_begin = p_marker_begin; }
    inline StringName get_marker_end() const { return marker_end; }
    inline void set_marker_end(StringName p_marker_end) { marker_end = p_marker_end; }

    double get_state_begin_time(const Ref<Animation> &p_animation) const;
    double get_state_end_time(const Ref<Animation> &p_animation) const;

    virtual void notify_begin(const AnimationNotifyContext &p_context, double p_delta) const;
    virtual void notify_process(const AnimationNotifyContext &p_context, double p_delta) const;
    virtual void notify_end(const AnimationNotifyContext &p_context, double p_delta) const;
    virtual void notify_cancel(AnimationTree *tree, double p_delta) const;

    GDVIRTUAL2C(_notify_begin, AnimationNotifyContextWrapper*, double);
    GDVIRTUAL2C(_notify_process, AnimationNotifyContextWrapper*, double);
    GDVIRTUAL2C(_notify_end, AnimationNotifyContextWrapper*, double);
    GDVIRTUAL2C(_notify_cancel, AnimationTree*, double);

    AnimationNotifyState();

protected:
    static void _bind_methods();
};

class AnimationNotifyQueue : public Object {
    GDSOFTCLASS(AnimationNotifyQueue, Object)

    struct Event {
        Ref<AnimationNotifyEvent> event;
        AnimationNotifyContext context;
    };
    LocalVector<Event> events_queue;

    struct State {
        Ref<AnimationNotifyState> state;
        AnimationNotifyContext context;
        bool should_end;
    };
    LocalVector<State> states_queue;

    HashSet<Ref<AnimationNotifyState>> known_states;
    HashSet<Ref<AnimationNotifyState>> alive_states;

public:
    void push_event(AnimationNotifyEvent *p_event, const AnimationNotifyContext &p_context);
    void keep_alive_state(AnimationNotifyState *p_state, const AnimationNotifyContext &p_context);
    void end_state(AnimationNotifyState *p_state, const AnimationNotifyContext &p_context);

    void flush(double p_delta);

    AnimationTree *tree{ nullptr };
};
