// value.hpp

#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>

#include "common/types.hpp"
#include "common/error.hpp"

struct Value;

using List = std::shared_ptr<std::vector<Value>>;

using RawValue = std::variant<std::monostate, int, double, std::string, bool, List>;

struct Value {
    ValueType type;
    RawValue data;

    bool isTruthy() const;

    template<typename T>
    T asNum() const;

    double asFloat() const;
    int asInt() const;

    bool can_be_bool() const;
};

std::string toString(const Value&, SourceLocation);
int toInt(const Value&, SourceLocation);
double toFloat(const Value&, SourceLocation);
bool toBool(const Value&, SourceLocation);
List toList(const Value&, SourceLocation);

Value copy(const Value&, bool, SourceLocation);

std::string typeToStr(const ValueType&);
bool isNumber(const Value&);
bool isBool(const Value&);

template<typename T>
T Value::asNum() const {
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, double>,
        "asNum<T>: T must be int or double"
    );

    switch (type) {
        case ValueType::Int: {
            return static_cast<T>(std::get<int>(data));
        }
        case ValueType::Float: {
            return static_cast<T>(std::get<double>(data));
        }
        case ValueType::Bool: {
            return static_cast<T>(std::get<bool>(data) ? 1 : 0);
        }

        default: {
            throw std::runtime_error("Expected number, got " + typeToStr(type));
        }
    }
}
