#include <systemc.h>

SC_MODULE(TopModule) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> data;
    sc_out<bool> flag;

    sc_uint<3> state;

    enum {
        S0 = 0, S1 = 1, S2 = 2, S3 = 3, S4 = 4
    };

    void evaluate() {
        // Active-low asynchronous reset.
        if (!rst.read()) {
            state = S0;
            flag.write(false);
            return;
        }

        sc_uint<3> old_state = state;
        sc_uint<3> next_state;

        if (old_state == S0)
            next_state = data.read() ? S1 : S0;
        else if (old_state == S1)
            next_state = data.read() ? S1 : S2;
        else if (old_state == S2)
            next_state = data.read() ? S3 : S0;
        else if (old_state == S3)
            next_state = data.read() ? S4 : S2;
        else if (old_state == S4)
            next_state = data.read() ? S1 : S2;
        else
            next_state = S0;

        // Uses old state, matching: flag <= (state == S4).
        flag.write(old_state == S4);
        state = next_state;
    }

    SC_CTOR(TopModule) {
        SC_METHOD(evaluate);
        sensitive << clk.pos() << rst.neg();
    }
};