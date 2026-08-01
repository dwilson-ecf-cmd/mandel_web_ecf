# Same depth-two region under independent contracts

Existing exact real widths are `R=1/4`, `A=B=1/8`, `C=D=1/16`; lineages are 0, 1, and 2. Splits are the preserved `R -> A,B` and `A -> C,D`.

| Contract | R rank | A/B rank | C/D rank | split 1 / split 2 | Descent | At zero / unresolved |
|---|---:|---:|---:|---|---|---|
| A: maximum depth 2 | 2 | 1 | 0 | allowed / allowed | both child ranks strictly lower; ternary multiset witness remains `9>6>5` | guard may discharge; split rejected; otherwise selected incomplete exhaustion state |
| B: minimum real width 1/16 | 2 exact halvings | 1 | 0 | allowed / allowed for registered real halving | same numeric ranks, now geometric capacity | at width floor, same discharge/exhaustion behavior |
| C: fixed token allowance 2 | 2 | 1 | 0 | allowed / allowed | accounting decrease passes, but token source is only `PARTIAL` | no token permits split; incomplete/fallback if guard fails |
| D: no refinement allowance | 0 | not creatable by split | not creatable | both rejected | no split transition exists | root guard may still discharge; otherwise unresolved/fallback according to non-refinement policy |

The same region legitimately has different ranks under different immutable contracts because rank describes remaining **contract-relative search capacity**, not an intrinsic Mandelbrot property. Certificate validity therefore includes contract identity. This variability would be unacceptable if rank were claimed as contract-free geometry or theorem truth; no such claim is made.
