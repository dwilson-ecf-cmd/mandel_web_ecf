# Experiment 0: terminal certificate

`ENGINEERING_INTERPRETATION` — The minimal executable certificate records: formula identity; `c`; `z0`; binary64 representation; the three state values and step labels; radius; strict comparison; conventional count; zero CDC count; field-presence mask; and mapping-decision identity. Bit patterns provide the arithmetic evidence. Deterministic JSON is an audit artifact, not a CDC certificate in the source sense.

The claims are deliberately separate:

1. binary64 produced `z2=6`: validated from trace bits and the stated operation sequence (`ENGINEERING_INTERPRETATION`);
2. exact real orbit reached 6: follows here because the represented integer operations are exact (`ENGINEERING_INTERPRETATION`);
3. `|6|>2`: equivalently `36>4`, with exact operands/results (`ENGINEERING_INTERPRETATION`);
4. conventional orbit escapes: follows from the stipulated conventional strict-radius rule (`ENGINEERING_INTERPRETATION`);
5. CDC reached a terminal state: does **not** follow (`NEGATIVE_RESULT`).

A candidate CDC terminal certificate would additionally require the CDC state, accepted potential values and inequalities for both transitions, preserved invariant, terminal-rule identity, and proof connecting that rule to escape. These fields cannot honestly be populated. `SOURCE_DEFINED` — §23.1, printed p.71 requires state, potential, guards, updates, certificates, halt, invariants, and a global bound where invoked. The verifier therefore reports `NEGATIVE_RESULT`, never calls either orbit transition certified, and rejects absent or mismatched conventional evidence.
