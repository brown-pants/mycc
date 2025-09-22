#include "Parser.h"
#include "../debug/Debug.h"
#include <stack>

Parser::Parser(const std::vector<Token> &tokens)
    : tokens(tokens), m_hasError(false)
{
    initTable();
}

Parser::TreeNode Parser::exec()
{
    TreeNode treeRoot = { program };
    std::stack<std::pair<TreeNode *, int>> nodeStack; // stack< pair< TreeNodePointer, ItemCount > >
    std::stack<Item> itemStack;
    int tokenIndex = 0;

    nodeStack.push({&treeRoot, 1});
    itemStack.push(Item{program, Vn});

    while (!itemStack.empty())
    {
        std::pair<TreeNode *, int> &topNode = nodeStack.top();
        const Item &topItem = itemStack.top();
        const Token &curToken = tokens[tokenIndex];

        if (topItem.type == Vt)
        {
            if (topItem.item == curToken.type())
            {
                topNode.first->tokens.push_back(curToken);
                if (-- topNode.second <= 0)
                {
                    nodeStack.pop();
                }
                tokenIndex ++;
                itemStack.pop();
            }
            else
            {
                Debug::ParseError(curToken, (Token::Type)topItem.item);
                m_hasError = true;
                break;
            }
        }
        else 
        {
            const std::vector<Item> &items = ASTable[topItem.item][curToken.type()];
            if (topItem.item != program)
            {
                topNode.first->childs.push_back(TreeNode{ (VnType)topItem.item });
                if (-- topNode.second <= 0)
                {
                    nodeStack.pop();
                }
                nodeStack.push(std::pair<TreeNode *, int>{ &*topNode.first->childs.rbegin(), items.size() });
                if (!items.empty() && items[0].item == Token::Nul)
                {// ε
                    nodeStack.pop();
                }
            }
            if (!items.empty())
            {
                itemStack.pop();
                for (auto iter = items.rbegin(); iter != items.rend(); iter ++)
                {
                    if (iter->type == Vt && iter->item == Token::Nul)
                    {// ε
                        continue;
                    }
                    itemStack.push(*iter);
                }
            }
            else
            {
                Debug::ParseError(curToken);
                m_hasError = true;
                break;
            }
        }
    }
    return treeRoot;
}

bool Parser::hasError() const
{
      return m_hasError;
}

