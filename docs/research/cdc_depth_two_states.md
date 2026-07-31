# Composed proof states

An entry is `identity:region/rank/evidence/unresolved/token/lineage`. Evidence is conventional unless identified as CDC bookkeeping. Retired parents remain immutable lineage records, never active obligations.

| State | Active obligations and evidence | Potential | Retired structure |
|---|---|---:|---|
| S0 | `R:R/2/root-bounds/theorem(R)/9/root` | 9 | none |
| S1 | `A:A/1/split_1-cover/theorem(A)/3/R`; `B:B/1/split_1-cover/theorem(B)/3/R` | 6 | R plus immutable split_1 certificate |
| S2 | `C:C/0/split_2-cover/theorem(C)/1/A`; `D:D/0/split_2-cover/theorem(D)/1/A`; `B:B/1/unchanged/theorem(B)/3/R` | 5 | R, A and both immutable split certificates |
| S3 | `D:D/0/guard(D)/theorem(D)/1/A`; `B:B/1/unchanged/theorem(B)/3/R` | 4 | C discharge appended |
| S4 | `B:B/1/unchanged/theorem(B)/3/R` | 3 | D discharge appended |
| S5 | empty | 0 | B discharge appended; all leaf theorems and coverage establish R |

The batch path is `S0(9)->S1(6)->S2(5)->empty(0)` when C, D, and B each already has independently verified conventional guard evidence. A stalled guard leaves its obligation, evidence, and potential unchanged and reports `UNRESOLVED`; it is not a CDC transition. Proof-sheet tokens are exactly `3^rank`. CDC owns validated replacements/discharges and bookkeeping; conventional mathematics owns region truth, guards, coverage, inequalities, and terminal theorems.
