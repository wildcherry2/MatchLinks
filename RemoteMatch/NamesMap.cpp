#include "pch.h"
#include "NamesMap.h"

static const std::vector<std::pair<std::string, std::string>> names_map = {
    { "Mannfield", "EuroStadium_P" },
    { "Mannfield (Stormy)" , "EuroStadium_Rainy_P" },
    { "Beckwith Park" , "Park_P" },
    { "Beckwith Park (Midnight)" , "Park_Night_P" },
    { "DFH Stadium" , "Stadium_P"},
};

static const std::vector<std::string> friendly_map_names = {
    "Mannfield",
    "Mannfield (Stormy)",
    "Beckwith Park",
    "Beckwith Park (Midnight)",
    "DFH Stadium",
};

static const std::vector<std::string> internal_map_names = {
    "EuroStadium_P",
    "EuroStadium_Rainy_P",
    "Park_P",
    "Park_Night_P",
    "Stadium_P",
};

static const std::map<std::string, Region> region_map = {
    { "US-East" , Region::USE },
    { "US-West", Region::USW },
    { "Europe", Region::EU }
};

static const std::vector<std::string> region_friendly_names = {
    "US-East",
    "US-West",
    "Europe"
};