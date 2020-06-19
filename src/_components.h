#ifndef _COMPONENTS_H
#define _COMPONENTS_H
#include <typeindex>
#include <typeinfo>
#define flase false
typedef std::type_index component_id_t;

typedef struct _component {
 virtual ~_component() = default;
} component_t;

#endif