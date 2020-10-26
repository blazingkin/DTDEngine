#ifndef POSTPROCESSING_COMPONENT_H
#define POSTPROCESSING_COMPONENT_H 1
#include "../_components.h"

typedef struct _postprocessing : component_t {

} c_postprocessing_step_t;
#define COMPONENT_POSTPROCESSING_STEP (std::type_index(typeid(c_postprocessing_step_t)))

#endif