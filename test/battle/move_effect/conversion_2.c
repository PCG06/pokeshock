#include "global.h"
#include "test/battle.h"

TO_DO_BATTLE_TEST("Conversion 2's type change considers Inverse Battles");

SINGLE_BATTLE_TEST("Conversion 2 randomly changes the type of the user to a type that resists the last move that hit the user (Gen 3-4)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 < GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet used Ominous Wind!");
        // turn 1
        ONE_OF {
         MESSAGE("Foe Wobbuffet transformed into the Normal type!");
         MESSAGE("Foe Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2 randomly changes the type of the user to a type that resists the last used target's move (Gen 5+)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet used Ominous Wind!");
        // turn 1
        ONE_OF {
         MESSAGE("Foe Wobbuffet transformed into the Normal type!");
         MESSAGE("Foe Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers status moves (Gen 5+)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CURSE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Curse!");
        // turn 2
        ONE_OF {
         MESSAGE("Wobbuffet transformed into the Normal type!");
         MESSAGE("Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers the type of moves called by other moves")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_MIRROR_MOVE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Mirror Move!");
        // turn 2
        ONE_OF {
         MESSAGE("Wobbuffet transformed into the Normal type!");
         MESSAGE("Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers dynamic type moves")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_WEATHER_BALL); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Weather Ball!");
        // turn 2
        ONE_OF {
         MESSAGE("Wobbuffet transformed into the Steel type!");
         MESSAGE("Wobbuffet transformed into the Fire type!");
         MESSAGE("Wobbuffet transformed into the Water type!");
         MESSAGE("Wobbuffet transformed into the Ice type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers move types changed by Normalize and Electrify")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NORMALIZE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Water Gun!");
        // turn 2
        ONE_OF {
         MESSAGE("Wobbuffet transformed into the Steel type!");
         MESSAGE("Wobbuffet transformed into the Rock type!");
         MESSAGE("Wobbuffet transformed into the Ghost type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers Struggle to be Normal type (Gen 3-4)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 < GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STRUGGLE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Struggle!");
        // turn 2
        ONE_OF {
         MESSAGE("Wobbuffet transformed into the Steel type!");
         MESSAGE("Wobbuffet transformed into the Rock type!");
         MESSAGE("Wobbuffet transformed into the Ghost type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change fails targeting Struggle (Gen 5+)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STRUGGLE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Wobbuffet used Struggle!");
        // turn 2
        MESSAGE("Wobbuffet used Conversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if the move used is of typeless damage (Gen 5+)")
{
    GIVEN {
        ASSUME(B_UPDATED_CONVERSION_2 >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ENTEI);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BURN_UP); }
        TURN { MOVE(opponent, MOVE_REVELATION_DANCE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Foe Entei used Burn Up!");
        // turn 2
        MESSAGE("Foe Entei used Revelation Dance!");
        // turn 3
        MESSAGE("Wobbuffet used Conversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if the targeted move is Stellar Type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { TeraType(TYPE_STELLAR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_BLAST, gimmick: GIMMICK_TERA); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet used Tera Blast!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_BLAST, player);
        // turn 1
        MESSAGE("Foe Wobbuffet used Conversion 2!");
        MESSAGE("But it failed!");
    }
}
