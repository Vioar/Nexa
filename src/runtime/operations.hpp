// operations.hpp

#pragma once

#include "runtime/value.hpp"
#include "common/op.hpp"
#include "common/error.hpp"

namespace Operations {
    Value applyOp(Op op, const Value& a, const Value& b, SourceLocation loc);
    Value applyNumericOp(Op op, double a, double b, ValueType resultType, SourceLocation loc);
    Value applyStringOp(Op op, const Value& va, const Value& vb, SourceLocation loc);
    Value applyComparison(Op op, double a, double b, SourceLocation loc);
    Value applyEquality(Op op, const Value& a, const Value& b, SourceLocation loc);

    bool isNumber(ValueType t);
    bool isComparisonOp(Op op);
    bool isEqualityOp(Op op);

    ValueType promote(ValueType a, ValueType b);
};
