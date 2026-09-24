#include <cxxrtl/cxxrtl.h>

using namespace cxxrtl_yosys;

namespace cxxrtl_design {

struct p_TopModule : public module {
    // Public ports.
    value<1> p_clk;
    value<1> p_rst;
    value<1> p_data;
    wire<1> p_flag;

    // Internal state and previous clock value.
    wire<3> state;
    value<1> prev_p_clk;

    bool posedge_clk() const {
        return !prev_p_clk.slice<0>().val() && p_clk.slice<0>().val();
    }

    void reset() override {
        state.curr = value<3>{0u};
        state.next = value<3>{0u};
        p_flag.curr = value<1>{0u};
        p_flag.next = value<1>{0u};
        prev_p_clk = value<1>{0u};
    }

    bool eval(performer *performer = nullptr) override {
        const bool reset_active = (p_rst.data[0] & 1u) == 0;
        const bool rising_edge = posedge_clk();

        if (reset_active) {
            state.next = value<3>{0u};
            p_flag.next = value<1>{0u};
        } else if (rising_edge) {
            const unsigned old_state = state.curr.data[0] & 0x7u;
            const bool data = (p_data.data[0] & 1u) != 0;

            unsigned next_state = 0;
            switch (old_state) {
                case 0: next_state = data ? 1 : 0; break;
                case 1: next_state = data ? 1 : 2; break;
                case 2: next_state = data ? 3 : 0; break;
                case 3: next_state = data ? 4 : 2; break;
                case 4: next_state = data ? 1 : 2; break;
                default: next_state = 0;           break;
            }

            p_flag.next = value<1>{old_state == 4 ? 1u : 0u};
            state.next = value<3>{next_state};
        }

        return true;
    }

    bool commit() override {
        observer observer;
        bool changed = false;

        if (state.commit(observer))
            changed = true;
        if (p_flag.commit(observer))
            changed = true;

        prev_p_clk = p_clk;
        return changed;
    }
};

} // namespace cxxrtl_design