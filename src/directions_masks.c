#include "directions_masks.h"

Direction directions_masks[8] = {
    {-6, MASK_LEFT | MASK_UP   },
    {-5, MASK_UP               },
    {-4, MASK_RIGHT | MASK_UP  },
    {-1, MASK_LEFT             },
    { 1, MASK_RIGHT            },
    { 4, MASK_LEFT | MASK_DOWN },
    { 5, MASK_DOWN             },
    { 6, MASK_RIGHT | MASK_DOWN}
};