#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <utility>

class Expression;
class Tokenizer;
std::unique_ptr<Expression> Item(Tokenizer* tokenizer);
std::unique_ptr<Expression> Expr(Tokenizer* tokenizer);

class Tokenizer {
   public:
    explicit Tokenizer(std::istream* in) : in_(in) {
        Consume();
    }

    enum TokenType : uint8_t { kNumber, kSymbol, kEnd };

    void Consume() {
        while (in_->peek() == 32) {
            in_->get();
        }
        if (in_->peek() == -1) {
            type_ = kEnd;
            return;
        }
        if (in_->peek() >= '0' && in_->peek() <= '9') {
            type_ = kNumber;
            (*in_) >> number_;
        } else {
            type_ = kSymbol;
            (*in_) >> symbol_;
        }
    }

    [[nodiscard]] TokenType GetType() const {
        return type_;
    }

    [[nodiscard]] int64_t GetNumber() const {
        return number_;
    }

    [[nodiscard]] char GetSymbol() const {
        return symbol_;
    }

   private:
    std::istream* in_;

    TokenType type_ = kEnd;
    int64_t number_ = 0;
    char symbol_ = '0';
};

class Expression {
   public:
    Expression() = default;
    Expression(const Expression&) = default;
    Expression(Expression&&) = default;
    Expression& operator=(const Expression&) = default;
    Expression& operator=(Expression&&) = default;
    virtual ~Expression() = default;
    [[nodiscard]] virtual int64_t Evaluate() const = 0;
};

class JustNumber : public Expression {
   public:
    explicit JustNumber(int64_t arg) : arg_(arg) {
    }

    [[nodiscard]] int64_t Evaluate() const override {
        return arg_;
    }

   private:
    int64_t arg_;
};

class UnaryExpression : public Expression {
   public:
    explicit UnaryExpression(std::unique_ptr<Expression> arg, char sign)
        : arg_(std::move(arg)), sign_(sign) {
    }

    [[nodiscard]] int64_t Evaluate() const override {
        if (sign_ == '-') {
            return -arg_->Evaluate();
        }
        return arg_->Evaluate();
    }

   private:
    std::unique_ptr<Expression> arg_;
    char sign_;
};

class BinaryExpression : public Expression {
   public:
    explicit BinaryExpression(
        std::unique_ptr<Expression> arg1, std::unique_ptr<Expression> arg2, char sign)
        : arg1_(std::move(arg1)), arg2_(std::move(arg2)), sign_(sign) {
    }

    [[nodiscard]] int64_t Evaluate() const override {
        if (sign_ == '+') {
            return arg1_->Evaluate() + arg2_->Evaluate();
        }
        if (sign_ == '-') {
            return arg1_->Evaluate() - arg2_->Evaluate();
        }
        if (sign_ == '*') {
            return arg1_->Evaluate() * arg2_->Evaluate();
        }
        return arg1_->Evaluate() / arg2_->Evaluate();
    }

   private:
    std::unique_ptr<Expression> arg1_;
    std::unique_ptr<Expression> arg2_;
    char sign_;
};

inline std::unique_ptr<Expression> Mult(Tokenizer* tokenizer) {
    if (tokenizer->GetType() == Tokenizer::kNumber) {
        const int64_t a = tokenizer->GetNumber();
        tokenizer->Consume();
        return std::make_unique<JustNumber>(a);
    }
    const char a = tokenizer->GetSymbol();
    tokenizer->Consume();
    if (a == '-' || a == '+') {
        auto b = Mult(tokenizer);
        return std::make_unique<UnaryExpression>(std::move(b), a);
    }
    auto tmp = Expr(tokenizer);
    tokenizer->Consume();
    return tmp;
}

inline std::unique_ptr<Expression> Item(Tokenizer* tokenizer) {
    auto a = Mult(tokenizer);
    char curtoken = '0';
    while (tokenizer->GetType() == Tokenizer::kSymbol &&
           (tokenizer->GetSymbol() == '*' || tokenizer->GetSymbol() == '/')) {
        curtoken = tokenizer->GetSymbol();
        tokenizer->Consume();
        auto b = Mult(tokenizer);
        a = std::make_unique<BinaryExpression>(std::move(a), std::move(b), curtoken);
    }
    return a;
}

inline std::unique_ptr<Expression> Expr(Tokenizer* tokenizer) {
    auto a = Item(tokenizer);
    char curtoken = '0';
    while (tokenizer->GetType() == Tokenizer::kSymbol &&
           (tokenizer->GetSymbol() == '+' || tokenizer->GetSymbol() == '-')) {
        curtoken = tokenizer->GetSymbol();
        tokenizer->Consume();
        auto b = Item(tokenizer);
        a = std::make_unique<BinaryExpression>(std::move(a), std::move(b), curtoken);
    }
    return a;
}

inline std::unique_ptr<Expression> ParseExpression(Tokenizer* tokenizer) {
    return Expr(tokenizer);
}