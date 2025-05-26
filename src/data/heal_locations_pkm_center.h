#define DEFAULT_POKEMON_CENTER_COORDS .x = 7, .y = 4

static const struct HealLocation sHealLocationsPokemonCenter[HEAL_LOCATION_COUNT - 1] =
{
    [HEAL_LOCATION_BATTLE_FRONTIER_OUTSIDE_EAST - 1] =
    {
        .group = MAP_GROUP(BATTLE_FRONTIER_POKEMON_CENTER_1F),
        .map = MAP_NUM(BATTLE_FRONTIER_POKEMON_CENTER_1F),
        DEFAULT_POKEMON_CENTER_COORDS,
    },
};

#undef DEFAULT_POKEMON_CENTER_COORDS

// localIds can be found in the generated events.inc file for the specific heal location map
// e.g. for OldaleTown_PokemonCenter1F/events.inc the following entry gets generated:
// object_event 1, OBJ_EVENT_GFX_NURSE, 7, 2, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, OldaleTown_PokemonCenter_1F_EventScript_Nurse, 0
// In this case the localId is 1.
static const u8 sHealNpcLocalId[HEAL_LOCATION_COUNT - 1] =
{
    [HEAL_LOCATION_DEMO_TOWN - 1] = 1,
    [HEAL_LOCATION_BATTLE_FRONTIER_OUTSIDE_EAST - 1] = 1,
};
