//
// Created by Eduardo Huerta on 21/01/26.
//

#include "Debug.h"

void Debug::SetDebug(bool debug) {
    DrawColliders = debug;
}

bool Debug::GetDebug() {
    return DrawColliders;
}
