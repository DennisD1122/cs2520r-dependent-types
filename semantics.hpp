#include "string_reps.hpp"
#include "transform_exprs.hpp"

void _evaluate(Expr* expr) {
    if (std::holds_alternative<FuncType>(*expr)) {
        // Eval-Π-domain, Eval-Π-codomain
        FuncType ft = std::get<FuncType>(*expr);
        _evaluate(ft.arg_type);
        _evaluate(ft.body_type);
    }
    else if (std::holds_alternative<Func>(*expr)) {
        // Eval-λ-Body
        Func fun = std::get<Func>(*expr);
        _evaluate(fun.body);
    }
    else if (std::holds_alternative<App>(*expr)) {
        // Eval-Rator, Eval-Rand
        App app = std::get<App>(*expr);
        _evaluate(app.rator);
        _evaluate(app.rand);
        // Eval-App
        if (std::holds_alternative<Func>(*app.rator)) {
            Func rator = std::get<Func>(*app.rator);
            substitute(rator.body, rator.arg_name, app.rand);
            *expr = *rator.body;
            delete rator.arg_name;
            del(rator.arg_type);
            del(app.rand);
        }
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        // Eval-succ
        Succ suc = std::get<Succ>(*expr);
        _evaluate(suc.n);
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        // Eval-elimNat-mot, Eval-elimNat-base, Eval-elimNat-ind, Eval-elimNat-target
        ElimNat en = std::get<ElimNat>(*expr);
        _evaluate(en.mot);
        _evaluate(en.base);
        _evaluate(en.ind);
        _evaluate(en.target);
        // Eval-elimNat-0
        if (std::holds_alternative<Zero>(*en.target)) {
            *expr = *en.base;
        }
        // Eval-elimNat-succ
        if (std::holds_alternative<Succ>(*en.target)) {
            *en.target = *std::get<Succ>(*en.target).n;
            Expr *app = new Expr(App(copy(en.ind), copy(en.target)));
            Expr* expr_cpy = copy(expr);
            *expr = App(app, expr_cpy);
            del(en.base);
            _evaluate(expr);
        }
        del(en.mot);
        del(en.ind);
        del(en.target);
    }
}

Expr* evaluate(Expr* expr) {
    expr = copy(expr);
    std::map<Var,Var> name_map;
    std::map<Var,std::stack<Var>> var_to_syms;
    canonicalize(expr, name_map, var_to_syms);
    _evaluate(expr);
    rename(expr, &name_map);
    return expr;
}

