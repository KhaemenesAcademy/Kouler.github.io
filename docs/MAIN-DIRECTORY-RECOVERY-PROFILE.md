# Main Directory Recovery Profile

The certified failure is an inert V5 replacement paired with a CSS rule that hides
the legacy `#tree`.

Kouler v0.1.0 removes only:

```css
#tree{
  display:none !important;
}
```

It requires exactly one inert V5 marker, zero active V5 markers, and exactly one
`/v1/directory` endpoint marker before mutation. It accepts no target argument.
After ABBY performs the atomic replacement, Kouler re-reads through ABBY and
requires exact postimage equality.

This profile does not migrate Directory data, activate V5, change Gaffer, or
activate terminals.
