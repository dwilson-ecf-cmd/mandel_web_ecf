# Experiment 0: finite-precision evidence

`ENGINEERING_INTERPRETATION` — Under IEC/IEEE binary64 semantics, `+0`, `2`, and `6` are exactly representable with bits `0000000000000000`, `4000000000000000`, and `4018000000000000`. For the stated operation order, every multiplication/addition/subtraction in `0+0i→2+0i→6+0i`, and squared magnitudes `4` and `36`, has an exact representable result. The threshold square is exact, and ordered finite comparisons give `4>4` false and `36>4` true.

`ENGINEERING_INTERPRETATION` — Minimum verifier evidence is: binary64 identity; formula/order; exact point, initial value and radius bits; step labels; all real/imaginary result bits; strict comparison true; conventional count two; and zero CDC count. The verifier checks these rather than trusting serialized assertions.

`UNRESOLVED` — This is not a generalized floating-point proof rule. Arbitrary points can round; intermediate multiplication/addition can overflow or underflow; NaN makes ordered comparisons false; infinity may arise before useful evidence. The conventional backend already treats non-finite state or magnitude as numerically uncertain rather than escape evidence.

`ENGINEERING_INTERPRETATION` — Portability assumes C11 `double` has the binary64 format asserted by the evidence (the bit checks fail otherwise), eight-bit bytes for the published hexadecimal interpretation, no reassociation/fast-math contraction that changes the stated operations, and ordinary finite ordered comparisons. The build does not request fast-math. A production portable proof format would add compile-time/runtime format declarations; this narrowly scoped verifier instead rejects any differing representation through exact bit validation.