Expr* _type(Expr* expr, std::map<Var,std::stack<Expr*>>& env) {
    if (std::holds_alternative<Var>(*expr)) {
        // Type-Var-Ref
        Var var = std::get<Var>(*expr);
        if (env.find(var) != env.end() && !env[var].empty()) {
            return evaluate(env[var].top());
        }
    }
    else if (std::holds_alternative<Star>(*expr)) {
        // Type-⋆
        return new Expr(Star());
    }
    else if (std::holds_alternative<FuncType>(*expr)) {
        // Type-Π
        FuncType ft = std::get<FuncType>(*expr);
        Expr *arg_type_type = _type(ft.arg_type, env);
        bool arg_type_type_star = std::holds_alternative<Star>(*arg_type_type);
        del(arg_type_type);
        if (arg_type_type_star) {
            env[*ft.arg_name].push(ft.arg_type);
            Expr *body_type_type = _type(ft.body_type, env);
            bool body_type_type_star = std::holds_alternative<Star>(*body_type_type);
            del(body_type_type);
            env[*ft.arg_name].pop();
            if (body_type_type_star) {
                return new Expr(Star());
            }
        }
    }
    else if (std::holds_alternative<Func>(*expr)) {
        // Type-λ
        Func fun = std::get<Func>(*expr);
        Expr *arg_type_type = _type(fun.arg_type, env);
        bool arg_type_type_star = std::holds_alternative<Star>(*arg_type_type);
        del(arg_type_type);
        if (arg_type_type_star) {
            env[*fun.arg_name].push(fun.arg_type);
            Expr *body_type = _type(fun.body, env);
            Expr *body_type_type = _type(body_type, env);
            bool body_type_type_star = std::holds_alternative<Star>(*body_type_type);
            del(body_type_type);
            env[*fun.arg_name].pop();
            if (body_type_type_star) {
                return new Expr(FuncType(new Var(*fun.arg_name), evaluate(fun.arg_type), body_type));
            }
            del(body_type);
        }
    }
    else if (std::holds_alternative<App>(*expr)) {
        // Type-App
        App app = std::get<App>(*expr);
        Expr *rator_type = _type(app.rator, env);
        Expr *rand_type = _type(app.rand, env);
        Expr *t = nullptr;
        if (std::holds_alternative<FuncType>(*rator_type)) {
            FuncType ft = std::get<FuncType>(*rator_type);
            Expr *arg_type = evaluate(ft.arg_type);
            if (to_string(arg_type) == to_string(rand_type)) {
                t = evaluate(ft.body_type);
                substitute(t, ft.arg_name, app.rand);
            }
            del(arg_type);
        }
        del(rator_type);
        del(rand_type);
        if (t) {
            return t;
        }
    }
    else if (std::holds_alternative<NatType>(*expr)) {
        // Type-N
        return new Expr(Star());
    }
    else if (std::holds_alternative<Zero>(*expr)) {
        // Type-0
        return new Expr(NatType());
    }
    else if (std::holds_alternative<Succ>(*expr)) {
        // Type-succ
        Succ suc = std::get<Succ>(*expr);
        Expr *n_type = _type(suc.n, env);
        bool n_type_nat = std::holds_alternative<NatType>(*n_type);
        del(n_type);
        if (n_type_nat) {
            return new Expr(NatType());
        }
    }
    else if (std::holds_alternative<ElimNat>(*expr)) {
        // Type-elimNat
        ElimNat en = std::get<ElimNat>(*expr);
        Expr *mot_type = _type(en.mot, env);
        Expr *base_type = _type(en.base, env);
        Expr *ind_type = _type(en.ind, env);
        Expr *target_type = _type(en.target, env);
        bool b1 = std::holds_alternative<FuncType>(*mot_type) &&
                  std::holds_alternative<NatType>(*std::get<FuncType>(*mot_type).arg_type) &&
                  std::holds_alternative<Star>(*std::get<FuncType>(*mot_type).body_type);
        Expr *x2 = new Expr(App(copy(en.mot), new Expr(Zero())));
        Expr *t2 = evaluate(x2);
        bool b2 = to_string(base_type) == to_string(t2);
        del(x2);
        del(t2);
        bool b3 = false;
        if (std::holds_alternative<FuncType>(*ind_type)) {
            FuncType ft = std::get<FuncType>(*ind_type);
            Var var = *ft.arg_name;
            if (std::holds_alternative<FuncType>(*ft.body_type) && std::holds_alternative<NatType>(*ft.arg_type)) {
                ft = std::get<FuncType>(*ft.body_type);
                Expr *arg_type = evaluate(ft.arg_type);
                Expr *body_type = evaluate(ft.body_type);
                Expr *x3a = new Expr(App(copy(en.mot), new Expr(Var(var))));
                Expr *x3b = new Expr(App(copy(en.mot), new Expr(Succ(new Expr(Var(var))))));
                Expr *t3a = evaluate(x3a);
                Expr *t3b = evaluate(x3b);
                if (to_string(arg_type) == to_string(t3a) && to_string(body_type) == to_string(t3b)) {
                    b3 = true;
                }
                del(arg_type);
                del(body_type);
                del(x3a);
                del(x3b);
                del(t3a);
                del(t3b);
            }
        }
        bool b4 = std::holds_alternative<NatType>(*target_type);
        del(mot_type);
        del(base_type);
        del(ind_type);
        del(target_type);
        if (b1 && b2 && b3 && b4) {
            Expr *r1 = new Expr(App(copy(en.mot), copy(en.target)));
            Expr *r2 = evaluate(r1);
            del(r1);
            return r2;
        }
    }
    return new Expr(UnknownType());
}

Expr* type(Expr* expr) {
    expr = copy(expr);
    std::map<Var,Var> name_map;
    std::map<Var,std::stack<Var>> var_to_syms;
    std::map<Var,std::stack<Expr*>> env;
    canonicalize(expr, name_map, var_to_syms);
    Expr *t = _type(expr, env);
    rename(t, &name_map);
    del(expr);
    return t;
} 
