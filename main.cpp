#include "semantics.hpp"

void display(std::string str_expr, std::string message) {
    Expr *expr = parse(str_expr);
    Expr *expr_eval = evaluate(expr);
    Expr *t = type(expr);
    std::cout << message << "\n";
    std::cout << "Original expression:  " << to_string(expr) << "\n";
    std::cout << "Evaluated expression: " << to_string(expr_eval) << "\n";
    std::cout << "Type of expression:   " << to_string(t) << "\n";
    std::cout << "\n";
    del(expr);
    del(expr_eval);
    del(t);
}

int main() {
    display("Func(A, Star, Func(x, A, x))",
            "Polymorphic identity function");
    display("App(App(Func(A, Star, Func(x, A, x)), NatType), Succ(Zero))",
            "Applying polymorphic identity function to the number 1");
    display("App(App(Func(x, NatType, Func(x, NatType, x)), Zero), Succ(Zero))",
            "Shadowing");
    display("App(Func(x, NatType, Func(y, NatType, App(x, Func(x, NatType, x)))), z)",
            "Capture-avoiding substitution");
    display("ElimNat("
                "Func(x, NatType, NatType),"
                "Succ(Succ(Zero)),"
                "Func(_, NatType, Func(x, NatType, Succ(x))),"
                "Succ(Succ(Succ(Zero)))"
            ")", "ElimNat addition: 2+3=5");
}
