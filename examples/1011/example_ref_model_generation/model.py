class TopModule:
    # State encoding matches the RTL parameters.
    S0 = 0
    S1 = 1
    S2 = 2
    S3 = 3
    S4 = 4

    def __init__(self):
        self.state = self.S0 
        self.flag = 0 

    def step(self, rst: int, data: int) -> int:
        rst = rst & 0x1
        data = data & 0x1

        if rst == 0:
            self.state = self.S0
            self.flag = 0
            return self.flag

        old_state = self.state

        if old_state == self.S0:
            next_state = self.S1 if data else self.S0
        elif old_state == self.S1:
            next_state = self.S1 if data else self.S2
        elif old_state == self.S2:
            next_state = self.S3 if data else self.S0
        elif old_state == self.S3:
            next_state = self.S4 if data else self.S2
        elif old_state == self.S4:
            next_state = self.S1 if data else self.S2
        else:
            next_state = self.S0

        next_flag = 1 if old_state == self.S4 else 0

        self.state = next_state
        self.flag = next_flag

        return self.flag

    def eval(self, inputs: dict) -> dict:
        # Chipbenchy api which allows us to call this like c or rtl
        rst = inputs["rst"]
        data = inputs["data"]

        return {"flag": self.step(rst, data)}