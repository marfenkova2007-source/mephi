#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <exception>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <functional>
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;

class IObject {
    /*
     * Базовый класс для всех объектов, с которыми работает калькулятор
     */
public:
    virtual ~IObject() = default;

    /*
     * Преобразование объекта в строку. Удобно, когда нужно вывести объект в описание исключения или в логи.
     * В проверяющем коде задания ожидается, что объекты, представляющие числовые литералы, будут приводиться к строке
     * так, как это делает функция std::to_string (не стоит изобретать свой велосипед в этом месте)
     */
    virtual string Stringify() const = 0;
};

using ObjPtr = shared_ptr<const IObject>;
using Context = unordered_map<std::string, ObjPtr>;

class IExpression : public IObject {
    /*
     * Базовый класс для вычислимых выражений, таких как
     * Числовые константы, функции (в т.ч. лямбда-функции), имена, операции применения функции к аргументу
     */
public:
    /*
     * Вычисляет значение выражения. Результатом вычисления может быть как константа, так и какой-то более сложный объект
     * Например, функция после применения её в какому-то аргументу может вернуть другую функцию.
     *
     * Подразумевается, что функции в данной задаче каррированные, т.е. могут быть применены только к одному аргументу
     * за один раз. В таком случае функция с несколькими аргументами превращается в функцию от одного аргумента, которая возвращает
     * другую функцию, имеющую на один аргумент меньше. См. https://ru.wikipedia.org/wiki/Каррирование
     */
    virtual ObjPtr Evaluate(const Context& ctx) const = 0;
};

using ExprPtr = shared_ptr<const IExpression>;

class Exception : public std::exception {
private:
    string error;
public:
    Exception(const string& words): error(words) {}
    const char* what() const noexcept override {
        return error.c_str();
    }
};
//А зачем определённые названия то...

class ParsingException : public Exception {
public:
    ParsingException(const string& words) : Exception("Cannot parse string: " + words) {}
};

class ParenthesisMismatchException : public ParsingException {
public:
    ParenthesisMismatchException(const string& words = "Parenthesis mismatch") : ParsingException(words) {}
};

class EmptyParsingResultException : public ParsingException {
public:
    EmptyParsingResultException(const string& words = "Empty parsing result") : ParsingException(words) {}
};

class BadLambdaArgumentException : public ParsingException {
public:
    BadLambdaArgumentException(const string& words) : ParsingException("Expected argument names for the lambda, got " + words) {}
};

class LambdaWithoutArgumentsException : public ParsingException {
public:
    LambdaWithoutArgumentsException(const string& words = "A lambda requires at least one argument") : ParsingException(words) {}
};

class EvaluationException : public Exception {
public:
    EvaluationException(const string& words) : Exception(words) {}
};

class UnresolvedReferenceException : public EvaluationException {
public:
    UnresolvedReferenceException(const string& words) : EvaluationException("Unknown name " + words) {}
};

class ApplyException : public EvaluationException {
public:
    ApplyException(const string& words) : EvaluationException(words) {}
};


class Work_with_n : public IExpression {
private:
    double n;
public:
    Work_with_n(double nn) : n(nn) {}
    string Stringify() const override {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.6f", n);
        string itog = buffer;
        return itog;
    }
    ObjPtr Evaluate(const Context& ctx)const override {
        return make_shared<Work_with_n>(n);
    }
    double GetValue()const{
        return n;
    }
};

class Name : public IExpression {
private:
    string name;
public:
    Name(const string& ima): name(ima) {}
    string Stringify()const override{
        return name;
    }
    ObjPtr Evaluate(const Context& ctx)const override{
        auto it = ctx.find(name);
        if (it==ctx.end()) {
            throw UnresolvedReferenceException(name);
        }
        return it->second;
    }
};

class BuiltInFunction : public IExpression {
private:
    string name_;
    function<double(double, double)> binary_op_;
    function<double(double)> unary_op_;
    bool is_binary_;
    
public:
    BuiltInFunction(const string& name, function<double(double, double)> op)
        : name_(name), binary_op_(op), is_binary_(true) {}
    
