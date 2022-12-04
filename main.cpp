#include <iostream>
#include <vector>
#include <stack>
#include <cmath>

//defining types values
#define OPERAND 0
#define OPERATOR 1
#define FUNCTION 2
#define BRACKET 3

//operations tokens
enum tokens{_null, _sum, _sub, _mul, _div, _pow, _sin, _cos, _open, _close};

///value sepcifications
struct value{
    std::string val;
    int type;
    tokens token = _null;
    int precedence = -1;

    //create new value with type and make sure that if subtraction symbol is not a negative number then its operator
    value(int _type, std::string _val): val(_val), type(_type){if(val == "-") type = OPERATOR; create_token(); get_precedence();}

    //set token of value
    //define shorten return
    #define _return_token(t) {this->token = t; return t;}
    tokens create_token() {
        if(type == OPERAND) _return_token(_null);
        if(type == OPERATOR || type == BRACKET) {
            switch(val[0]) {
            case '+':
                _return_token(_sum);
            case '-':
                _return_token(_sub);
            case '*':
                _return_token(_mul);
            case '/':
                _return_token(_div);
            case '^':
                _return_token(_pow);
            case '(':
                _return_token(_open);
            case ')':
                _return_token(_close);
            }
        }
        if(val == "sin" || val == "SIN") _return_token(_sin);
        if(val == "cos" || val == "COS") _return_token(_cos);
        _return_token(_null);
    }

    //return operation precedence
    //define shorten return
    #define _set(v) precedence = v; return v;
    int get_precedence() {
        switch(token) {
        case _open:
            _set(0);
        case _sum: case _sub: case _close:
            _set(1);
        case _mul: case _div:
            _set(2);
        case _sin: case _cos:
            _set(3);
        case _pow:
            _set(4);
        default: return -1;
        }
    }
};

///return separated expression
//define shorten to clear specific type of vals table and to push it into the output
#define _clear(type) output.push_back(new value(type, vals[type])); vals[type] = "";
//define shorten to push value into the output and to write next value
#define _push(type) if(last >= 0 && (last != type || last == BRACKET)){_clear(last)} last = type; vals[type] += ch;
std::vector<value*> separate(std::string& read) {
    std::vector<value*> output;
    //values for = {operands, operators, functions, brackets}
    std::string vals[4] = {"", "", "", ""};
    // last added type
    int last = -1;
    //check all posible types of values
    for(char ch:read) {
        if(ch==' ') continue;
        if((ch>='0'&&ch<='9')||ch=='.'||ch=='-'){
            if(ch=='-') {_clear(OPERAND)}
            _push(OPERAND);
        } else if(ch==')'||ch=='('){
            _push(BRACKET);
        } else if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')){
            _push(FUNCTION);
        } else {
            _push(OPERATOR);
        }
    }
    //push last type of bracket or operand
    output.push_back((vals[OPERAND]=="")? new value(BRACKET, vals[BRACKET]): new value(OPERAND, vals[OPERAND]));
    //add + operator between two adjacent numbers
    for(int i=1; i<output.size(); i++) if((output[i-1]->type==output[i]->type) && output[i]->type==OPERAND) output.insert(output.begin()+i, new value(OPERATOR, "+"));
    return output;
}

///converts expression to rpn
#define pop_stack output.push_back(operations.top()); operations.pop();
std::vector<value*> rpn(std::vector<value*>& expression) {
    std::vector<value*> output;
    std::stack<value*> operations;
    for(value* val : expression) {
        switch(val->token) {
        //push numbers to output
        case _null:
            if(val->type == OPERAND) output.push_back(val);
            else {throw ("EXCEPTION WHILE CONVERTING! unknown value: " + val->val);}
            break;
        //handle close bracket
        case _close:
            while(!operations.empty() && operations.top()->token != _open) {pop_stack;}
            operations.pop();
            break;
        //handle open bracket
        case _open:
            operations.push(val);
            break;
        //handle functions and operators
        default:
            while(!operations.empty() && operations.top()->precedence >= val->precedence) {pop_stack;}
            operations.push(val);
        }
    }
    //empty stack
    while(!operations.empty()) {pop_stack;}
    return output;
}

///calculate final result
//define shorten 1 number from stack
#define get_number if(result.size()<1)throw("EXCEPTION WHILE ACCESSING STACK! not enough values"); x=result.top(); result.pop();
//define shorten 2 numbers from stack
#define get_numbers if(result.size()<2)throw("EXCEPTION WHILE ACCESSING STACK! not enough values"); x=result.top(); result.pop(); y=result.top(); result.pop();
double calculate(std::vector<value*>& expression) {
    std::stack<double> result;
    //handle each operation or function and push to result
    for(value* val : expression) {
        double x, y;
        switch(val->token) {
        case _null:
            result.push(std::__cxx11::stoi(val->val));
            break;
        case _sum:
            get_numbers;
            result.push(x+y);
            break;
        case _sub:
            get_numbers;
            result.push(x-y);
            break;
        case _mul:
            get_numbers;
            result.push(x*y);
            break;
        case _div:
            get_numbers;
            result.push(x/y);
            break;
        case _pow:
            get_numbers;
            result.push(std::pow(x, y));
            break;
        case _sin:
            get_number;
            result.push(std::sin(x));
            break;
        case _cos:
            get_number;
            result.push(std::cos(x));
            break;
        default :
            throw("EXCEPTION WHILE CALCULATING! unknown operator or function" + val->val);
        }
    }
    return result.top();
}

int main() {
    //read input
    std::string read;
    std::getline(std::cin, read);
    std::vector<value*> expression=separate(read);
    try{
        expression = rpn(expression);
        //write result
        std::cout << "= " << calculate(expression);
        //look for exceptions
    } catch(std::string& exc){std::cout << exc;};
    return 0;
}
