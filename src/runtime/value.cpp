// value.cpp

#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "runtime/value.hpp"
#include "common/error.hpp"
#include "common/types.hpp"

std::string toString(const Value& v, SourceLocation loc) {
    switch (v.type) {
        case ValueType::Int:
            return std::to_string(std::get<int>(v.data));

        case ValueType::Float: {
            std::ostringstream oss;
            oss.precision(15);
            oss << std::get<double>(v.data);

            std::string s = oss.str();

            if (s.find('.') == std::string::npos &&
                s.find('e') == std::string::npos &&
                s.find('E') == std::string::npos)
            {
                s += ".0";
            }
            return s;
        }

        case ValueType::String:
            return std::get<std::string>(v.data);

        case ValueType::Bool:
            return std::get<bool>(v.data) ? "true" : "false";

        case ValueType::List: {
            const auto& list = std::get<List>(v.data);

            std::string out = "[";
            for (size_t i = 0; i < list->size(); i++) {
                out += toString((*list)[i], loc);
                if (i+1 < list->size()) {
                    out += ", ";
                }
            }

            out += "]";

            return out;
        }

        case ValueType::Undefined:
            return "";
    }

    throw ValueError("Unknown type", loc);
}

int toInt(const Value& v, SourceLocation loc) {
    switch (v.type) {
        case ValueType::Int:
        case ValueType::Float:
        case ValueType::Bool:
            return v.asInt();
        case ValueType::String: {
            const std::string& s = std::get<std::string>(v.data);

            try {
                size_t idx = 0;

                int result = std::stoi(s, &idx);

                if (idx != s.size()) {
                    for (char c : s) {
                        if (isalpha(c)) {
                            throw ValueError("Invalid integer: " + s, loc);
                        }
                    }
                    return result;
                }

                return result;

            } catch (const std::invalid_argument&) {
                throw ValueError("Unable to convert " + s + " to int", loc);
            } catch (const std::out_of_range&) {
                throw ValueError("Unable to convert " + s + " to int", loc);
            }
        }
        case ValueType::List:
        case ValueType::Undefined:
            throw ValueError("Cannot convert " + typeToStr(v.type) + " to int", loc);
    }

    throw ValueError("Unknown type: " + typeToStr(v.type), loc);
}

double toFloat(const Value& v, SourceLocation loc) {
    switch (v.type) {
        case ValueType::Int:
        case ValueType::Float:
        case ValueType::Bool:
            return v.asFloat();
        case ValueType::String: {
            const std::string& s = std::get<std::string>(v.data);

            try {
                size_t idx = 0;

                double result = std::stod(s, &idx);

                if (idx != s.size()) {
                    for (char c : s) {
                        if (isalpha(c)) {
                            throw ValueError("Invalid integer: " + s, loc);
                        }
                    }
                    return result;
                }

                return result;

            } catch (const std::invalid_argument&) {
                throw ValueError("Unable to convert " + s + " to float", loc);
            } catch (const std::out_of_range&) {
                throw ValueError("Unable to convert " + s + " to float", loc);
            }
        }
        case ValueType::List:
        case ValueType::Undefined:
            throw ValueError("Cannot convert " + typeToStr(v.type) + " to float", loc);
    }

    throw ValueError("Unknown type: " + typeToStr(v.type), loc);
}

bool toBool(const Value& v, SourceLocation loc) {
    switch (v.type) {
        case ValueType::Int:
            return std::get<int>(v.data) != 0;
        case ValueType::Float:
            return std::get<double>(v.data) != 0.0;
        case ValueType::Bool:
            return std::get<bool>(v.data) == true;
        case ValueType::String: {
            const std::string& s = std::get<std::string>(v.data);

            if (s == "true" || s == "false")
                return s == "true";
            else
                throw ValueError("Unable to convert " + s + " to bool", loc);
        }
        case ValueType::List:
            return !std::get<List>(v.data)->empty();
        case ValueType::Undefined:
            throw ValueError("Cannot convert " + typeToStr(v.type) + " to bool", loc);
    }

    throw ValueError("Unknown type: " + typeToStr(v.type), loc);
}

List toList(const Value& v, SourceLocation loc) {
    switch (v.type) {
        case ValueType::String:{
            List out = std::make_shared<std::vector<Value>>();

            auto& str = std::get<std::string>(v.data);

            for (auto& c : str) {
                std::string s = std::string(1, c);
                out->push_back(Value{ValueType::String, s});
            }

            return out;
        }

        default:
            throw ValueError("Cannot convert " + typeToStr(v.type) + " to list", loc);
    }
}

std::string typeToStr(const ValueType& v) {
    switch(v) {
        case ValueType::Int: return "int";
        case ValueType::Float: return "float";
        case ValueType::String: return "str";
        case ValueType::Bool: return "bool";
        case ValueType::List: return "list";
        case ValueType::Undefined: return "None";
    }

    throw std::runtime_error("Unknown type");
}

bool Value::isTruthy() const {
    switch (type) {
        case ValueType::Bool:
            return std::get<bool>(data);

        case ValueType::Int:
            return std::get<int>(data) > 0;

        case ValueType::Float:
            return std::get<double>(data) > 0.0;

        case ValueType::String:
            return !std::get<std::string>(data).empty();

        case ValueType::List:
            return !std::get<List>(data)->empty();

        case ValueType::Undefined:
            return false;
    }

    return false;
}

bool isNumber(const Value& v) {
    switch (v.type) {
        case ValueType::Bool:
        case ValueType::Float:
        case ValueType::Int:
            return true;
        case ValueType::String: {
            for (char c : std::get<std::string>(v.data)) {
                if (!isdigit(c)) {
                    return false;
                }
            }
            return true;
        }
        case ValueType::List: {
            for (auto& i : *std::get<List>(v.data)) {
                if (!isNumber(i)) {
                    return false;
                }
            }

            return true;
        }

        default:
            return false;
    }
}

bool isBool(const Value & v) {
    switch (v.type) {
        case ValueType::Bool:
        case ValueType::Int:
        case ValueType::Float:
            return true;
        case ValueType::String:
            return std::get<std::string>(v.data) == "true" || std::get<std::string>(v.data) == "false";
        case ValueType::List: {
            for (auto& i : *std::get<List>(v.data)) {
                if (!isBool(i)) {
                    return false;
                }
            }

            return true;
        }

        default:
            return false;
    }
}

Value copy(const Value& v, bool deep, SourceLocation loc) {
    switch (v.type) {
        case ValueType::Bool:
        case ValueType::Int:
        case ValueType::String:
        case ValueType::Float:
        case ValueType::Undefined:
            return v;

        case ValueType::List: {
            auto& src = *std::get<List>(v.data);

            List out = std::make_shared<std::vector<Value>>();

            out->reserve(src.size());

            if (deep) {
                for (auto& i : src) {
                    out->push_back(copy(i, deep, loc));
                }
            } else {
                for (auto& i : src) {
                    out->push_back(i);
                }
            }

            return {ValueType::List, out};
        }

        default:
            throw RuntimeError("Unknown type for copy: " + typeToStr(v.type), loc);
    }
}

double Value::asFloat() const {
    return Value::asNum<double>();
}

int Value::asInt() const {
    return Value::asNum<int>();
}
