#ifndef DEV_DEBUG
#define DEV_DEBUG

#define DEBUG_MODE false



#if DEBUG_MODE
    #define dvprint(...)              \
        printf("[DEV-DEBUG]: ");      \
        printf(__VA_ARGS__)
#else
    #define dvprint(...)        
#endif 



#endif /*DEV_DEBUG*/