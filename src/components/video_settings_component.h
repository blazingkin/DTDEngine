#ifndef VIDEO_SETTINGS_H
#define VIDEO_SETTINGS_H 1

#include "../_components.h"
#include <queue>
#include <vector>
#include <math.h>

// Singleton
typedef struct _video_settings : component_t {
    float fovX = (3.0 * M_PI / 5.0); // Note that this is in radians
} c_video_settings_t;
#define COMPONENT_VIDEO_SETTINGS_CONNECTIONS (std::type_index(typeid(c_video_settings_t)))

#endif