    BuiltInFunction(const string& name, function<double(double)> op)
        : name_(name), unary_op_(op), is_binary_(false) {}
    
    string Stringify() const override {
        return name_;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        return make_shared<BuiltInFunction>(*this);
    }
    
    bool IsBinary() const { return is_binary_; }
    
    double ApplyBinary(double a, double b) const {
        if (!is_binary_) throw ApplyException(name_ + " is not a binary operator");
        return binary_op_(a, b);
    }
    
    double ApplyUnary(double a) const {
        if (is_binary_) throw ApplyException(name_ + " is not a unary operator");
        return unary_op_(a);
    }
};

class LambdaFunction : public IExpression {
private:
    vector<string> arg_names;
    shared_ptr<const IExpression> body;
    Context capture_ctx;
    
public:
    LambdaFunction(const vector<string>& args, shared_ptr<const IExpression> b, const Context& ctx) : arg_names(args), body(b) {
        for (const auto& pair : ctx) capture_ctx[pair.first] = pair.second;
    }
    string Stringify() const override {
        string result = "λ";
        for (size_t i = 0; i < arg_names.size(); ++i) {
            result += arg_names[i];
            if (i != arg_names.size() - 1) {
                result += ".λ";
            }
        }
        
        string body_str = body->Stringify();
        result += "." + body_str;
        return result;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        return make_shared<LambdaFunction>(arg_names, body, capture_ctx);
    }
    
    ObjPtr Apply(ObjPtr arg) const {
        if (arg_names.empty()){
            throw ApplyException("земля в элюминаторе");
        }
        if (arg_names.size() == 1) {
            Context new_ctx = capture_ctx;
            new_ctx[arg_names[0]] = arg;
            return body->Evaluate(new_ctx);
        }
        else {
            vector<string> remaining_args(arg_names.begin() + 1, arg_names.end());
            Context new_ctx = capture_ctx;
            new_ctx[arg_names[0]] = arg;
            return make_shared<LambdaFunction>(remaining_args, body, new_ctx);
        }
    }
};

class Operations : public IExpression {
private:
    shared_ptr<const IExpression> x;
    shared_ptr<const IExpression> y;  
    string op;
public:
    Operations(shared_ptr<const IExpression> a, shared_ptr<const IExpression> b, const string& doo): 
        x(a),
        y(b),
        op(doo) {}
    
    string Stringify() const override {
        string left_str = x->Stringify();
        string right_str = y->Stringify();
        if (left_str[0] != '(' || left_str.back() != ')') {
            left_str = "(" + left_str + ")";
        }
        if (right_str[0] != '(' || right_str.back() != ')') {
            right_str = "(" + right_str + ")";
        }
        return "(" + op + left_str + right_str + ")";
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        ObjPtr xx = x->Evaluate(ctx);
        ObjPtr yy = y->Evaluate(ctx);
        auto xn = dynamic_pointer_cast<const Work_with_n>(xx);
        auto yn = dynamic_pointer_cast<const Work_with_n>(yy);
        if (xn==nullptr or yn==nullptr) {
            throw ApplyException("Поломочка из-за того что переменная хранит фигню");
        }
        double eins = xn->GetValue();
        double zwei = yn->GetValue();
        double itog;
        if (op=="+") {
            itog = eins + zwei;
        } 
        else if (op == "-") {
            itog = eins - zwei;
        }
        else if (op == "*") {
            itog = eins * zwei;
        }
        else if (op == "/") {
            if (zwei==0){
                throw ApplyException("пытаемся делить на 0");
            }
            else{
                itog = eins / zwei;
            }
        }
        else{
            throw ApplyException("Чёрт знает как мы тут оказались");
        }
        return make_shared<Work_with_n>(itog);
    }
};

