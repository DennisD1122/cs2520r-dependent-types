# cs2520r-dependent-types

Running `main.cpp` gives the following demonstration:

```
Polymorphic identity function
Original expression:  Func(A, Star, Func(x, A, x))
Evaluated expression: Func(A, Star, Func(x, A, x))
Type of expression:   FuncType(A, Star, FuncType(x, A, A))

Applying polymorphic identity function to the number 1
Original expression:  App(App(Func(A, Star, Func(x, A, x)), NatType), Succ(Zero))
Evaluated expression: Succ(Zero)
Type of expression:   NatType

Shadowing
Original expression:  App(App(Func(x, NatType, Func(x, NatType, x)), Zero), Succ(Zero))
Evaluated expression: Succ(Zero)
Type of expression:   NatType

Capture-avoiding substitution
Original expression:  App(Func(x, NatType, Func(y, NatType, App(x, Func(x, NatType, x)))), z)
Evaluated expression: Func(y, NatType, App(z, Func(x, NatType, x)))
Type of expression:   UnknownType

ElimNat addition: 2+3=5
Original expression:  ElimNat(Func(x, NatType, NatType), Succ(Succ(Zero)), Func(_, NatType, Func(x, NatType, Succ(x))), Succ(Succ(Succ(Zero))))
Evaluated expression: Succ(Succ(Succ(Succ(Succ(Zero)))))
Type of expression:   NatType
```
