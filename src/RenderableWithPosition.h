#ifndef RENDERABLE_WITH_POSITION_H
#define RENDERABLE_WITH_POSITION_H 1
#include "components.h"

class RenderableWithPosition {
    public:
        struct _renderable_obj *renderable;
        struct _model_obj *model;
        c_location_t relativeLocation;
};

#endif