class Functions : public IExpression {
private:
    shared_ptr<const IExpression> function;
    shared_ptr<const IExpression> argument;
public:
    Functions(shared_ptr<const IExpression> f, shared_ptr<const IExpression> x) : 
        function(f), argument(x) {}
    
    string Stringify() const override {
        string func_str = function->Stringify();
        string arg_str = argument->Stringify();
        
        auto builtin = dynamic_pointer_cast<const BuiltInFunction>(function);
        if (builtin && !builtin->IsBinary()) {
            return "(" + func_str + " " + arg_str + ")";
        }
        return func_str + " " + arg_str;
    }
    
    ObjPtr Evaluate(const Context& ctx) const override {
        auto func_app = dynamic_pointer_cast<const Functions>(function);
        if (func_app) {
            ObjPtr inner_result = func_app->Evaluate(ctx);
            auto inner_lambda = dynamic_pointer_cast<const LambdaFunction>(inner_result);
            if (!inner_lambda) {
                ObjPtr arg_obj = argument->Evaluate(ctx);
                throw ApplyException("Cannot apply function '" + func_app->Stringify() + "' to argument " + arg_obj->Stringify());
            }
            ObjPtr arg_obj = argument->Evaluate(ctx);
            return inner_lambda->Apply(arg_obj);
        }
        
        ObjPtr func_obj = function->Evaluate(ctx);
        ObjPtr arg_obj = argument->Evaluate(ctx);
        
        auto lambda = dynamic_pointer_cast<const LambdaFunction>(func_obj);
        if (lambda) return lambda->Apply(arg_obj);
        
        auto builtin = dynamic_pointer_cast<const BuiltInFunction>(func_obj);
        if (builtin && !builtin->IsBinary()) {
            auto num = dynamic_pointer_cast<const Work_with_n>(arg_obj);
            if (!num){
                throw ApplyException("sin/cos");
            }
            return make_shared<Work_with_n>(builtin->ApplyUnary(num->GetValue()));
        }
        
        throw ApplyException("Cannot apply function '" + func_obj->Stringify() + "' to argument " + arg_obj->Stringify());
    }
};

class IStatement : public IObject {
    /*
     * Базовый класс для инструкций, подаваемых на вход калькулятору
     * Одна входная строка в итоге должна быть преобразована в одну инструкцию
     * Инструкции бывают двух типов -- вычисление значения выражения и присваивание
     * Инструкция присваивания выглядит как `[name] = [expression]`, где
     *  `[name]` -- какое-то имя из букв и цифр, начинающееся с буквы;
     *  `[expression]` -- выражение, значение которого нужно запомнить под именем `[name]`.
     * Если в исходной строке не было знака равенства (который означает операцию присваивания),
     * то вся строка воспринимается как инструкция вычисления выражения
     */
public:
    virtual ObjPtr Execute(Context& ctx) const = 0;
};
using StatementPtr = shared_ptr<const IStatement>;

class Expression : public IStatement {
private:
    shared_ptr<const IExpression> write;
public:
    Expression(shared_ptr<const IExpression> words) : write(words) {}
    string Stringify() const override {
        return write->Stringify();
    }
    ObjPtr Execute(Context& ctx) const override {
        return write->Evaluate(ctx);
    }
};

class Prisvaivanie: public IStatement {
private:
    string name;
    shared_ptr<const IExpression> write;
public:
    Prisvaivanie(const string& ima, shared_ptr<const IExpression> words) : 
        name(ima), write(words) {}
    string Stringify() const override {
        return name + " = " + write->Stringify();
    }
    ObjPtr Execute(Context& ctx) const override {
        ObjPtr itog = write->Evaluate(ctx);
        ctx[name] = itog;
        return itog;
    }
};

