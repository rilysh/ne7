#define _XOPEN_SOURCE   500

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

#include "ne7.h"

/**
 * @brief Private function to create a random number based on random seed (PRNG)
 * @returns unsigned long int
*/
static unsigned long int xorshift64(unsigned long int seed)
{
	unsigned long int x;

	x = seed;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	seed = x;

	return seed;
}

/**
 * @brief Private function to convert a string integer to integer representation
 * @returns int
*/
static int to_int(const char *str)
{
    long val;
    char *eptr;

    val = strtol(str, &eptr, 10);

    if (eptr == str) {
        fprintf(stdout, "Error: No numbers were found\n");
        exit(EXIT_FAILURE);
    }

    return (int)val;
}

/**
 * @brief Private function to convert a string decimal to decimal representation
 * @returns double
*/
static double to_double(const char *str)
{
    char *eptr;
    double val;

    val = strtod(str, &eptr);

    if (eptr == str) {
        fprintf(stdout, "Error: No numbers were found\n");
        exit(EXIT_FAILURE);
    }

    return val;
}

/**
 * @brief Private function to generate random seed (for UNIX-based systems only
 * @returns unsigned char *
*/
static unsigned char *gen_seed(void)
{
    FILE *fp;
    static unsigned char buf[10];

    fp = fopen("/dev/urandom", "rb");
    if (fp == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    fread(buf, 1UL, sizeof(buf), fp);
    fclose(fp);

    return buf;
}

/**
 * @brief Private function to format error messages and exit
*/
static void fperr(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    exit(EXIT_FAILURE);
}

/**
 * @brief Function show_color uses terminal's RGB format as a sine wave to represent color
*/
static void show_color(struct Color color, struct Config cfg, const char *str)
{
    int idx;
    const char *p;
    double i, r, g, b;
    unsigned char *seed;

    if (cfg.noise < 0 || cfg.noise > 256)
        cfg.noise = 5;
    if (cfg.anim == 0)
        cfg.anim = 2;

    idx = 0;
    seed = gen_seed();

    if (cfg.anim == 1)
        fprintf(stdout, "%s", CLEAR_SCREEN);

    while (cfg.anim) {
        if (cfg.anim == 2)
            cfg.anim = 0;
        else
            fprintf(stdout, CLEAR_TOP);

        p = str;
        do {
            i = (double)(color.freq * color.seed / color.spread);
            r = sin(i) * 127.0 + 128.0;
            g = sin(i + (M_PI * 2.0 / 3.0)) * 127.0 + 128.0;
            b = sin(i + (M_PI * 4.0 / 3.0)) * 127.0 + 128.0;
            color.seed += (double)(xorshift64(seed[idx == 10 ? idx = 0 : idx++]) % (unsigned long int)cfg.noise);

            fprintf(stdout,
                "\033[38;2;%d;%d;%dm%c", (int)r, (int)g, (int)b, *p
            );
        } while (*p++ != '\0');

        if (cfg.anim == 1)
            usleep(cfg.wait);

        /* End color stream */
        fputs("\033[0m", stdout);
    }
}

static void usage(void)
{
    fprintf(stdout,
        "ne7 - a color tool for UNIX-based systems\n\n"
        "Usage\n"
        "-----\n"
        "   --file [file_name]   - Select the file and print output with mixed RGB colors\n"
        "   --text [message]     - Text message to colorize\n"
        "   --ms [milliseconds]  - Wait for provided milliseconds on animation before recolorize (default: 1000ms)\n"
        "   --sec [seconds]      - Wait for provided seconds on animation before recolorize (default: 1s)\n"
        "   --amin               - Enable/disable animation of text buffer\n"
        "   --freq [frequency]   - Set RGB frequency (default: 0.3)\n"
        "   --seed [limit]       - Set a seed limit (default: 0..255, random)\n"
        "   --spread [limit]     - Set a spread limit (default: 8.0)\n"
        "   --noise [limit]      - Set a noise limit (default: 7)\n"
        "   --showcur            - Make the terminal cursor visible\n"
        "   --hidecur            - Make the terminal cursor hidden\n\n"
    );
}

int main(int argc, char **argv)
{
    if (argc < 2 || argv[1][0] != '-') {
        usage();
        exit(EXIT_FAILURE);
    }

    /* local variables */
    long sz;
    FILE *fp;
    int opt, pi_0, pi_1;
    char *buf, *passed_arg;
    double pd_0, pd_1, pd_2;

    /* struct definition */
    struct Config cfg;
    struct Color color;
    struct OptFlags flag = {
        .file_opt  = FALSE, .text_opt  = FALSE, .ms_opt   = FALSE,
        .sec_opt   = FALSE, .ani_opt   = FALSE, .freq_opt = FALSE,
        .seed_opt  = FALSE, .spr_opt   = FALSE, .noi_opt  = FALSE,
        .hidec_opt = FALSE, .showc_opt = FALSE
    };
    struct option options[] = {
        { "file",     required_argument, NULL, 1 },
        { "text",     required_argument, NULL, 2 },
        { "ms",       required_argument, NULL, 3 },
        { "sec",      required_argument, NULL, 4 },
        { "anim",     no_argument,       NULL, 5 },
        { "freq",     required_argument, NULL, 6 },
        { "seed",     required_argument, NULL, 7 },
        { "spread",   required_argument, NULL, 8 },
        { "noise",    required_argument, NULL, 9 },
        { "showcur",  no_argument,       NULL, 10 },
        { "hidecur",  no_argument,       NULL, 11 },
        { "help",     no_argument,       NULL, 12 }
    };

    /* initalize local variables */
    pi_0 = pi_1 = 0;
    buf = passed_arg = NULL;
    pd_0 = pd_1 = pd_2 = 0.0;

    /* set default config variables */
    cfg.anim = 0;
    cfg.noise = 7;
    cfg.wait = TO_MS(1000);

    /* set default color variables */
    color.freq = 0.3;
    color.spread = 8.0;
    color.seed = (double)(xorshift64(gen_seed()[0]) % 255);

    while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
        switch (opt) {
        /* file argument */
        case 1:
            flag.file_opt = TRUE;
            passed_arg = optarg;
            break;

        /* text argument */
        case 2:
            flag.text_opt = TRUE;
            buf = optarg;
            break;

        /* ms argument */
        case 3:
            flag.ms_opt = TRUE;
            pi_0 = to_int(optarg);
            break;

        /* sec argument */
        case 4:
            flag.sec_opt = TRUE;
            pi_0 = to_int(optarg);
            break;

        /* anim argument */
        case 5:
            flag.ani_opt = TRUE;
            break;

        /* freq argument */
        case 6:
            flag.freq_opt = TRUE;
            pd_0 = to_double(optarg);
            break;

        /* seed argument */
        case 7:
            flag.seed_opt = TRUE;
            pd_1 = to_double(optarg);
            break;

        /* spread argument */
        case 8:
            flag.spr_opt = TRUE;
            pd_2 = to_double(optarg);
            break;

        /* noise argument */
        case 9:
            flag.noi_opt = TRUE;
            pi_1 = to_int(optarg);
            break;

        /* showcur argument */
        case 10:
            flag.hidec_opt = TRUE;
            break;

        /* hidecur atgument */
        case 11:
            flag.showc_opt = TRUE;
            break;

        /* help argument */
        case 12:
            /* we don't expect --help argument others */
            usage();
            exit(EXIT_SUCCESS);

        /* unknown argument */
        case '?':
        default:
            exit(EXIT_FAILURE);
        }
    }

    if (flag.text_opt && flag.file_opt)
        fperr("Error: Passing both param text and file aren't allowed.\n");

    if (flag.file_opt) {
        fp = fopen(passed_arg, "rb");
        if (fp == NULL) {
            perror("fopen()");
            exit(EXIT_FAILURE);
        }

        fseek(fp, 1L, SEEK_END);
        sz = ftell(fp);
        rewind(fp);

        buf = calloc((unsigned long int)(sz + 1), 1UL);
        if (buf == NULL) {
            perror("calloc()");
            exit(EXIT_FAILURE);
        }

        fread(buf, 1UL, (unsigned long int)(sz + 1), fp);
        fclose(fp);
    }

    if (flag.ms_opt)
        cfg.wait = TO_MS(pi_0);

    if (flag.sec_opt) {
        if (flag.ms_opt)
            fperr("Error: Passing both sec and ms opt aren't allowed.\n");

        cfg.wait = TO_SEC(pi_0);
    }

    if (flag.ani_opt)
        cfg.anim = TRUE;

    if (flag.freq_opt)
        color.freq = pd_0;
    
    if (flag.seed_opt)
        color.seed = pd_1;

    if (flag.spr_opt)
        color.spread = pd_2;

    if (flag.noi_opt)
        cfg.noise = pi_1;

    if (flag.file_opt || flag.text_opt) {
        show_color(color, cfg, buf);

        if (flag.file_opt)
            free(buf);
    }

    if (flag.hidec_opt)
        fprintf(stdout, "%s", HIDE_CURSOR);

    if (flag.showc_opt)
        fprintf(stdout, "%s", SHOW_CURSOR);
}
