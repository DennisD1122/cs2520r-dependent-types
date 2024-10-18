#include "types.hpp"

Expr* copy(Expr* expr) {
    if (std::holds_alternative<Var>(*expr)) {
        return new Expr(*expr);
    }
    else if (std::holds_alternative<Star>(*expr)) {
        return new Expr(Star());
    }
    else if (std::holds_alternative<FuncType>(*expr)) {
        FuncType ft = std::get<FuncType>(*expr);
        Var *v = new Var(*ft.arg_name);
        Expr *e1 = copy(ft.arg_type);
        Expr *e2 = copy(ft.body_type);
        return new Expr(FuncType(v, e1, e2));
    }
    else if (std::holds_alternative<Func>(*expr)) {
        Func fun = std::get<Func>(*expr);
        Var *v = new Var(*fun.arg_name);
        Expr *e1 = copy(fun.arg_type);
        Expr *e2 = copy(fun.body);
        return new Expr(Func(v, e1, e2));
    }
    else if (std::holds_alternative<App>(*expr)) {
        App app = std::get<App>(*expr);
        Expr *e1 = copy(app.rator);
        Expr *e2 = copy(app.rand);
        return new Expr(App(e1, e2));
    }
    else if (std::holds_alternative<NatType>(*expr)) {
        return new Expr(NatType());
    }
    else if (std::holds_alternative<Zero>(*expr)) {
        return new Expr(Zero());
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        Succ suc = std::get<Succ>(*expr);
        Expr *e = copy(suc.n);
        return new Expr(Succ(e));
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        ElimNat en = std::get<ElimNat>(*expr);
        Expr *e1 = copy(en.mot);
        Expr *e2 = copy(en.base);
        Expr *e3 = copy(en.ind);
        Expr *e4 = copy(en.target);
        return new Expr(ElimNat(e1, e2, e3, e4));
    }
    return nullptr;
}

void del(Expr* expr) {
    if (std::holds_alternative<FuncType>(*expr)) {
        FuncType ft = std::get<FuncType>(*expr);
        delete ft.arg_name;
        del(ft.arg_type);
        del(ft.body_type);
    }
    else if (std::holds_alternative<Func>(*expr)) {
        Func fun = std::get<Func>(*expr);
        delete fun.arg_name;
        del(fun.arg_type);
        del(fun.body);
    }
    else if (std::holds_alternative<App>(*expr)) {
        App app = std::get<App>(*expr);
        del(app.rator);
        del(app.rand);
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        Succ suc = std::get<Succ>(*expr);
        del(suc.n);
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        ElimNat en = std::get<ElimNat>(*expr);
        del(en.mot);
        del(en.base);
        del(en.ind);
        del(en.target);
    }
    delete expr;
}

void substitute_or_rename(Expr* expr, Var* var, Expr* sub, std::map<Var,Var>* map, bool use_rename) {
    if (std::holds_alternative<Var>(*expr)) {
        if (use_rename) {
            *expr = (*map)[std::get<Var>(*expr)];
        }
        else if (std::get<Var>(*expr) == *var) {
            *expr = *copy(sub);
        }
    }
    else if (std::holds_alternative<FuncType>(*expr)) {
        FuncType ft = std::get<FuncType>(*expr);
        if (use_rename) {
            *ft.arg_name = (*map)[*ft.arg_name];
        }
        substitute_or_rename(ft.arg_type, var, sub, map, use_rename);
        substitute_or_rename(ft.body_type, var, sub, map, use_rename);
    }
    else if (std::holds_alternative<Func>(*expr)) {
        Func fun = std::get<Func>(*expr);
        if (use_rename) {
            *fun.arg_name = (*map)[*fun.arg_name];
        }
        substitute_or_rename(fun.arg_type, var, sub, map, use_rename);
        substitute_or_rename(fun.body, var, sub, map, use_rename);
    }
    else if (std::holds_alternative<App>(*expr)) {
        App app = std::get<App>(*expr);
        substitute_or_rename(app.rator, var, sub, map, use_rename);
        substitute_or_rename(app.rand, var, sub, map, use_rename);
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        Succ suc = std::get<Succ>(*expr);
        substitute_or_rename(suc.n, var, sub, map, use_rename);
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        ElimNat en = std::get<ElimNat>(*expr);
        substitute_or_rename(en.mot, var, sub, map, use_rename);
        substitute_or_rename(en.base, var, sub, map, use_rename);
        substitute_or_rename(en.ind, var, sub, map, use_rename);
        substitute_or_rename(en.target, var, sub, map, use_rename);
    }
}

void substitute(Expr* expr, Var* var, Expr* sub) {
    substitute_or_rename(expr, var, sub, nullptr, false);
}

void rename(Expr* expr, std::map<Var,Var>* map) {
    substitute_or_rename(expr, nullptr, nullptr, map, true);
}

Var gensym(bool reset = false) {
    static int n = 0;
    if (reset) n = 0;
    return std::to_string(n++);
}

void canonicalize(Expr* expr, std::map<Var,Var>& sym_to_var, std::map<Var,std::stack<Var>>& var_to_syms) {
    if (std::holds_alternative<Var>(*expr)) {
        Var var = std::get<Var>(*expr);
        if (var_to_syms[var].empty()) {
            Var new_sym = gensym();
            var_to_syms[var].push(new_sym);
            sym_to_var[new_sym] = var;
        }
        *expr = var_to_syms[var].top();
    }
    else if (std::holds_alternative<FuncType>(*expr)) {
        FuncType ft = std::get<FuncType>(*expr);
        canonicalize(ft.arg_type, sym_to_var, var_to_syms);
        Var var = *ft.arg_name;
        Var new_sym = gensym();
        var_to_syms[var].push(new_sym);
        sym_to_var[new_sym] = var;
        *ft.arg_name = new_sym;
        canonicalize(ft.body_type, sym_to_var, var_to_syms);
        var_to_syms[var].pop();
    }
    else if (std::holds_alternative<Func>(*expr)) {
        Func fun = std::get<Func>(*expr);
        canonicalize(fun.arg_type, sym_to_var, var_to_syms);
        Var var = *fun.arg_name;
        Var new_sym = gensym();
        var_to_syms[var].push(new_sym);
        sym_to_var[new_sym] = var;
        *fun.arg_name = new_sym;
        canonicalize(fun.body, sym_to_var, var_to_syms);
        var_to_syms[var].pop();
    }
    else if (std::holds_alternative<App>(*expr)) {
        App app = std::get<App>(*expr);
        canonicalize(app.rator, sym_to_var, var_to_syms);
        canonicalize(app.rand, sym_to_var, var_to_syms);
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        Succ suc = std::get<Succ>(*expr);
        canonicalize(suc.n, sym_to_var, var_to_syms);
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        ElimNat en = std::get<ElimNat>(*expr);
        canonicalize(en.mot, sym_to_var, var_to_syms);
        canonicalize(en.base, sym_to_var, var_to_syms);
        canonicalize(en.ind, sym_to_var, var_to_syms);
        canonicalize(en.target, sym_to_var, var_to_syms);
    }
}