struct Token {
    /*
     * Токен -- минимальная неделимая единица разбора исходной строки
     */
    enum class Type {
        Literal = 0,  // литерал -- буквальное значение (числовая константа в данной задаче типа `1`, `25.48` и т.п.)
        Name,  // Какое-то имя из букв и цифр, начинающееся с буквы, либо знак встроенной бинарной операции (+-*/). Имя ссылается на какое-то значение (константа, функция)
        LeftParenthesis,  // Открывающая круглая скобка `(`
        RightParenthesis,  // Закрывающая круглая скобка скобка `)`
        LambdaSign,  // Условный символ "лямбда" `\` (в данном случае это левый слэш как в языке Haskell)
        Arrow,  // Стрелка `->`
        EqualsSign,  // Знак равенства `=`
    };

    Type type_;
    string value_;
};

class StatementParser {
public:
    StatementPtr ParseString(const string& s, const Context& ctx) {
        vector<Token> mas = Tokenize(s, ctx);
        if (mas.empty()) {
            throw EmptyParsingResultException();
        }

        for (size_t i = 0; i < mas.size(); ++i) {
            if (mas[i].type_ == Token::Type::EqualsSign) {
                if (i == 0 || mas[i-1].type_ != Token::Type::Name) {
                    throw ParsingException("Cannot assign in expression");
                }
                string imya = mas[i-1].value_;
                size_t pos = i + 1;
                shared_ptr<const IExpression> expr = ParseExpression(mas, pos, ctx);
                if (pos != mas.size()) {
                    throw ParsingException("-----");
                }
                return make_shared<Prisvaivanie>(imya, expr);
            }
        }
        
        size_t pos = 0;
        shared_ptr<const IExpression> expr = ParseExpression(mas, pos, ctx);
        if (pos != mas.size()) {
            if (pos < mas.size() && mas[pos].type_ == Token::Type::RightParenthesis) {
                throw ParenthesisMismatchException();
            }
            throw ParsingException("-----");
        }
        return make_shared<Expression>(expr);
    }

    /*
     * Разобрать исходную строку на токены, которые можно в дальнейшем преобразовать в инструкцию для калькулятора
     * Эта функция будет полезна в `ParseString`
     */
    std::vector<Token> Tokenize(const string& s, const Context& ctx);

