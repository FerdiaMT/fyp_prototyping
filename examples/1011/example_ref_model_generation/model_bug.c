#include <stdbool.h>

typedef enum {
    S0,  // nothing matched
    S1,  // saw: 1
    S2,  // saw: 10
    S3,  // saw: 101
    S4   // saw: 1011
} state_t;

typedef struct {
    state_t state;
    bool flag;
} model_t;

void step(model_t *m, bool rst, bool data)
{
    if (!rst) {
        m->state = S0;
        m->flag = false;
        return;
    }

    state_t next_state;

    switch (m->state) {
        case S0: next_state = data ? S1 : S0; break;
        case S1: next_state = data ? S1 : S2; break;
        case S2: next_state = data ? S3 : S0; break;
        case S3: next_state = data ? S4 : S2; break;
        case S4: next_state = data ? S1 : S2; break;
        default: next_state = S0;             break;
    }

    m->state = next_state;
       
    /* Bug: checks the newly updated state. */
    m->flag = (m->state == S4);