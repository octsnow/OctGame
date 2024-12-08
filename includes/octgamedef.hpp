#pragma once

#ifndef OCT_EXPORTS
    #ifdef EXPORT_OCTGAME
        #define OCT_EXPORTS __declspec(dllexport)
    #else
        #define OCT_EXPORTS
    #endif
#endif

