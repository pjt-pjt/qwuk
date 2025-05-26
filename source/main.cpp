#include <memory>
#include <string>
#include <vector>
#include "SDL2/SDL.h"
#include "application.h"
#include "version.h"


#if defined(DEBUG)
    bool    debugMode = true;
#else
    bool    debugMode = false;
#endif


int main (int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string              startMap;

    bool help = false;
    auto arg = args.begin ();
    while (arg != args.end ()) {
        if (arg->empty ()) { // helps debugging
            ++arg;
            continue;
        }
        if (*arg == "-m" || *arg == "--map") {
            if (++arg != args.end () && !arg->empty()) {
                startMap = *arg;
            }
        } else if (*arg == "-d" || *arg == "--debug") {
            debugMode = true;
        } else if (*arg == "-h" || *arg == "--help") {
            help = true;
            break;
        } else {
            help = true;
            printf ("Unknown command line option '%s'\n", arg->c_str ());
            break;
        }
        if (arg == args.end()) {
            help = true;
            printf ("Bad command line options\n");
            break;
        }
        ++arg;
    }

    if (debugMode || help) {
        printf ("%s%s\n", APP_NAME " by PJT v", APP_VERSION);
    }

    if (help) {
        printf ("Usage: " APP_NAME " [optons]\n"\
                "       -m|--map <map path>\n"\
                "       -d|--debug\n"\
                "       -h|--help\n");
        return 0;
    }

    Application app;
    if (!app.Init ()) {
        return 1;
    }
    bool ok = app.Run (startMap.c_str());
    app.Destroy();

    return ok ? 0 : 1;
}
