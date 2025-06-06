#ifndef __PARSER_H__
#define __PARSER_H__

#include "../token/Token.h"
#include <vector>

struct ASTNode;

class Parser
{
public:
    //  Non-terminal Symbols Types
    enum VnType
    {
        program,                    //  <program>                   -> <declaration_list>
        declaration_list,           //  <declaration_list>          -> <declaration> <declaration_list> | ~
        declaration,                //  <declaration>               -> type id <dec_tail>
        dec_tail,                   //  <dec_tail>                  -> ; | [ num ] ; | ( <params> ) <compound_stmt>
        params,                     //  <params>                    -> <param> <params_tail> | ~ | 
        param,                      //  <param>                     -> type id
        params_tail,                //  <params_tail>               -> , <param> <params_tail> | ~
        statement,                  //  <statement>                 -> <expression_stmt> | <compound_stmt> | <selection_stmt> | <iteration_stmt> | <return_stmt>
        expression_stmt,            //  <expression_stmt>           -> <expression> ; | ;
        compound_stmt,              //  <compound_stmt>             -> { <compound_list> }
        compound_list,              //  <compound_list>             -> <compound> <compound_list> | ~
        compound,                   //  <compound>                  -> <var_dec> | <statement>
        var_dec,                    //  <var_dec>                   -> type id <var_dec_tail> ; | 
        var_dec_tail,               //  <var_dec_tail>              -> [ num ] | ~
        selection_stmt,             //  <selection_stmt>            -> if ( <expression> ) <statement> <else_part>
        else_part,                  //  <else_part>                 -> else <statement> | ~
        iteration_stmt,             //  <iteration_stmt>            -> while ( <expression> ) <statement> | for ( <expression> ; <expression> ; <expression> ) <statement>
        return_stmt,                //  <return_stmt>               -> return <return_tail> ;
        return_tail,                //  <return_tail>               -> <expression> | ~
        expression,                 //  <expression>                -> <additive_expression> <expression_tail>
        expression_tail,            //  <expression_tail>           -> <relop> <additive_expression> | ~
        relop,                      //  <relop>                     -> <= | < | > | >= | == | !=
        additive_expression,        //  <additive_expression>       -> <term> <additive_expression_tail>
        additive_expression_tail,   //  <additive_expression_tail>  -> <addop> <term> <additive_expression_tail> | ~
        addop,                      //  <addop>                     -> + | -
        term,                       //  <term>                      -> <factor> <term_tail>
        term_tail,                  //  <term_tail>                 -> <mulop> <factor> <term_tail> | ~
        mulop,                      //  <mulop>                     -> * | /
        factor,                     //  <factor>                    -> ( <expression> ) | id <id_tail> | num
        id_tail,                    //  <id_tail>                   -> ( <args> ) | <arr_idx> | <expression> | ~
        args,                       //  <args>                      -> <expression> <arg_tail> | ~
        arg_tail,                   //  <arg_tail>                  -> , <expression> <arg_tail> | ~
        arr_idx,                    //  <arr_idx>                   -> [ <expression> ] <arr_idx_tail>
        arr_idx_tail                //  <arr_idx_tail>              -> = <expression> | ~
    };

    // Parse Tree Node
    struct TreeNode
    {
        VnType vn_type;
        std::vector<Token> tokens;
        std::vector<TreeNode> childs;
    };

    Parser(const std::vector<Token> &tokens);

    TreeNode exec();

    bool hasError() const;

private:
    const std::vector<Token> &tokens;
    bool m_hasError;

    enum ItemType
    {
        Vn, //Non-terminal Symbols
        Vt  //Terminal Symbols
    };

    struct Item
    {
        int item = -1;  //no Vn or Vt in item
        ItemType type;
    };

    std::vector<Item> ASTable[34][35];

    void initTable();
};

#endif