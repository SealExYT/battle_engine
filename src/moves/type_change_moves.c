#include <pokeagb/pokeagb.h>
#include "../battle_data/pkmn_bank.h"
#include "../battle_data/pkmn_bank_stats.h"
#include "../battle_data/battle_state.h"

extern void dprintf(const char * str, ...);
extern bool enqueue_message(u16 move, u8 bank, enum battle_string_ids id, u16 effect);
extern bool b_pkmn_has_type(u8 bank, enum PokemonType type);
extern bool b_pkmn_set_type(u8 bank, enum PokemonType type);
extern bool b_pkmn_add_type(u8 bank, enum PokemonType type);
extern u16 rand_range(u16, u16);

/* Conversion */
u8 conversion_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    // get type of move in first slot
    u16 first_move = pokemon_getattr(B_PKMN(user), REQUEST_MOVE1, NULL);
    u8 type = moves[first_move].type;
    if (b_pkmn_has_type(user, type)) {
        return false;
    }

    // X transformed\ninto the Y type!
    enqueue_message(move, user, STRING_CONVERSION_TYPE, type);
    return b_pkmn_set_type(user, type);
}


/* Conversion 2 */
u8 conversion_two_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 last_type = MOVE_TYPE(LAST_MOVE(TARGET_OF(user)));
    u8 possible_types[MTYPE_NONE];
    memset(&possible_types, MTYPE_NONE, MTYPE_NONE);
    if (last_type == MTYPE_EGG)
        return false;
    u8 array_index = 0;
    for (u8 i = 0; i < MTYPES_MAX; i++) {
        if (b_pkmn_has_type(user, i))
            continue;
        u16 effectiveness = MOVE_EFFECTIVENESS(i, last_type);
        if (effectiveness < 100) {
            possible_types[array_index] = i;
            array_index++;
        }
    }
    u8 type = possible_types[rand_range(0, array_index)];
    enqueue_message(move, user, STRING_CONVERSION_TYPE, type);
    return b_pkmn_set_type(user, type);
}


/* Reflect type */
u8 reflect_type_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 defender = TARGET_OF(user);
    u8 types[3];
    types[0] = B_PKMN_TYPE(defender, 0);
    types[1] = B_PKMN_TYPE(defender, 1);
    types[2] = B_PKMN_TYPE(defender, 2);
    // fail if target lost it's types
    if ((types[0] == MTYPE_EGG) && (types[1] == MTYPE_EGG) && (types[2] == MTYPE_EGG)) {
        return false;
    }
    // fail if arceus or silvally
    u16 species = p_bank[user]->b_data.species;
    if ((species == SPECIES_ARCEUS) || (species == SPECIES_SILVALLY)) {
        return false;
    }

    if ((types[0] == MTYPE_EGG) && (types[1] == MTYPE_EGG) && (types[2] != MTYPE_EGG)) {
        B_PKMN_TYPE(user, 0) = MTYPE_NORMAL;
        B_PKMN_TYPE(user, 1) = MTYPE_EGG;
        B_PKMN_TYPE(user, 2) = types[2];
    } else {
        B_PKMN_TYPE(user, 0) = types[0];
        B_PKMN_TYPE(user, 1) = types[1];
        B_PKMN_TYPE(user, 2) = types[2];
    }

    //X's type\nchanged to match the Foe Y's!
    enqueue_message(0, user, STRING_REFLECT_TYPE_MATCHED, 0);
    return true;
}


/* Soak */
u8 soak_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    if (b_pkmn_set_type(TARGET_OF(src), MTYPE_WATER))
        enqueue_message(move, TARGET_OF(src), STRING_CONVERSION_TYPE, MTYPE_WATER);
    else
        return false;
    return true;
}


/* Flying Press */
u8 flying_press_on_modify_move(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    B_MOVE_TYPE(user, 1) = MTYPE_FLYING;
    if (HAS_VOLATILE(TARGET_OF(user), VOLATILE_MINIMIZE)) {
        B_MOVE_ACCURACY(user) = 101;
    }
    return true;
}

void flying_press_on_base_power(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    u8 base_power = B_MOVE_POWER(user);
    if (HAS_VOLATILE(TARGET_OF(user), VOLATILE_MINIMIZE)) {
        if ((base_power << 1) < base_power) {
            B_MOVE_POWER(user) = 255;
        } else {
            B_MOVE_POWER(user) *= 2;
        }
    }
    B_MOVE_POWER(user) = base_power;
}


/* Trick-or-Treat */
u8 trick_or_treat_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    // The Type type was added to\nthe foe defender!
    u8 defender = TARGET_OF(user);
    if (b_pkmn_has_type(defender, MTYPE_GHOST))
        return false;
    B_PKMN_TYPE(defender, 2) = MTYPE_GHOST;
    enqueue_message(move, user, STRING_TYPE_ADDED, MTYPE_GHOST);
    return true;
}


/* Ion Deluge */
u8 ion_deluge_on_modify_move_anon(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (B_MOVE_TYPE(user, 0) == MTYPE_NORMAL) {
        B_MOVE_TYPE(user, 0) = MTYPE_ELECTRIC;
    } else if (B_MOVE_TYPE(user, 1) == MTYPE_NORMAL) {
        B_MOVE_TYPE(user, 0) = MTYPE_ELECTRIC;
    }
    return true;
}

u8 ion_deluge_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    // a deluge of ions showers the battlefield!
    enqueue_message(move, user, STRING_ION_DELUGE, 0);
    add_callback(CB_ON_TRYHIT_MOVE, -2, 0, user, (u32)(ion_deluge_on_modify_move_anon));
    return true;
}


/* Forest's Curse */
u8 forests_curse_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    // The Type type was added to\nthe foe defender!
    u8 defender = TARGET_OF(user);
    if (b_pkmn_has_type(defender, MTYPE_GRASS))
        return false;
    B_PKMN_TYPE(defender, 2) = MTYPE_GRASS;
    enqueue_message(move, user, STRING_TYPE_ADDED, MTYPE_GRASS);
    return true;
}


/* Electrify */
u8 electrify_on_tryhit(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    return (user == battle_master->first_bank);
}

u8 electrify_on_modify_move_anon(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user == TARGET_OF(src)) {
        B_MOVE_TYPE(user, 0) = MTYPE_ELECTRIC;
        acb->in_use = false;
    }
    return true;
}

u8 electrify_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    enqueue_message(move, user, STRING_ELECTRIFIED, MTYPE_GRASS);
    add_callback(CB_ON_TRYHIT_MOVE, -2, 0, user, (u32)(electrify_on_modify_move_anon));
    return true;
}


/* Burn up */
u8 burn_up_on_tryhit(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    return b_pkmn_has_type(user, MTYPE_FIRE);
}

u8 burn_up_on_effect(u8 user, u8 src, u16 move, struct anonymous_callback* acb)
{
    if (user != src) return true;
    for (u8 i = 0; i < sizeof(p_bank[user]->b_data.type); i++) {
        if (B_PKMN_TYPE(user, i) == MTYPE_FIRE) {
            B_PKMN_TYPE(user, i) = MTYPE_EGG;
        }
    }
    enqueue_message(move, user, STRING_BURNT_OUT, 0);
    return true;
}
