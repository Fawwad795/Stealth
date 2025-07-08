#ifndef REVERSE_POLISH_NOTATION_H
#define REVERSE_POLISH_NOTATION_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include "../queue/Queue.h"
#include "../stack/Stack.h"
#include "../Token/token.h"
#include "../Table/typedefs.h"
#include "../Token/token_str.h"
#include "../Token/relational.h"
#include "../Token/result_set.h"
#include "../error_code/error_code.h"

using namespace std;

//implementing Reverse Polish Notation evaluator for SQL WHERE clause conditions
class RPN {
public:
    //constructing empty RPN evaluator
    RPN() {;}

    //constructing RPN evaluator with postfix token queue
    RPN(const Queue<Token*> &postfix) : _postfix(postfix) {;}

    //evaluating postfix expression and returning matching record indices
    vectorlong operator()(vector<mmap_sl>& record_indicies, map_sl& field_indicies) throw(Error_Code) {
        assert(_postfix.begin() && "Cannot evaluate an empty Queue");
        Error_Code error_code;
        Stack<Token*> rpn_stack;
        int token_str_count = 0;  //tracking consecutive string tokens for syntax validation

        //processing each token in postfix expression
        for(Queue<Token*>::Iterator it = _postfix.begin(); it != _postfix.end(); ++it) {
            Token* first_pop;
            Token* second_pop;
            ResultSet result;

            //checking for invalid syntax: two string tokens without an operator
            if(token_str_count == 2 && (*it)->get_type() != OPERATOR) {
                token_str_count = 0;
                error_code._error_token = (*rpn_stack.begin())->get_val();
                if(!field_indicies.contains((*it)->get_val())) {
                    error_code._code = SYNTAX_ERR_AT_NEAR;
                    error_code._modify_to_postgre = true;
                }
                else
                    error_code._code = EXPECT_A_RELATIONAL;
                throw error_code;
            }
            else if(token_str_count == 2 && (*it)->get_type() == OPERATOR)
                token_str_count = 0;

            //processing tokens based on their type
            switch((*it)->get_type()) {
                case TOKEN_STR:  //handling string tokens (field names or values)
                    token_str_count++;
                    rpn_stack.push(*it);
                    break;

                case OPERATOR:  //handling operators (relational or logical)
                    //checking for sufficient operands
                    if(rpn_stack.size() < 2) {
                        //handling missing operand errors
                        if(rpn_stack.empty()) {
                            error_code._error_token = (*it)->get_val();
                            error_code._code = MISSING_ARGUMENTS;
                        }
                        else {
                            if(static_cast<Operator*>(*it)->get_operator_type() == RELATIONAL) {
                                //handling missing operand for relational operator
                                error_code._error_token = (*it)->get_val();
                                first_pop = rpn_stack.pop();
                                if(field_indicies.contains(first_pop->get_val())) {
                                    error_code._code = RELATIONAL_MISSING_RIGHT_ARG;
                                }
                                else {
                                    error_code._code = RELATIONAL_MISSING_LEFT_ARG;
                                }
                            }
                            else {
                                //handling missing operand for logical operator
                                error_code._error_token = (*it)->get_val();
                                error_code._code = LOGICAL_MISSING_AN_ARGUMENT;
                            }
                        }
                        throw error_code;
                    }

                    //popping operands for evaluation
                    first_pop = rpn_stack.pop();
                    second_pop = rpn_stack.pop();

                    //validating operand types match
                    if(first_pop->get_type() != second_pop->get_type()) {
                        string token_str = "";
                        if(first_pop->get_type() == TOKEN_STR)
                            token_str = first_pop->get_val();
                        else
                            token_str = second_pop->get_val();

                        error_code._error_token = (*it)->get_val();
                        if(static_cast<Operator*>(*it)->get_operator_type() == RELATIONAL) {
                            //handling mismatched operands for relational operator
                            error_code._error_token = (*it)->get_val();
                            if(field_indicies.contains(token_str)) {
                                error_code._code = RELATIONAL_MISSING_RIGHT_ARG;
                            }
                            else {
                                error_code._code = RELATIONAL_MISSING_LEFT_ARG;
                            }
                        }
                        else {
                            error_code._code = INVALID_USAGE_OF_OP;
                        }
                        throw error_code;
                    }

                    //checking for correct operator type with string operands
                    if(first_pop->get_type() == TOKEN_STR && static_cast<Operator*>(*it)->get_operator_type() == LOGICAL) {
                        error_code._error_token = (*it)->get_val();
                        error_code._code = EXPECT_RELATIONAL;
                        throw error_code;
                    }

                    //evaluating operator and pushing result
                    rpn_stack.push(new ResultSet((*it)->evaluate(second_pop, first_pop, record_indicies, field_indicies)));
                    break;

                default:
                    break;
            }
        }

        //validating final stack state
        if(rpn_stack.size() == 2) {
            error_code._code = EXPECT_A_RELATIONAL;
            throw error_code;
        }
        assert(rpn_stack.size() == 1 && "Result Stack cannot contain more than one element");

        //validating result type
        Stack<Token*>::Iterator it = rpn_stack.begin();
        if((*it)->get_type() != RESULT_SET) {
            error_code._code = EXPECT_A_RELATIONAL;
            throw error_code;
        }

        //returning final result as vector of matching record indices
        return static_cast<ResultSet*>(rpn_stack.pop())->get_val_list();
    }

    //outputting RPN expression
    friend ostream& operator << (ostream& outs, const RPN& print_me) {
        outs << print_me._postfix;
        return outs;
    }

    //setting postfix expression for evaluation
    void set_input(const Queue<Token*> &postfix) {
        _postfix = postfix;
    }

private:
    Queue<Token*> _postfix;    //queue containing postfix expression tokens
};

#endif //REVERSE_POLISH_NOTATION_H