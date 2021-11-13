typedef enum {
              Fighter,
              Mage,
              Support
} player_class_type;

extern unsigned char player_name[4][5];
extern unsigned char player_str[4];
extern unsigned char player_int[4];
extern unsigned char player_wis[4];
extern unsigned char player_dex[4];
extern unsigned char player_con[4];
extern unsigned int player_hp[4];
extern unsigned int player_max_hp[4];
extern unsigned int player_mp[4];
extern unsigned int player_max_mp[4];
extern unsigned int player_xp[4];
extern unsigned int player_lv[4];
extern player_class_type player_class[4];

void initialize_party(void);
