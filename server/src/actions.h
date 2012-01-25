
// ########################### Actionneurs ###########################


struct actionFct_t{
    void (*fct)(char[8]);
    char id[8];
    struct actionFct_t * nextActionFct;
};

struct action_t{
    char nom[20];
    struct actionFct_t * actionFcts;
    struct action_t * nextAction;
};

struct action_t * actions;