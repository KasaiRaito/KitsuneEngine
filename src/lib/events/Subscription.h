//
// Created by Eduardo Huerta on 29/01/26.
//

#ifndef KITSUNEENGINE_SUBSCRIPTION_H
#define KITSUNEENGINE_SUBSCRIPTION_H

#pragma once

template <typename Payload>
class EventChannel;

struct Subscription
{
    void (*unsubscribe)(void* channel, void* ctx, void* fn) = nullptr;
    void* channel = nullptr;
    void* ctx = nullptr;
    void* fn = nullptr;

    void Unsubscribe()
    {
        if (unsubscribe && channel)
            unsubscribe(channel, ctx, fn);

        unsubscribe = nullptr;
        channel = nullptr;
        ctx = nullptr;
        fn = nullptr;
    }
};

#endif //KITSUNEENGINE_SUBSCRIPTION_H