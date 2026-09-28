// English translation of the game's text. Strings are looked up by their
// original Finnish, like gettext message ids, so the game code draws text
// exactly as before and the translation happens in the font routines.

#include "pumLanguage.h"

#include <SDL3/SDL.h>

static const struct
{
    const char* fi;
    const char* en;
} s_English[] =
{
    // Main menu, padded to line up when centred
    {" f1 - aloita   ", " f1 - play     "},
    {" f2 - asetukset", " f2 - options  "},
    {" f3 - kartta   ", " f3 - map      "},
    {"esc - pois     ", "esc - quit     "},

    // Main menu info line
    {"kopyrikht (c) 10tons entertainment 2001", "kopyrite (c) 10tons entertainment 2001"},
    {"intter netsku: www.10tons.org", "interweb: www.10tons.org"},
    {"koodin ja gfx:n tuotti milzer", "code and gfx by milzer"},
    {"musiqja lahjoitti crud", "muziq donated by crud"},
    {"turbiksen kartat myonsi mikko virta meille", "mikko virta granted us his turboraketti maps"},
    {"ma tykkaan maileista ja oluesta, milzer(a)10tons.org", "i like emails and beer, milzer(a)10tons.org"},
    {"terkquja kaikille upeille, en jaksa listata :) (dokuissa on)", "greetz to all you awesome folks, too lazy to list :) (see docs)"},

    // Options
    {" - peliin liittyva matsku -", " - game stuff -"},
    {"painovoima", "gravity"},
    {"verikerroin", "blood factor"},
    {"partiklekerroin", "particle factor"},
    {"kentan korjaus", "map repair"},
    {"veren siivous", "blood cleanup"},
    {"partikleja", "particles"},
    {"ammuksia", "bullets"},
    {"pelaajia", "players"},
    {" - audio eli aeaeni -", " - audio aka sound -"},
    {"musiikin voimakqus", "muziq volume"},
    {"ulosvehje", "output gizmo"},
    {"ihan hiljasta", "dead silent"},
    {"taajuus", "frequency"},
    {"kanavia", "channels"},
    {" - visuaalista matskua -", " - visual stuff -"},
    {"hienot efut", "fancy effects"},
    {"juu", "yep"},
    {"ei", "nope"},
    {"normaalit asetukset", "default settings"},
    {"muuta nappulat", "change keys"},
    {"tallenna ja poistu", "save and exit"},
    {"tunge rektumiin", "stuff it"},

    // Key setup
    {"painahan toki haluamas nappula?", "go on, press the key you want"},
    {"pelaaja 1 vasen", "player 1 left"},
    {"pelaaja 1 oikea", "player 1 right"},
    {"pelaaja 1 ylos", "player 1 aim up"},
    {"pelaaja 1 alas", "player 1 aim down"},
    {"pelaaja 1 aseen vaihto", "player 1 change weapon"},
    {"pelaaja 1 hyppy", "player 1 jump"},
    {"pelaaja 1 ampuminen", "player 1 shoot"},
    {"pelaaja 2 vasen", "player 2 left"},
    {"pelaaja 2 oikea", "player 2 right"},
    {"pelaaja 2 ylos", "player 2 aim up"},
    {"pelaaja 2 alas", "player 2 aim down"},
    {"pelaaja 2 aseen vaihto", "player 2 change weapon"},
    {"pelaaja 2 hyppy", "player 2 jump"},
    {"pelaaja 2 ampuminen", "player 2 shoot"},
    {"pelaaja 3 vasen", "player 3 left"},
    {"pelaaja 3 oikea", "player 3 right"},
    {"pelaaja 3 ylos", "player 3 aim up"},
    {"pelaaja 3 alas", "player 3 aim down"},
    {"pelaaja 3 aseen vaihto", "player 3 change weapon"},
    {"pelaaja 3 hyppy", "player 3 jump"},
    {"pelaaja 3 ampuminen", "player 3 shoot"},

    // Exit prompt
    {" f9 - jatka ", " f9 - resume"},
    {"f10 - lopeta", "f10 - quit  "},

    // Weapon names from aseet.puo, including the ones only in the 1.0 release
    {"Minitykki", "Minigun"},
    {"Kranu", "Grenade"},
    {"Ohjus", "Missile"},
    {"Moerssaein", "Mortar"},
    {"Tykisto", "Artillery"},
    {"Kokkareet", "Clumps"},
    {"Miina", "Mine"},
    {"Polttopullo", "Molotov"},
};

// English unless PUM_LANG or the system's preferred language says Finnish
static bool UseEnglish()
{
    const char* lang = SDL_getenv("PUM_LANG");
    if (lang)
    {
        return SDL_strncasecmp(lang, "fi", 2) != 0;
    }

    int count = 0;
    SDL_Locale** locales = SDL_GetPreferredLocales(&count);
    bool english = !(locales && count > 0 && SDL_strcmp(locales[0]->language, "fi") == 0);
    SDL_free(locales);

    return english;
}

const char* Translate(const char* s)
{
    static const bool english = UseEnglish();

    if (english)
    {
        for (const auto& t : s_English)
        {
            if (SDL_strcmp(s, t.fi) == 0)
            {
                return t.en;
            }
        }
    }
    return s;
}
