void fe_irc_dcc_init(void);void fe_irc_notifylist_init(void);
void fe_irc_notifylist_deinit(void);void fe_irc_dcc_deinit(void);
void fe_irc_modules_init(void) {  fe_irc_dcc_init(); fe_irc_notifylist_init(); }
void fe_irc_modules_deinit(void) { fe_irc_notifylist_deinit(); fe_irc_dcc_deinit();  }
