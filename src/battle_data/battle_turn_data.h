#ifndef BATTLE_TURN_DATA_PKMN
#define BATTLE_TURN_DATA_PKMN

#include <pokeagb/pokeagb.h>

// executed right before using a move. bm_cb(user_bank)
typedef void (*BeforeMoveCallback)(u8);

enum BuffTags {
    ASSURANCE_TAG = (1 << 0), 
    BEAKBLAST_TAG = (1 << 1),
    COUNTER_TAG = (1 << 2),
    FOCUS_PUNCH_TAG = (1 << 3),
    METAL_BURST_TAG = (1 << 4),
    MIRROR_COAT_TAG = (1 << 5),
    PURSUIT_TAG = (1 << 6),
    SHELLTRAP_TAG = (1 << 7),
    ATTRACT_TAG = (1 << 8),
    BIDE_TAG = (1 << 9),
    DESTINY_BOND_TAG = (1 << 10),
    DISABLE_TAG = (1 << 11),
    GRUDGE_TAG = (1 << 12),
    HEAL_BLOCK_TAG = (1 << 13),
    IMPRISON_TAG = (1 << 14),
    RAGE_TAG = (1 << 15),
    SKY_DROP_TAG = (1 << 16),
    TAUNT_TAG = (1 << 17),
    THROAT_CHOP_TAG = (1 << 18),
};

struct user_turn_action {
    u16 move_id;
    u8 target_bank;
    s8 priority;
    s8 speed;
    
    u8 is_running : 1;
    u8 using_item : 1;
    u8 is_switching : 1;
    u8 skip_move_select : 1;
    
    u8 crit_mod;
    u16 item_to_use;
    u8 switching_slot;
    u8 ability;
    u8 type[2];
    enum BuffTags buff_tag;
    BeforeMoveCallback bmc[19];
    u16 speed_current;
    
    u8 attracted_by_bank;
    u8 imprisoned_by_bank;
    u8 charging_move_counter;
    u8 turns_disabled;
    u8 heal_block_turns;
    u16 total_dmg_taken;
    u16 special_dmg_taken;
    u16 physical_dmg_taken;
};

#endif /* BATTLE_TURN_DATA_PKMN */

