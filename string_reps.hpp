#include "types.hpp"

std::vector<std::string> get_args(std::string str, int start_idx) {
    std::vector<std::string> args;
    int parens = 0;
    int i;
    for (i = start_idx; i < str.length(); ++i) {
        if (str[i] == '(') {
            parens++;
        }
        else if (str[i] == ')') {
            parens--;
            if (parens < 0) break;
        }
        else if (str[i] == ',' && parens == 0) {
            args.push_back(str.substr(start_idx, i-start_idx));
            start_idx = i+1;
        }
    }
    args.push_back(str.substr(start_idx, i-start_idx));
    return args;
}

Expr* _parse(std::string str) {
    if (str == "Star") {
        return new Expr(Star());
    }
    else if (str.substr(0, 9) == "FuncType(") {
        std::vector<std::string> args = get_args(str, 9);
        Var *v = new Var(args[0]);
        Expr *e1 = _parse(args[1]);
        Expr *e2 = _parse(args[2]);
        return new Expr(FuncType(v, e1, e2));
    }
    else if (str.substr(0, 5) == "Func(") {
        std::vector<std::string> args = get_args(str, 5);
        Var *v = new Var(args[0]);
        Expr *e1 = _parse(args[1]);
        Expr *e2 = _parse(args[2]);
        return new Expr(Func(v, e1, e2));
    }
    else if (str.substr(0, 4) == "App(") {
        std::vector<std::string> args = get_args(str, 4);
        Expr *e1 = _parse(args[0]);
        Expr *e2 = _parse(args[1]);
        return new Expr(App(e1, e2));
    }
    else if (str == "NatType") {
        return new Expr(NatType());
    }
    else if (str == "Zero") {
        return new Expr(Zero());
    }
    else if (str.substr(0, 5) == "Succ(") {
        std::vector<std::string> args = get_args(str, 5);
        Expr *e = _parse(args[0]);
        return new Expr(Succ(e));
    }
    else if (str.substr(0, 8) == "ElimNat(") {
        std::vector<std::string> args = get_args(str, 8);
        Expr *e1 = _parse(args[0]);
        Expr *e2 = _parse(args[1]);
        Expr *e3 = _parse(args[2]);
        Expr *e4 = _parse(args[3]);
        return new Expr(ElimNat(e1, e2, e3, e4));
    }
    return new Expr(Var(str));
}

Expr* parse(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return _parse(str);
}

std::string to_string(Expr* expr) {
    std::stringstream ss;
    if (std::holds_alternative<Var>(*expr)) {
        Var var = std::get<Var>(*expr);
        ss << var;
    }
    else if (std::holds_alternative<Star>(*expr)) {
        ss << "Star";
    }
    else if (std::holds_alternative<FuncType>(*expr)) {
        FuncType ft = std::get<FuncType>(*expr);
        ss << "FuncType(" << *ft.arg_name << ", " << to_string(ft.arg_type) << ", " << to_string(ft.body_type) << ")";
    }
    else if (std::holds_alternative<Func>(*expr)) {
        Func fun = std::get<Func>(*expr);
        ss << "Func(" << *fun.arg_name << ", " << to_string(fun.arg_type) << ", " << to_string(fun.body) << ")";
    }
    else if (std::holds_alternative<App>(*expr)) {
        App app = std::get<App>(*expr);
        ss << "App(" << to_string(app.rator) << ", " << to_string(app.rand) << ")";
    }
    else if (std::holds_alternative<NatType>(*expr)) {
        ss << "NatType";
    }
    else if (std::holds_alternative<Zero>(*expr)) {
        ss << "Zero";
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        Succ suc = std::get<Succ>(*expr);
        ss << "Succ(" << to_string(suc.n) << ")";
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        ElimNat en = std::get<ElimNat>(*expr);
        ss << "ElimNat(" << to_string(en.mot) << ", " << to_string(en.base) << ", " << to_string(en.ind) << ", " << to_string(en.target) << ")";
    }
    else if (std::holds_alternative<UnknownType>(*expr)) {
        ss << "UnknownType";
    }
    return ss.str();
}
