#ifndef NE7_H
#define NE7_H    1

#define CLEAR_SCREEN    "\033[H\033[2J\033[3J"
#define CLEAR_TOP       "\033[H"
#define HIDE_CURSOR     "\033[?25h"
#define SHOW_CURSOR     "\033[?25l"

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795
#endif
#ifndef TO_SEC
    #define TO_SEC(x)       ((unsigned int)(x * 1000000))
#endif
#ifndef TO_MS
    #define TO_MS(x)        ((unsigned int)(x * 1000))
#endif

#ifndef TRUE
    #define TRUE    1
#endif
#ifndef FALSE
    #define FALSE   0
#endif

struct Color {
    double freq;
    double seed;
    double spread;
};

struct Config {
    int anim;
    int noise;
    unsigned int wait;
};

struct OptFlags {
    int file_opt;
    int text_opt;
    int ms_opt;
    int sec_opt;
    int ani_opt;
    int freq_opt;
    int seed_opt;
    int spr_opt;
    int noi_opt;
    int hidec_opt;
    int showc_opt;
};

#endif
