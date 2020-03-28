#ifndef ACPI_LISTEN_CMD
#define ACPI_LISTEN_CMD {"/usr/bin/acpi_listen"}
#endif

static const multi_stringmap rules = {
    {{"ac_adapter", "ACPI0003:00", "00000080", "00000000"},
        {}
    },
    {{"ac_adapter", "ACPI0003:00", "00000080", "00000001"},
        {}
    },
    {{"battery", "PNP0C0A:00", "00000080", "00000000"},
        {}
    },
    {{"battery", "PNP0C0A:00", "00000080", "00000001"},
        {}
    },
    {{"button/lid", "LID", "close", "close"},
        {}
    },
    {{"button/lid", "LID", "open", "open"},
        {}
    },
    {{"processor", "LNXCPU:03", "00000081", "00000000"},
        {}
    },
    {{"video/tabletmode", "TBLT", "0000008A", "00000000"},
        {"thinkpad-rotate", "normal"}
    },
    {{"video/tabletmode", "TBLT", "0000008A", "00000001"},
        {"thinkpad-rotate", "inverted"}
    }
};

static const stringmap rules_fallback = {
    {"button/battery",
        {}
    },
    {"button/mute",
        {"amixer", "set", "Master", "toggle"}
    },
    {"button/sleep",
        {}
    },
    {"button/volumedown",
        {"amixer", "-D", "pulse", "set", "Master", "5%-"}
    },
    {"button/volumeup",
        {"amixer", "-D", "pulse", "set", "Master", "5%+"}
    },
    {"video/brightnessup",
        {"xbacklight", "-inc", "10"}
    },
    {"video/brightnessdown",
        {"xbacklight", "-dec", "10"}
    },
    {"video/switchmode",
        {}
    }
};

// vim: set et sw=4 sts=4 ft=cpp:
