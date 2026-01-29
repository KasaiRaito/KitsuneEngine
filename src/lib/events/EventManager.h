//
// Created by Eduardo Huerta on 29/01/26.
//

#ifndef KITSUNEENGINE_EVENTMANAGER_H
#define KITSUNEENGINE_EVENTMANAGER_H

#pragma once
#include "List.h"
#include "Subscription.h"

// A very small “delegate”: calls fn(ctx, payload)
template <typename Payload>
struct EventListener
{
    using Fn = void(*)(void* ctx, const Payload& payload);

    void* ctx = nullptr;
    Fn fn = nullptr;

    bool Equals(void* c, Fn f) const { return ctx == c && fn == f; }
    void Invoke(const Payload& p) const { if (fn) fn(ctx, p); }
};

template <typename Payload>
class EventChannel
{
public:
    using Listener = EventListener<Payload>;
    using Fn = typename Listener::Fn;

    Subscription SubscribeScoped(void* ctx, Fn fn)
    {
        Subscribe(ctx, fn);

        Subscription sub;
        sub.channel = this;
        sub.ctx = ctx;
        sub.fn = (void*)fn;
        sub.unsubscribe = [](void* ch, void* c, void* f)
        {
            auto* channel = (EventChannel<Payload>*)ch;
            channel->Unsubscribe(c, (Fn)f);
        };
        return sub;
    }

    void Subscribe(void* ctx, Fn fn)
    {
        // prevent duplicates
        for (size_t i = 0; i < listeners.Size(); i++)
        {
            if (listeners.Get(i).Equals(ctx, fn)) return;
        }
        Listener l; l.ctx = ctx; l.fn = fn;
        listeners.Add(l);
    }

    void Unsubscribe(void* ctx, Fn fn)
    {
        for (size_t i = 0; i < listeners.Size(); i++)
        {
            if (listeners.Get(i).Equals(ctx, fn))
            {
                // remove by swap-back (fast)
                size_t last = listeners.Size() - 1;
                if (i != last) listeners[i] = listeners.Get((int)last);
                listeners.PopBack();
                return;
            }
        }
    }

    void Emit(const Payload& payload)
    {
        // Call a snapshot in case listeners modify subscriptions during callbacks
        // (Simple approach: iterate original, but be careful with removals)
        for (size_t i = 0; i < listeners.Size(); i++)
            listeners.Get(i).Invoke(payload);
    }

    void Clear() { listeners.Clear(); }

private:
    List<Listener> listeners;
};

#endif //KITSUNEENGINE_EVENTMANAGER_H