void Parser::initTable()
{
    /* <program>:
          type     ->      { <declaration_list> }
          $        ->      { ~ } 
    */
    ASTable[program][Token::DT_int]                              = { Item{declaration_list, Vn} };       // type     ->      { <declaration_list> }
    ASTable[program][Token::DT_char]                             = { Item{declaration_list, Vn} };
    ASTable[program][Token::DT_float]                            = { Item{declaration_list, Vn} };
    ASTable[program][Token::Void]                                = { Item{declaration_list, Vn} };
    ASTable[program][Token::Eof]                                 = { Item{Token::Nul, Vt} };             // $        ->      { ~ } 
    
    /* <declaration_list>:
          type     ->      { <declaration> <declaration_list> }
          $        ->      { ~ } 
    */
    ASTable[declaration_list][Token::DT_int]                     = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // type     ->      { <declaration> <declaration_list> }
    ASTable[declaration_list][Token::DT_char]                    = { Item{declaration, Vn}, Item{declaration_list, Vn} };
    ASTable[declaration_list][Token::DT_float]                   = { Item{declaration, Vn}, Item{declaration_list, Vn} };
    ASTable[declaration_list][Token::Void]                       = { Item{declaration, Vn}, Item{declaration_list, Vn} };
    ASTable[declaration_list][Token::Eof]                        = { Item{Token::Nul, Vt} };                                     // $ -> { ~ } 

    /*<declaration>:
          type     ->      { type id <dec_tail> }
    */
    ASTable[declaration][Token::DT_int]                          = { Item{Token::DT_int, Vt}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };        // type     ->      { type id <dec_tail> }
    ASTable[declaration][Token::DT_char]                         = { Item{Token::DT_char, Vt}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };
    ASTable[declaration][Token::DT_float]                        = { Item{Token::DT_float, Vt}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };
    ASTable[declaration][Token::Void]                            = { Item{Token::Void, Vt}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };

    /*<dec_tail>:
          ;        ->      { ; }
          [        ->      { [ num ] ; }        num: Integer
          (        ->      { ( <params> ) <compound_stmt> }
    */
    ASTable[dec_tail][Token::Semicolon]                          = { Item{Token::Semicolon, Vt} };                                                                                        // ;        ->      { ; }
    ASTable[dec_tail][Token::OpenSquare]                         = { Item{Token::OpenSquare, Vt}, Item{Token::Integer, Vt}, Item{Token::CloseSquare, Vt}, Item{Token::Semicolon, Vt} };   // [        ->      { [ num ] ; }        num: Integer
    ASTable[dec_tail][Token::OpenParen]                          = { Item{Token::OpenParen, Vt}, Item{params, Vn}, Item{Token::CloseParen, Vt}, Item{compound_stmt, Vn} };                // (        ->      { ( <params> ) <compound_stmt> }

    /*<params>:
          type     ->      { <param> <params_tail> }
          )        ->      { ~ }
    */
    ASTable[params][Token::DT_int]                               = { Item{param, Vn}, Item{params_tail, Vn} };   // type     ->      { <param> <params_tail> }
    ASTable[params][Token::DT_char]                              = { Item{param, Vn}, Item{params_tail, Vn} };
    ASTable[params][Token::DT_float]                             = { Item{param, Vn}, Item{params_tail, Vn} };
    ASTable[params][Token::Void]                                 = { Item{param, Vn}, Item{params_tail, Vn} };
    ASTable[params][Token::CloseParen]                           = { Item{Token::Nul, Vt} };                     // )        ->      { ~ }

    /*<param>:
          type     ->      { type id }
    */
    ASTable[param][Token::DT_int]                                = { Item{Token::DT_int, Vt}, Item{Token::Identifier, Vt} };     // type     ->      { type id }
    ASTable[param][Token::DT_char]                               = { Item{Token::DT_char, Vt}, Item{Token::Identifier, Vt} };
    ASTable[param][Token::DT_float]                              = { Item{Token::DT_float, Vt}, Item{Token::Identifier, Vt} };
    ASTable[param][Token::Void]                                  = { Item{Token::Void, Vt}, Item{Token::Identifier, Vt} };

    /*<params_tail>:
          )        ->      { ~ }
          ,        ->      { , <param> <params_tail> }
    */
    ASTable[params_tail][Token::CloseParen]                      = { Item{Token::Nul, Vt} };                                             // )        ->      { ~ }
    ASTable[params_tail][Token::Comma]                           = { Item{Token::Comma, Vt}, Item{param, Vn}, Item{params_tail, Vn} };   // ,        ->      { , <param> <params_tail> }
    
    /*<statement>:
          id       ->      { <expression_stmt> }
          ;        ->      { <expression_stmt> }
          num      ->      { <expression_stmt> }
          (        ->      { <expression_stmt> }
          {        ->      { <compound_stmt> }
          if       ->      { <selection_stmt> }
          while    ->      { <iteration_stmt> }
          for      ->      { <iteration_stmt> }
          return   ->      { <return_stmt> }
    */
    ASTable[statement][Token::Identifier]                        = { Item{expression_stmt, Vn} };        // id       ->      { <expression_stmt> }
    ASTable[statement][Token::Semicolon]                         = { Item{expression_stmt, Vn} };        // ;        ->      { <expression_stmt> }
    ASTable[statement][Token::Integer]                           = { Item{expression_stmt, Vn} };        // num      ->      { <expression_stmt> }
    ASTable[statement][Token::Character]                         = { Item{expression_stmt, Vn} };        
    ASTable[statement][Token::Float]                             = { Item{expression_stmt, Vn} };        
    ASTable[statement][Token::String]                            = { Item{expression_stmt, Vn} };        
    ASTable[statement][Token::OpenParen]                         = { Item{expression_stmt, Vn} };        // (        ->      { <expression_stmt> }
    ASTable[statement][Token::OpenBrance]                        = { Item{compound_stmt, Vn} };          // {        ->      { <compound_stmt> }
    ASTable[statement][Token::If]                                = { Item{selection_stmt, Vn} };         // if       ->      { <selection_stmt> }
    ASTable[statement][Token::While]                             = { Item{iteration_stmt, Vn} };         // while    ->      { <iteration_stmt> }
    ASTable[statement][Token::For]                               = { Item{iteration_stmt, Vn} };         // for      ->      { <iteration_stmt> }
    ASTable[statement][Token::Return]                            = { Item{return_stmt, Vn} };            // return   ->      { <return_stmt> }

    /*<expression_stmt>:
          id       ->      { <expression> ; }
          ;        ->      { ; }
          num      ->      { <expression> ; }
          (        ->      { <expression> ; }
    */
    ASTable[expression_stmt][Token::Identifier]                  = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // id       ->      { <expression> ; }
    ASTable[expression_stmt][Token::Semicolon]                   = { Item{Token::Semicolon, Vt} };                               // ;        ->      { ; }
    ASTable[expression_stmt][Token::Integer]                     = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // num      ->      { <expression> ; }
    ASTable[expression_stmt][Token::Character]                   = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[expression_stmt][Token::Float]                       = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[expression_stmt][Token::String]                      = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[expression_stmt][Token::OpenParen]                   = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // (        ->      { <expression> ; }

    /*<compound_stmt>:
          {        ->      { { <compound_list> } }
    */
    ASTable[compound_stmt][Token::OpenBrance]                    = { Item{Token::OpenBrance, Vt}, Item{compound_list, Vn}, Item{Token::CloseBrance, Vt} };       // {        ->      { { <compound_list> } }

    /*<compound_list>:
          type     ->      { <compound> <compound_list> }
          id       ->      { <compound> <compound_list> }
          ;        ->      { <compound> <compound_list> }
          num      ->      { <compound> <compound_list> }
          (        ->      { <compound> <compound_list> }
          {        ->      { <compound> <compound_list> }
          }        ->      { ~ }
          if       ->      { <compound> <compound_list> }
          while    ->      { <compound> <compound_list> }
          for      ->      { <compound> <compound_list> }
          return   ->      { <compound> <compound_list> }
    */
    ASTable[compound_list][Token::DT_int]                        = { Item{compound, Vn}, Item{compound_list, Vn} };      // type     ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::DT_char]                       = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::DT_float]                      = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::Void]                          = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::Identifier]                    = { Item{compound, Vn}, Item{compound_list, Vn} };      // id       ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Semicolon]                     = { Item{compound, Vn}, Item{compound_list, Vn} };      // ;        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Integer]                       = { Item{compound, Vn}, Item{compound_list, Vn} };      // num      ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Character]                     = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::Float]                         = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::String]                        = { Item{compound, Vn}, Item{compound_list, Vn} };
    ASTable[compound_list][Token::OpenParen]                     = { Item{compound, Vn}, Item{compound_list, Vn} };      // (        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::OpenBrance]                    = { Item{compound, Vn}, Item{compound_list, Vn} };      // {        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::CloseBrance]                   = { Item{Token::Nul, Vt} };                             // }        ->      { ~ }
    ASTable[compound_list][Token::If]                            = { Item{compound, Vn}, Item{compound_list, Vn} };      // if       ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::While]                         = { Item{compound, Vn}, Item{compound_list, Vn} };      // while    ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::For]                           = { Item{compound, Vn}, Item{compound_list, Vn} };      // for      ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Return]                        = { Item{compound, Vn}, Item{compound_list, Vn} };      // return   ->      { <compound> <compound_list> }

    /*<compound>:
          type     ->      { <var_dec> }
          id       ->      { <statement> }
          ;        ->      { <statement> }
          num      ->      { <statement> }
          (        ->      { <statement> }
          {        ->      { <statement> }
          if       ->      { <statement> }
          while    ->      { <statement> }
          for      ->      { <statement> }
          return   ->      { <statement> }
    */
    ASTable[compound][Token::DT_int]                             = { Item{var_dec, Vn} };        // type     ->      { <var_dec> }
    ASTable[compound][Token::DT_char]                            = { Item{var_dec, Vn} };
    ASTable[compound][Token::DT_float]                           = { Item{var_dec, Vn} };
    ASTable[compound][Token::Void]                               = { Item{var_dec, Vn} };
    ASTable[compound][Token::Identifier]                         = { Item{statement, Vn} };      // id       ->      { <statement> }
    ASTable[compound][Token::Semicolon]                          = { Item{statement, Vn} };      // ;        ->      { <statement> }
    ASTable[compound][Token::Integer]                            = { Item{statement, Vn} };      // num      ->      { <statement> }
    ASTable[compound][Token::Character]                          = { Item{statement, Vn} };      
    ASTable[compound][Token::Float]                              = { Item{statement, Vn} };      
    ASTable[compound][Token::String]                             = { Item{statement, Vn} };      
    ASTable[compound][Token::OpenParen]                          = { Item{statement, Vn} };      // (        ->      { <statement> }
    ASTable[compound][Token::OpenBrance]                         = { Item{statement, Vn} };      // {        ->      { <statement> }
    ASTable[compound][Token::If]                                 = { Item{statement, Vn} };      // if       ->      { <statement> }
    ASTable[compound][Token::While]                              = { Item{statement, Vn} };      // while    ->      { <statement> }
    ASTable[compound][Token::For]                                = { Item{statement, Vn} };      // for      ->      { <statement> }
    ASTable[compound][Token::Return]                             = { Item{statement, Vn} };      // return   ->      { <statement> }

    /*<var_dec>:
          type     ->      { type id <var_dec_tail> ; }
    */
    ASTable[var_dec][Token::DT_int]                              = { Item{Token::DT_int, Vt}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };         // type     ->      { type id <var_dec_tail> ; }
    ASTable[var_dec][Token::DT_char]                             = { Item{Token::DT_char, Vt}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[var_dec][Token::DT_float]                            = { Item{Token::DT_float, Vt}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[var_dec][Token::Void]                                = { Item{Token::Void, Vt}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };

    /*<var_dec_tail>:
          ;        ->      { ~ }
          [        ->      { [ num ] }          num: Integer
    */
    ASTable[var_dec_tail][Token::Semicolon]                      = { Item{Token::Nul, Vt} };                                                                     // ;        ->      { ~ }
    ASTable[var_dec_tail][Token::OpenSquare]                     = { Item{Token::OpenSquare, Vt}, Item{Token::Integer, Vt}, Item{Token::CloseSquare, Vt} };      // [        ->      { [ num ] }          num: Integer
    
    /*<selection_stmt>:
          if       ->      { if ( <expression> ) <statement> <else_part> }
    */
    ASTable[selection_stmt][Token::If]                           = { Item{Token::If, Vt}, Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt}, Item{statement, Vn}, Item{else_part, Vn} };     // if       ->      { if ( <expression> ) <statement> <else_part> }

    /*<else_part>:
          type     ->      { ~ }
          id       ->      { ~ }
          ;        ->      { ~ }
          num      ->      { ~ }
          (        ->      { ~ }
          {        ->      { ~ }
          }        ->      { ~ }
          if       ->      { ~ }
          else     ->      { else <statement> }
          while    ->      { ~ }
          for      ->      { ~ }
          return   ->      { ~ }
    */
    ASTable[else_part][Token::DT_int]                            = { Item{Token::Nul, Vt} };                             // type     ->      { ~ }
    ASTable[else_part][Token::DT_char]                           = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::DT_float]                          = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::Void]                              = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::Identifier]                        = { Item{Token::Nul, Vt} };                             // id       ->      { ~ }
    ASTable[else_part][Token::Semicolon]                         = { Item{Token::Nul, Vt} };                             // ;        ->      { ~ }
    ASTable[else_part][Token::Integer]                           = { Item{Token::Nul, Vt} };                             // num      ->      { ~ }
    ASTable[else_part][Token::Character]                         = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::Float]                             = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::String]                            = { Item{Token::Nul, Vt} };
    ASTable[else_part][Token::OpenParen]                         = { Item{Token::Nul, Vt} };                             // (        ->      { ~ }
    ASTable[else_part][Token::OpenBrance]                        = { Item{Token::Nul, Vt} };                             // {        ->      { ~ }
    ASTable[else_part][Token::CloseBrance]                       = { Item{Token::Nul, Vt} };                             // }        ->      { ~ }
    ASTable[else_part][Token::If]                                = { Item{Token::Nul, Vt} };                             // if       ->      { ~ }
    ASTable[else_part][Token::Else]                              = { Item{Token::Else, Vt}, Item{statement, Vn} };       // else     ->      { else <statement> }
    ASTable[else_part][Token::While]                             = { Item{Token::Nul, Vt} };                             // while    ->      { ~ }
    ASTable[else_part][Token::For]                               = { Item{Token::Nul, Vt} };                             // for      ->      { ~ }
    ASTable[else_part][Token::Return]                            = { Item{Token::Nul, Vt} };                             // return   ->      { ~ }

    /*<iteration_stmt>:
          while    ->      { while ( <expression> ) <statement> }
          for      ->      { for ( <expression> ; <expression> ; <expression> ) <statement> }
    */
    ASTable[iteration_stmt][Token::While]                        = { Item{Token::While, Vt}, Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt}, Item{statement, Vn} };                                                                                                       // while    ->      { while ( <expression> ) <statement> }
    ASTable[iteration_stmt][Token::For]                          = { Item{Token::For, Vt}, Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::Semicolon, Vt}, Item{expression, Vn}, Item{Token::Semicolon, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt}, Item{statement, Vn} };     // for      ->      { for ( <expression> ; <expression> ; <expression> ) <statement> }
   
    /*<return_stmt>:
          return   ->      { return <return_tail> ; }
    */
    ASTable[return_stmt][Token::Return]                          = { Item{Token::Return, Vt}, Item{return_tail, Vn}, Item{Token::Semicolon, Vt} };       // return   ->      { return <return_tail> ; }

    /*<return_tail>:
          id       ->      { <expression> }
          ;        ->      { ~ }
          num      ->      { <expression> }
          (        ->      { <expression> }
    */
    ASTable[return_tail][Token::Identifier]                      = { Item{expression, Vn} };     // id       ->      { <expression> }
    ASTable[return_tail][Token::Semicolon]                       = { Item{Token::Nul, Vt} };     // ;        ->      { ~ }
    ASTable[return_tail][Token::Integer]                         = { Item{expression, Vn} };     // num      ->      { <expression> }
    ASTable[return_tail][Token::Character]                       = { Item{expression, Vn} };
    ASTable[return_tail][Token::Float]                           = { Item{expression, Vn} };
    ASTable[return_tail][Token::String]                          = { Item{expression, Vn} };
    ASTable[return_tail][Token::OpenParen]                       = { Item{expression, Vn} };     // (        ->      { <expression> }

    /*<expression>:
          id       ->      { <additive_expression> <expression_tail> }
          num      ->      { <additive_expression> <expression_tail> }
          (        ->      { <additive_expression> <expression_tail> }
    */
    ASTable[expression][Token::Identifier]                       = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };         // id       ->      { <additive_expression> <expression_tail> }
    ASTable[expression][Token::Integer]                          = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };         // num      ->      { <additive_expression> <expression_tail> }
    ASTable[expression][Token::Character]                        = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::Float]                            = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::String]                           = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::OpenParen]                        = { Item{additive_expression, Vn}, Item{expression_tail, Vn} };         // (        ->      { <additive_expression> <expression_tail> }

    /*<expression_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          <=       ->      { <relop> <additive_expression> }
          <        ->      { <relop> <additive_expression> }
          >        ->      { <relop> <additive_expression> }
          >=       ->      { <relop> <additive_expression> }
          ==       ->      { <relop> <additive_expression> }
          !=       ->      { <relop> <additive_expression> }
          +        ->      { ~ }
          -        ->      { ~ }
          *        ->      { ~ }
          /        ->      { ~ }
          %        ->      { ~ }
    */
    ASTable[expression_tail][Token::Semicolon]                   = { Item{Token::Nul, Vt} };                             // ;        ->      { ~ }        
    ASTable[expression_tail][Token::CloseSquare]                 = { Item{Token::Nul, Vt} };                             // ]        ->      { ~ }
    ASTable[expression_tail][Token::CloseParen]                  = { Item{Token::Nul, Vt} };                             // )        ->      { ~ }
    ASTable[expression_tail][Token::Comma]                       = { Item{Token::Nul, Vt} };                             // ,        ->      { ~ }
    ASTable[expression_tail][Token::Less_Eq]                     = { Item{relop, Vn}, Item{additive_expression, Vn} };   // <=       ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Less]                        = { Item{relop, Vn}, Item{additive_expression, Vn} };   // <        ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Greater]                     = { Item{relop, Vn}, Item{additive_expression, Vn} };   // >        ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Greater_Eq]                  = { Item{relop, Vn}, Item{additive_expression, Vn} };   // >=       ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Eq_Eq]                       = { Item{relop, Vn}, Item{additive_expression, Vn} };   // ==       ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Not_Eq]                      = { Item{relop, Vn}, Item{additive_expression, Vn} };   // !=       ->      { <relop> <additive_expression> }
    ASTable[expression_tail][Token::Plus]                        = { Item{Token::Nul, Vt} };                             // +        ->      { ~ }
    ASTable[expression_tail][Token::Minus]                       = { Item{Token::Nul, Vt} };                             // -        ->      { ~ }
    ASTable[expression_tail][Token::Mult]                        = { Item{Token::Nul, Vt} };                             // *        ->      { ~ }
    ASTable[expression_tail][Token::Div]                         = { Item{Token::Nul, Vt} };                             // /        ->      { ~ }
    ASTable[expression_tail][Token::Mod]                         = { Item{Token::Nul, Vt} };                             // %        ->      { ~ }

    /*<relop>:
          <=       ->      { <= }
          <        ->      { < }
          >        ->      { > }
          >=       ->      { >= }
          ==       ->      { == }
          !=       ->      { != }
    */
    ASTable[relop][Token::Less_Eq]                               = { Item{Token::Less_Eq, Vt} };         // <=       ->      { <= }
    ASTable[relop][Token::Less]                                  = { Item{Token::Less, Vt} };            // <        ->      { < }
    ASTable[relop][Token::Greater]                               = { Item{Token::Greater, Vt} };         // >        ->      { > }
    ASTable[relop][Token::Greater_Eq]                            = { Item{Token::Greater_Eq, Vt} };      // >=       ->      { >= }
    ASTable[relop][Token::Eq_Eq]                                 = { Item{Token::Eq_Eq, Vt} };           // ==       ->      { == }
    ASTable[relop][Token::Not_Eq]                                = { Item{Token::Not_Eq, Vt} };          // !=       ->      { != }

    /*<additive_expression>:
          id       ->      { <term> <additive_expression_tail> }
          num      ->      { <term> <additive_expression_tail> }
          (        ->      { <term> <additive_expression_tail> }
    */
    ASTable[additive_expression][Token::Identifier]              = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // id       ->      { <term> <additive_expression_tail> }
    ASTable[additive_expression][Token::Integer]                 = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // num      ->      { <term> <additive_expression_tail> }
    ASTable[additive_expression][Token::Character]               = { Item{term, Vn}, Item{additive_expression_tail, Vn} };
    ASTable[additive_expression][Token::Float]                   = { Item{term, Vn}, Item{additive_expression_tail, Vn} };
    ASTable[additive_expression][Token::String]                  = { Item{term, Vn}, Item{additive_expression_tail, Vn} };
    ASTable[additive_expression][Token::OpenParen]               = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // (        ->      { <term> <additive_expression_tail> }

    /*<additive_expression_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          +        ->      { <addop> <term> <additive_expression_tail> }
          -        ->      { <addop> <term> <additive_expression_tail> }
          *        ->      { ~ }
          /        ->      { ~ }
          %        ->      { ~ }
   */
    ASTable[additive_expression_tail][Token::Semicolon]          = { Item{Token::Nul, Vt} };                                                     // ;        ->      { ~ }
    ASTable[additive_expression_tail][Token::CloseSquare]        = { Item{Token::Nul, Vt} };                                                     // ]        ->      { ~ }
    ASTable[additive_expression_tail][Token::CloseParen]         = { Item{Token::Nul, Vt} };                                                     // )        ->      { ~ }
    ASTable[additive_expression_tail][Token::Comma]              = { Item{Token::Nul, Vt} };                                                     // ,        ->      { ~ }
    ASTable[additive_expression_tail][Token::Less_Eq]            = { Item{Token::Nul, Vt} };                                                     // <=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Less]               = { Item{Token::Nul, Vt} };                                                     // <        ->      { ~ }
    ASTable[additive_expression_tail][Token::Greater]            = { Item{Token::Nul, Vt} };                                                     // >        ->      { ~ }
    ASTable[additive_expression_tail][Token::Greater_Eq]         = { Item{Token::Nul, Vt} };                                                     // >=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Eq_Eq]              = { Item{Token::Nul, Vt} };                                                     // ==       ->      { ~ }
    ASTable[additive_expression_tail][Token::Not_Eq]             = { Item{Token::Nul, Vt} };                                                     // !=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Plus]               = { Item{addop, Vn}, Item{term, Vn}, Item{additive_expression_tail, Vn} };      // +        ->      { <addop> <term> <additive_expression_tail> }
    ASTable[additive_expression_tail][Token::Minus]              = { Item{addop, Vn}, Item{term, Vn}, Item{additive_expression_tail, Vn} };      // -        ->      { <addop> <term> <additive_expression_tail> }
    ASTable[additive_expression_tail][Token::Mult]               = { Item{Token::Nul, Vt} };                                                     // *        ->      { ~ }
    ASTable[additive_expression_tail][Token::Div]                = { Item{Token::Nul, Vt} };                                                     // /        ->      { ~ }
    ASTable[additive_expression_tail][Token::Mod]                = { Item{Token::Nul, Vt} };                                                     // %        ->      { ~ }

    /*<addop>:
          +        ->      { + }
          -        ->      { - }
    */
    ASTable[addop][Token::Plus]                                  = { Item{Token::Plus, Vt} };    // +        ->      { + }
    ASTable[addop][Token::Minus]                                 = { Item{Token::Minus, Vt} };   // -        ->      { - }

    /*<term>:
          id       ->      { <factor> <term_tail> }
          num      ->      { <factor> <term_tail> }
          (        ->      { <factor> <term_tail> }
    */
    ASTable[term][Token::Identifier]                             = { Item{factor, Vn}, Item{term_tail, Vn} };    // id       ->      { <factor> <term_tail> }
    ASTable[term][Token::Integer]                                = { Item{factor, Vn}, Item{term_tail, Vn} };    // num      ->      { <factor> <term_tail> }
    ASTable[term][Token::Character]                              = { Item{factor, Vn}, Item{term_tail, Vn} };
    ASTable[term][Token::Float]                                  = { Item{factor, Vn}, Item{term_tail, Vn} };
    ASTable[term][Token::String]                                 = { Item{factor, Vn}, Item{term_tail, Vn} };
    ASTable[term][Token::OpenParen]                              = { Item{factor, Vn}, Item{term_tail, Vn} };    // (        ->      { <factor> <term_tail> }

    /*<term_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          +        ->      { ~ }
          -        ->      { ~ }
          *        ->      { <mulop> <factor> <term_tail> }
          /        ->      { <mulop> <factor> <term_tail> }
          %        ->      { <mulop> <factor> <term_tail> }
    */
    ASTable[term_tail][Token::Semicolon]                         = { Item{Token::Nul, Vt} };                                     // ;        ->      { ~ }
    ASTable[term_tail][Token::CloseSquare]                       = { Item{Token::Nul, Vt} };                                     // ]        ->      { ~ }
    ASTable[term_tail][Token::CloseParen]                        = { Item{Token::Nul, Vt} };                                     // )        ->      { ~ }
    ASTable[term_tail][Token::Comma]                             = { Item{Token::Nul, Vt} };                                     // ,        ->      { ~ }
    ASTable[term_tail][Token::Less_Eq]                           = { Item{Token::Nul, Vt} };                                     // <=       ->      { ~ }
    ASTable[term_tail][Token::Less]                              = { Item{Token::Nul, Vt} };                                     // <        ->      { ~ }
    ASTable[term_tail][Token::Greater]                           = { Item{Token::Nul, Vt} };                                     // >        ->      { ~ }
    ASTable[term_tail][Token::Greater_Eq]                        = { Item{Token::Nul, Vt} };                                     // >=       ->      { ~ }
    ASTable[term_tail][Token::Eq_Eq]                             = { Item{Token::Nul, Vt} };                                     // ==       ->      { ~ }
    ASTable[term_tail][Token::Not_Eq]                            = { Item{Token::Nul, Vt} };                                     // !=       ->      { ~ }
    ASTable[term_tail][Token::Plus]                              = { Item{Token::Nul, Vt} };                                     // +        ->      { ~ }
    ASTable[term_tail][Token::Minus]                             = { Item{Token::Nul, Vt} };                                     // -        ->      { ~ }
    ASTable[term_tail][Token::Mult]                              = { Item{mulop, Vn}, Item{factor, Vn}, Item{term_tail, Vn} };   // *        ->      { <mulop> <factor> <term_tail> }
    ASTable[term_tail][Token::Div]                               = { Item{mulop, Vn}, Item{factor, Vn}, Item{term_tail, Vn} };   // /        ->      { <mulop> <factor> <term_tail> }
    ASTable[term_tail][Token::Mod]                               = { Item{mulop, Vn}, Item{factor, Vn}, Item{term_tail, Vn} };   // %        ->      { <mulop> <factor> <term_tail> }

    /*<mulop>:
          *        ->      { * }
          /        ->      { / }
          %        ->      { % }
    */
    ASTable[mulop][Token::Mult]                                  = { Item{Token::Mult, Vt} };    // *        ->      { * }
    ASTable[mulop][Token::Div]                                   = { Item{Token::Div, Vt} };     // /        ->      { / }
    ASTable[mulop][Token::Mod]                                   = { Item{Token::Mod, Vt} };     // %        ->      { % }

    /*<factor>:
          id       ->      { id <id_tail> }
          num      ->      { num }
          (        ->      { ( <expression> ) }
    */
    ASTable[factor][Token::Identifier]                           = { Item{Token::Identifier, Vt}, Item{id_tail, Vn} };                                   // id       ->      { id <id_tail> }
    ASTable[factor][Token::Integer]                              = { Item{Token::Integer, Vt} };                                                         // num      ->      { num }
    ASTable[factor][Token::Character]                            = { Item{Token::Character, Vt} };
    ASTable[factor][Token::Float]                                = { Item{Token::Float, Vt} };
    ASTable[factor][Token::String]                               = { Item{Token::String, Vt} };
    ASTable[factor][Token::OpenParen]                            = { Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt} };    // (        ->      { ( <expression> ) }   

    /*<id_tail>:
          ;        ->      { ~ }
          [        ->      { <arr_idx> }
          ]        ->      { ~ }
          (        ->      { ( <args> ) }
          )        ->      { ~ }
          ,        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          +        ->      { ~ }
          -        ->      { ~ }
          *        ->      { ~ }
          /        ->      { ~ }
          %        ->      { ~ }
          =        ->      { = <expression> }
    */
    ASTable[id_tail][Token::Semicolon]                           = { Item{Token::Nul, Vt} };                                                     // ;        ->      { ~ }
    ASTable[id_tail][Token::OpenSquare]                          = { Item{arr_idx, Vn} };                                                        // [        ->      { <arr_idx> }
    ASTable[id_tail][Token::CloseSquare]                         = { Item{Token::Nul, Vt} };                                                     // ]        ->      { ~ }
    ASTable[id_tail][Token::OpenParen]                           = { Item{Token::OpenParen, Vt}, Item{args, Vn}, Item{Token::CloseParen, Vt} };  // (        ->      { ( <args> ) }
    ASTable[id_tail][Token::CloseParen]                          = { Item{Token::Nul, Vt} };                                                     // )        ->      { ~ }
    ASTable[id_tail][Token::Comma]                               = { Item{Token::Nul, Vt} };                                                     // ,        ->      { ~ }
    ASTable[id_tail][Token::Less_Eq]                             = { Item{Token::Nul, Vt} };                                                     // <=       ->      { ~ }
    ASTable[id_tail][Token::Less]                                = { Item{Token::Nul, Vt} };                                                     // <        ->      { ~ }
    ASTable[id_tail][Token::Greater]                             = { Item{Token::Nul, Vt} };                                                     // >        ->      { ~ }
    ASTable[id_tail][Token::Greater_Eq]                          = { Item{Token::Nul, Vt} };                                                     // >=       ->      { ~ }
    ASTable[id_tail][Token::Eq_Eq]                               = { Item{Token::Nul, Vt} };                                                     // ==       ->      { ~ }
    ASTable[id_tail][Token::Not_Eq]                              = { Item{Token::Nul, Vt} };                                                     // !=       ->      { ~ }
    ASTable[id_tail][Token::Plus]                                = { Item{Token::Nul, Vt} };                                                     // +        ->      { ~ }
    ASTable[id_tail][Token::Minus]                               = { Item{Token::Nul, Vt} };                                                     // -        ->      { ~ }
    ASTable[id_tail][Token::Mult]                                = { Item{Token::Nul, Vt} };                                                     // *        ->      { ~ }
    ASTable[id_tail][Token::Div]                                 = { Item{Token::Nul, Vt} };                                                     // /        ->      { ~ }
    ASTable[id_tail][Token::Mod]                                 = { Item{Token::Nul, Vt} };                                                     // %        ->      { ~ }
    ASTable[id_tail][Token::Eq]                                  = { Item{Token::Eq, Vt}, Item{expression, Vn} };                                // =        ->      { = <expression> }

    /*<args>:
          id       ->      { <expression> <arg_tail> }
          num      ->      { <expression> <arg_tail> }
          (        ->      { <expression> <arg_tail> }
          )        ->      { ~ }
    */
    ASTable[args][Token::Identifier]                             = { Item{expression, Vn}, Item{arg_tail, Vn} };         // id       ->      { <expression> <arg_tail> }
    ASTable[args][Token::Integer]                                = { Item{expression, Vn}, Item{arg_tail, Vn} };         // num      ->      { <expression> <arg_tail> }
    ASTable[args][Token::Character]                              = { Item{expression, Vn}, Item{arg_tail, Vn} };
    ASTable[args][Token::Float]                                  = { Item{expression, Vn}, Item{arg_tail, Vn} };
    ASTable[args][Token::String]                                 = { Item{expression, Vn}, Item{arg_tail, Vn} };
    ASTable[args][Token::OpenParen]                              = { Item{expression, Vn}, Item{arg_tail, Vn} };         // (        ->      { <expression> <arg_tail> }
    ASTable[args][Token::CloseParen]                             = { Item{Token::Nul, Vt} };                             // )        ->      { ~ }

    /*<arg_tail>:
          )        ->      { ~ }
          ,        ->      { , <expression> <arg_tail> }
    */
    ASTable[arg_tail][Token::CloseParen]                         = { Item{Token::Nul, Vt} };                                                     // )        ->      { ~ }
    ASTable[arg_tail][Token::Comma]                              = { Item{Token::Comma, Vt}, Item{expression, Vn}, Item{arg_tail, Vn} };         // ,        ->      { , <expression> <arg_tail> }

    /*<arr_idx>:
          [        ->      { [ <expression> ] <arr_idx_tail> }
    */
    ASTable[arr_idx][Token::OpenSquare]                          = { Item{Token::OpenSquare, Vt}, Item{expression, Vn}, Item{Token::CloseSquare, Vt}, Item{arr_idx_tail, Vn} };  // [        ->      { [ <expression> ] <arr_idx_tail> }

    /*<arr_idx_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          +        ->      { ~ }
          -        ->      { ~ }
          *        ->      { ~ }
          /        ->      { ~ }
          %        ->      { ~ }
          =        ->      { = <expression> }
    */
    ASTable[arr_idx_tail][Token::Semicolon]                      = { Item{Token::Nul, Vt} };                             // ;        ->      { ~ }
    ASTable[arr_idx_tail][Token::CloseSquare]                    = { Item{Token::Nul, Vt} };                             // ]        ->      { ~ }
    ASTable[arr_idx_tail][Token::CloseParen]                     = { Item{Token::Nul, Vt} };                             // )        ->      { ~ }
    ASTable[arr_idx_tail][Token::Comma]                          = { Item{Token::Nul, Vt} };                             // ,        ->      { ~ }
    ASTable[arr_idx_tail][Token::Less_Eq]                        = { Item{Token::Nul, Vt} };                             // <=       ->      { ~ }
    ASTable[arr_idx_tail][Token::Less]                           = { Item{Token::Nul, Vt} };                             // <        ->      { ~ }
    ASTable[arr_idx_tail][Token::Greater]                        = { Item{Token::Nul, Vt} };                             // >        ->      { ~ }
    ASTable[arr_idx_tail][Token::Greater_Eq]                     = { Item{Token::Nul, Vt} };                             // >=       ->      { ~ }
    ASTable[arr_idx_tail][Token::Eq_Eq]                          = { Item{Token::Nul, Vt} };                             // ==       ->      { ~ }
    ASTable[arr_idx_tail][Token::Not_Eq]                         = { Item{Token::Nul, Vt} };                             // !=       ->      { ~ }
    ASTable[arr_idx_tail][Token::Plus]                           = { Item{Token::Nul, Vt} };                             // +        ->      { ~ }
    ASTable[arr_idx_tail][Token::Minus]                          = { Item{Token::Nul, Vt} };                             // -        ->      { ~ }
    ASTable[arr_idx_tail][Token::Mult]                           = { Item{Token::Nul, Vt} };                             // *        ->      { ~ }
    ASTable[arr_idx_tail][Token::Div]                            = { Item{Token::Nul, Vt} };                             // /        ->      { ~ }
    ASTable[arr_idx_tail][Token::Mod]                            = { Item{Token::Nul, Vt} };                             // %        ->      { ~ }
    ASTable[arr_idx_tail][Token::Eq]                             = { Item{Token::Eq, Vt}, Item{expression, Vn} };        // =        ->      { = <expression> }
}