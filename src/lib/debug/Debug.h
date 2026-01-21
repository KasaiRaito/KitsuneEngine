//
// Created by Eduardo Huerta on 21/01/26.
//

#ifndef KITSUNEENGINE_DEBUG_H
#define KITSUNEENGINE_DEBUG_H

#pragma once

struct Debug
{
public:
    static inline bool DrawColliders;

    static void SetDebug(bool debug);

    static bool GetDebug();

};

#endif //KITSUNEENGINE_DEBUG_H