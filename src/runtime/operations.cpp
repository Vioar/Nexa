// operations.cpp

#include <cmath>

#include "runtime/operations.hpp"
#include "common/error.hpp"
#include "runtime/value.hpp"

namespace Operations {
    bool isNumber(ValueType t) {
        return t == ValueType::Int || t == ValueType::Float || t == ValueType::Bool;
    }

    bool isComparisonOp(Op op) {
        return op == Op::Less || op == Op::Greater || op == Op::LessEqual || op == Op::GreaterEqual;
    }

    bool isEqualityOp(Op op) {
        return op == Op::Equal || op == Op::NotEqual;
    }

    ValueType promote(ValueType a, ValueType b) {
        if (a == ValueType::Float || b == ValueType::Float) {
            return ValueType::Float;
        }

        return ValueType::Int;
    }

    Value applyNumericOp(Op op, double a, double b, ValueType resultType, SourceLocation loc) {
        double result;

        switch (op) {
            case Op::Add: result = a+b; break;
            case Op::Sub: result = a-b; break;
            case Op::Div: {
                if (b == 0.0) {
                    throw MathError("Division by zero", loc);
                }

                result = a/b;
                resultType = ValueType::Float;
                break;
            }
            case Op::Mul: result = a*b; break;
            case Op::Pow: result = std::pow(a, b); break;
            default: throw MathError("Unsupported operation", loc);
        }

        if (resultType == ValueType::Int) {
            return {ValueType::Int, static_cast<int>(result)};
        }

        return {ValueType::Float, result};
    }

    Value applyOp(Op op, const Value& va, const Value& vb, SourceLocation loc) {
        if (isEqualityOp(op)) {
            return applyEquality(op, va, vb, loc);
        }

        if (va.type == ValueType::String && vb.type == ValueType::String) {
            return applyStringOp(op, va, vb, loc);
        }

        if (isNumber(va.type) && isNumber(vb.type)) {
            double a = va.asFloat();
            double b = vb.asFloat();

            if (isComparisonOp(op)) {
                return applyComparison(op, a, b, loc);
            }

            ValueType resultType = promote(va.type, vb.type);
            return applyNumericOp(op, a, b, resultType, loc);
        }

        throw MathError(
            "Unsupported operation between " +
            typeToStr(va.type) + " and " +
            typeToStr(vb.type),
            loc
        );
    }

    Value applyStringOp(Op op, const Value& va, const Value& vb, SourceLocation loc) {
        if (op != Op::Add) {
            throw MathError("Invalid string operation (only '+' allowed)", loc);
        }

        return {ValueType::String,
            std::get<std::string>(va.data) + std::get<std::string>(vb.data)
        };
    }

    Value applyComparison(Op op, double a, double b, SourceLocation loc) {
        switch (op) {
            case Op::Less: return {ValueType::Bool, a < b};
            case Op::Greater: return {ValueType::Bool, a > b};
            case Op::LessEqual: return {ValueType::Bool, a <= b};
            case Op::GreaterEqual: return {ValueType::Bool, a >= b};

            default:
                throw MathError("Undefined comparison", loc);
        }
    }

    Value applyEquality(Op op, const Value& a, const Value& b, SourceLocation loc) {
        bool eq;

        if (isNumber(a.type) && isNumber(b.type)) {
            constexpr double EPS = 1e-9;
            eq = std::abs(a.asFloat() - b.asFloat()) < EPS;
        } else {
            eq = (a.data == b.data);
        }

        switch (op) {
            case Op::Equal: return {ValueType::Bool, eq};
            case Op::NotEqual: return {ValueType::Bool, !eq};

            default:
                throw MathError("Invalid equality operator", loc);
        }
    }
}
