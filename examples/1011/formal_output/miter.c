#include <assert.h>
#include <stdbool.h>

// Chipbench allows the ai to see the input and output wires (and nothing else)
// this part here is hardcoded right now to 1011, but in the future it may be ai generated ? or systematically 
struct module_RefModule {
    _Bool clk;
    _Bool rst;
    _Bool data;
    _Bool flag;
};

// golden hidden rtl we cannot see
extern struct module_RefModule RefModule;

// hw cbmc methods to acess it
void set_inputs(void);
void next_timeframe(void);
extern _Bool nondet_bool(void);

// HW CBMC can receive one C translation file , so this include
// It remains a separate file right now for readability + it isnt finished
#include "model_formal.c"

int main(void)
{
    ModelState c_model; // cmodel for now, not python
    // Once we move to python, it will still be inserted in C format, just lowered from
    // its original python state

    // HWCBMC init
    model_init(&c_model);

    // Contract reset sequence
    RefModule.rst = 0;
    RefModule.data = 0;
    set_inputs();

    model_step(&c_model, false, false);
    next_timeframe();

    // assert that our hidden RTL is the same as our AI genned
    assert(RefModule.flag == c_model.flag);

    // reset()
    // after reset,  explore shared input vals for both models
    // HW-CBMC unrolls loop to whatevers requested
    while (1) {
        bool data = nondet_bool();

        RefModule.rst = 1;
        RefModule.data = data;
        set_inputs();

        model_step(&c_model, true, data);
        next_timeframe();

        // check equivalence on every declared input
        // these are generated from the public input/output port
        assert(RefModule.flag == c_model.flag);
    }
}