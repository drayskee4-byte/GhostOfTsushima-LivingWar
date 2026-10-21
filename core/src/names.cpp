#include "lw/names.hpp"

#include <vector>

namespace lw {

namespace {
// Historical Mongol, Turkic and Jurchen given names, plus Goryeo and Song names for the
// conscripted contingents that sailed with the 1274 invasion.
const std::vector<std::string> kMongol = {
    "Temur", "Batu", "Jochi", "Subutai", "Jebe", "Bayan", "Arslan", "Borokhula", "Chagaan", "Dorji",
    "Ganbold", "Khasar", "Mukhali", "Naimachu", "Ochir", "Qutlugh", "Sorkhan", "Toghril", "Ulagchi", "Yesun",
    "Altan", "Baatar", "Chinua", "Erdene", "Khulan", "Mongke", "Nogai", "Orda", "Sartaq", "Tolui",
    "Boroldai", "Kitbuqa", "Hulegu", "Belgutei", "Jelme", "Shiremun", "Tekuder", "Arigh", "Qaidu", "Buqa",
    "Ghazan", "Tumur", "Bilge", "Kuchlug", "Esen", "Toqta", "Abaqa", "Uriang", "Kokochu", "Dayir",
};
const std::vector<std::string> kGoryeo = {"Kim Bang", "Hong Dagu", "Yi Sun", "Pak Chong", "Choe Yun", "Han Gyeong",
                                          "Jeong In", "Kang Hyeon", "Yu Seok", "Seo Mun", "Jo Hui", "Nam Geon"};
const std::vector<std::string> kSong = {"Liu Fuheng", "Zhang Wen", "Wang Shao", "Chen Ying", "Zhao Kang",
                                        "Li Ting", "Sun Hu", "Xu Dao", "Ma Rong", "Gao Shun"};

const std::vector<std::string> kOrigins = {
    "the Kerulen steppe", "Karakorum", "the Onon river", "the Altai foothills", "Khanbaliq",
    "the Gobi's edge", "the Orkhon valley", "the Jurchen marches", "Goryeo", "the Song frontier",
};

std::string appearanceFor(Archetype a, Rng& rng) {
    // Adapter-interpreted keys: "<class>_<armour tier>_<variant>"
    static const char* cls[] = {"sword", "shield", "spear", "brute", "archer", "shaman"};
    return std::string(cls[static_cast<int>(a)]) + "_officer_" + std::to_string(rng.range(1, 4));
}
} // namespace

Identity makeIdentity(Rng& rng, Archetype a, const std::set<std::string>& taken) {
    Identity id;
    int roll = rng.range(1, 100);
    const auto& pool = roll <= 82 ? kMongol : (roll <= 94 ? kGoryeo : kSong);
    id.origin = &pool == &kMongol ? rng.pick(kOrigins) : (&pool == &kGoryeo ? "Goryeo" : "the Song frontier");
    for (int tries = 0; tries < 30; ++tries) {
        std::string n = rng.pick(pool);
        if (!taken.count(n)) { id.name = n; break; }
    }
    if (id.name.empty()) {
        // Pool exhausted: use a patronymic, as the Mongols themselves did ("Batu, son of Jochi").
        for (int k = 0; k < 200 && id.name.empty(); ++k) {
            std::string n = rng.pick(pool) + " son of " + rng.pick(kMongol);
            if (!taken.count(n)) id.name = n;
        }
        if (id.name.empty()) id.name = rng.pick(kMongol) + " the Nameless";
    }
    id.appearance = appearanceFor(a, rng);
    return id;
}

std::string epithetFor(const std::string& tag, int times) {
    if (tag == "burned") return "the Burned";
    if (tag == "cheated_death") return times >= 2 ? "the Undying" : "Twice-Born";
    if (tag == "slew_jin") return times >= 2 ? "Ghost-Breaker" : "the Ghost-Slayer";
    if (tag == "paranoid") return "the Watchful";
    if (tag == "arrows") return "Iron-Shield";
    if (tag == "fled") return "the Hare";
    if (tag == "vengeful") return "the Vengeful";
    if (tag == "spared") return "the Spared";
    if (tag == "rival_slain") return "the Usurper";
    return "";
}

} // namespace lw