    shared_ptr<const IExpression> ParseExpression(const vector<Token>& mas, size_t& pos, const Context& ctx);
    
private:
    shared_ptr<const IExpression> ParseAddSub(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseMulDiv(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParsePrimary(const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseApplications(shared_ptr<const IExpression> func, const vector<Token>& tokens, size_t& pos, const Context& ctx);
    shared_ptr<const IExpression> ParseLambda(const vector<Token>& tokens, size_t& pos, const Context& ctx);
};
shared_ptr<const IExpression> StatementParser::ParseExpression(const vector<Token>& mas, size_t& pos, const Context& ctx) {
    return ParseAddSub(mas, pos, ctx);
}

shared_ptr<const IExpression> StatementParser::ParseAddSub(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    shared_ptr<const IExpression> left = ParseMulDiv(tokens, pos, ctx);
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        string op = tokens[pos].value_;
        if (op == "+" or op == "-") {
            ++pos;
            shared_ptr<const IExpression> right = ParseMulDiv(tokens, pos, ctx);
            left = make_shared<Operations>(left, right, op);
        }
        else{
            break;
        }
    }
    return left;
}

shared_ptr<const IExpression> StatementParser::ParseMulDiv(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    shared_ptr<const IExpression> left = ParsePrimary(tokens, pos, ctx);
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        string op = tokens[pos].value_;
        if (op == "*" or op == "/") {
            ++pos;
            shared_ptr<const IExpression> right = ParsePrimary(tokens, pos, ctx);
            left = make_shared<Operations>(left, right, op);
        }else{
            break;
        }
    }
    return left;
}

shared_ptr<const IExpression> StatementParser::ParsePrimary(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    if (pos >= tokens.size()){
        throw EmptyParsingResultException();
    }
    
    if (tokens[pos].type_ == Token::Type::LambdaSign) {
        return ParseLambda(tokens, pos, ctx);
    }
    
    if (tokens[pos].type_ == Token::Type::LeftParenthesis) {
        ++pos;
        if (pos < tokens.size() && tokens[pos].type_ == Token::Type::RightParenthesis) {
            throw EmptyParsingResultException();
        }
        shared_ptr<const IExpression> expr = ParseAddSub(tokens, pos, ctx);
        if (pos >= tokens.size() || tokens[pos].type_ != Token::Type::RightParenthesis) {
            throw ParenthesisMismatchException();
        }
        ++pos;
        return ParseApplications(expr, tokens, pos, ctx);
    }
    
    shared_ptr<const IExpression> expr;
    if (tokens[pos].type_ == Token::Type::Literal) {
        expr = make_shared<Work_with_n>(stod(tokens[pos].value_));
    }
    else if (tokens[pos].type_ == Token::Type::Name) {
        expr = make_shared<Name>(tokens[pos].value_);
    }
    else {
        throw ParsingException("Unexpected token: " + tokens[pos].value_);
    }
    ++pos;
    
    return ParseApplications(expr, tokens, pos, ctx);
}

shared_ptr<const IExpression> StatementParser::ParseApplications(shared_ptr<const IExpression> func, const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    while (pos < tokens.size()) {
        if (tokens[pos].type_ == Token::Type::LambdaSign or
            tokens[pos].type_ == Token::Type::LeftParenthesis or
            tokens[pos].type_ == Token::Type::Literal or
            tokens[pos].type_ == Token::Type::Name) {
            
            if (tokens[pos].type_ == Token::Type::Name) {
                string name = tokens[pos].value_;
                if (name=="+" or name=="-" or name=="*" or name=="/"){
                    break;
                }
            }
            
            shared_ptr<const IExpression> arg;
            if (tokens[pos].type_ == Token::Type::LambdaSign) {
                arg = ParseLambda(tokens, pos, ctx);
            }
            else if (tokens[pos].type_ == Token::Type::LeftParenthesis) {
                ++pos;
                arg = ParseAddSub(tokens, pos, ctx);
                if (pos >= tokens.size() or tokens[pos].type_ != Token::Type::RightParenthesis) {
                    throw ParenthesisMismatchException();
                }
                ++pos;
            }
            else if (tokens[pos].type_ == Token::Type::Literal) {
                arg = make_shared<Work_with_n>(stod(tokens[pos].value_));
                ++pos;
            }
            else if (tokens[pos].type_ == Token::Type::Name) {
                arg = make_shared<Name>(tokens[pos].value_);
                ++pos;
            }
            
            func = make_shared<Functions>(func, arg);
        }
        else {
            break;
        }
    }
    return func;
}

shared_ptr<const IExpression> StatementParser::ParseLambda(const vector<Token>& tokens, size_t& pos, const Context& ctx) {
    if (tokens[pos].type_ != Token::Type::LambdaSign) {
        throw ParsingException("Expected lambda");
    }
    ++pos;
    
    vector<string> args;
    while (pos < tokens.size() && tokens[pos].type_ == Token::Type::Name) {
        args.push_back(tokens[pos].value_);
        ++pos;
    }
    
    if (args.empty()) {
        throw LambdaWithoutArgumentsException();
    }
    
    if (pos >= tokens.size() || tokens[pos].type_ != Token::Type::Arrow) {
        if (pos < tokens.size()) {
            throw BadLambdaArgumentException(tokens[pos].value_);
        } else {
            throw BadLambdaArgumentException("ничего");
        }
    }
    ++pos;
    
    shared_ptr<const IExpression> body = ParseAddSub(tokens, pos, ctx);
    return make_shared<LambdaFunction>(args, body, ctx);
}

vector<Token> StatementParser::Tokenize(const string& s, const Context& ctx) {
    vector<Token> mas;
    size_t i = 0;
    string chisla = "0123456789.";
    string alfavit = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_";
    
    while (i < s.size()) {
        if (s[i] == ' ') {
            ++i;
            continue;
        }
        
        // Числа
        if (isdigit(s[i]) || (s[i] == '.' && i + 1 < s.size() && isdigit(s[i+1]))) {
            string n;
            while (i < s.size() && (isdigit(s[i]) || s[i] == '.')) {
                n += s[i];
                ++i;
            }
            mas.push_back({Token::Type::Literal, n});
            continue;
        }
        
        // Имена
        if (isalpha(s[i]) || s[i] == '_') {
            string imya;
            while (i < s.size() && (isalnum(s[i]) || s[i] == '_')) {
                imya += s[i];
                ++i;
            }
            mas.push_back({Token::Type::Name, imya});
            continue;
        }
        
        // Специальные символы
        if (s[i] == '(') {
            mas.push_back({Token::Type::LeftParenthesis, "("});
        }
        else if (s[i] == ')') {
            mas.push_back({Token::Type::RightParenthesis, ")"});
        }
        else if (s[i] == '\\') {
            mas.push_back({Token::Type::LambdaSign, "\\"});
        }
        else if (s[i] == '=') {
            mas.push_back({Token::Type::EqualsSign, "="});
        }
        else if (s[i] == '-') {
            if (i + 1 < s.size() and s[i + 1] == '>') {
                mas.push_back({Token::Type::Arrow, "->"});
                ++i;
            }
            else {
                mas.push_back({Token::Type::Name, "-"});
            }
        }
        else if (s[i] == '+') {
            mas.push_back({Token::Type::Name, "+"});
        }
        else if (s[i] == '*') {
            mas.push_back({Token::Type::Name, "*"});
        }
        else if (s[i] == '/') {
            mas.push_back({Token::Type::Name, "/"});
        }
        else {
            throw ParsingException(string(1, s[i]));
        }
        ++i;
    }
    
    return mas;
}

class Calculator {
public:
    Calculator() {
        // Добавляем базовые предопределенные функции в калькулятор.
        // Реализовать классы нужно самостоятельно, тригонометрические функции брать из стандартной библиотеки
        ctx_["+"] = make_shared<BuiltInFunction>("+", [](double a, double b) { return a + b; });
        ctx_["-"] = make_shared<BuiltInFunction>("-", [](double a, double b) { return a - b; });
        ctx_["*"] = make_shared<BuiltInFunction>("*", [](double a, double b) { return a * b; });
        ctx_["/"] = make_shared<BuiltInFunction>("/", [](double a, double b) { return a / b; });
        ctx_["sin"] = make_shared<BuiltInFunction>("sin", [](double a) { return sin(a); });
        ctx_["cos"] = make_shared<BuiltInFunction>("cos", [](double a) { return cos(a); });
    }

