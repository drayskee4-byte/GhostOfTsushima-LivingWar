// Living War - identity generation (names, origins, appearance keys).
#pragma once
#include <set>
#include <string>

#include "lw/model.hpp"
#include "lw/util.hpp"

namespace lw {

struct Identity {
    std::string name;
    std::string origin;
    std::string appearance;
};

// Picks a name not in `taken` (falls back to a numbered variant).
Identity makeIdentity(Rng& rng, Archetype a, const std::set<std::string>& taken);

// Epithet earned from an event tag ("burned", "cheated_death", "slew_jin", ...). Empty if none.
std::string epithetFor(const std::string& eventTag, int times);

} // namespace lw
