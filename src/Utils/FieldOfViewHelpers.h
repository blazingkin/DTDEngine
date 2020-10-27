#ifndef FIELD_OF_VIEW_HELPERS_H
#define FIELD_OF_VIEW_HELPERS_H 1

// https://en.wikipedia.org/wiki/Field_of_view_in_video_games
#define FOV_Y_FROM_X(FOV_X, aspect) (2.0f * atan(tan((FOV_X) / 2.0f) * (1.0f / (aspect))))


#endif
