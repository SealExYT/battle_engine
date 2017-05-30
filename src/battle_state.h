#ifndef BATTLE_STATE_H_
#define BATTLE_STATE_H_

#include <pokeagb/pokeagb.h>

struct battle_selection_cursor {
    u8 objid;
    u8 objid_mv_crsr[2];
    u8 cursor_pos; // 0-3
    u8 cursor_x[6];
    u8 cursor_y[6];
    
};

struct battle_main {
    struct battle_selection_cursor battle_cursor;
    u8 selected_option;
    u8 type_objid[4];
    u8 move_name_objid[4];
    u8 fight_menu_content_spawned;
};

enum fight_menu {
    OPTION_FIGHT,
    OPTION_POKEMON,
    OPTION_BAG,
    OPTION_RUN
};

extern struct battle_main* battle_master;
//extern pchar (*multi_string_buff)[20];
extern u8 bs_anim_status;

#endif /* BATTLE_STATE_H_ */