    string ProcessString(const string& stringExpression) {
        StatementPtr statement = ParseLine(stringExpression);
        return ExecuteStatement(statement)->Stringify();
    }

    ObjPtr ExecuteStatement(StatementPtr statement) {
        /*
         * Выполнить данную инструкцию.
         * В результате выполнения инструкции присваивания будут происходить изменения в контексте
         * В результате выполнения инструкции вычисления выражения контекст не должен меняться
         */
        return statement->Execute(ctx_);
    }

    ObjPtr EvaluateExpression(shared_ptr<const IExpression> expression) const {
        return expression->Evaluate(ctx_);
    }

    StatementPtr ParseLine(const string& line) {
        return parser_.ParseString(line, ctx_);
    }

    shared_ptr<const IExpression> ParseExpressionLine(const string line) {
        /*
         * Разобрать строку в выражение.
         * Если строка задает инструкцию присваивания, а не инструкцию вычисления выражения, то бросить исключение
         */
        vector<Token> mas = parser_.Tokenize(line, ctx_);
        if (mas.empty()) {
            throw EmptyParsingResultException();
        }
        for (const Token& el : mas) {
            if (el.type_ == Token::Type::EqualsSign) {
                throw ParsingException("Want expression but it is assignment statement");
            }
        }
        size_t pos = 0;
        return parser_.ParseExpression(mas, pos, ctx_);
    }
    
private:
    Context ctx_;
    StatementParser parser_;
};