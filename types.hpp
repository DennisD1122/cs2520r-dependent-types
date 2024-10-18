#ifndef TYPES_HPP
#define TYPES_HPP

#include "headers.hpp"

using Var = std::string;
struct Star;
struct FuncType;
struct Func;
struct App;
struct NatType;
struct Zero;
struct Succ;
struct ElimNat;
struct UnknownType;

using Expr = std::variant<Var, Star, FuncType, Func, App, NatType, Zero, Succ, ElimNat, UnknownType>;

struct Star {};

struct FuncType {
    Var *arg_name;
    Expr *arg_type;
    Expr *body_type;
    FuncType(Var *arg_name, Expr *arg_type, Expr *body_type) {
        this->arg_name = arg_name;
        this->arg_type = arg_type;
        this->body_type = body_type;
    }
};

struct Func {
    Var *arg_name;
    Expr *arg_type;
    Expr *body;
    Func(Var *arg_name, Expr *arg_type, Expr *body) {
        this->arg_name = arg_name;
        this->arg_type = arg_type;
        this->body = body;
    }
};

struct App {
    Expr *rator;
    Expr *rand;
    App(Expr *rator, Expr *rand) {
        this->rator = rator;
        this->rand = rand;
    }
};

struct NatType {};

struct Zero {};

struct Succ {
    Expr *n;
    Succ(Expr *n) {
        this->n = n;
    }
};

struct ElimNat {
    Expr *mot;
    Expr *base;
    Expr *ind;
    Expr *target;
    ElimNat(Expr *mot, Expr *base, Expr *ind, Expr *target) {
        this->mot = mot;
        this->base = base;
        this->ind = ind;
        this->target = target;
    }
};

struct UnknownType {};

#endif
