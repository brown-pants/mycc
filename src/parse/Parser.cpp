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
                if (!items.empty() && items[0].type == Vt && items[0].item == Token::Nul)
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
          int      ->      { <declaration_list> }
          char     ->      { <declaration_list> }
          float    ->      { <declaration_list> }
          void     ->      { <declaration_list> }
          extern   ->      { <declaration_list> }
          $        ->      { ~ } 
    */
    ASTable[program][Token::DT_int]                              = { Item{declaration_list, Vn} };       // int      ->      { <declaration_list> }
    ASTable[program][Token::DT_char]                             = { Item{declaration_list, Vn} };       // char     ->      { <declaration_list> }
    ASTable[program][Token::DT_float]                            = { Item{declaration_list, Vn} };       // float    ->      { <declaration_list> }
    ASTable[program][Token::Void]                                = { Item{declaration_list, Vn} };       // void     ->      { <declaration_list> }
    ASTable[program][Token::Extern]                              = { Item{declaration_list, Vn} };       // extern   ->      { <declaration_list> }
    ASTable[program][Token::Eof]                                 = { Item{Token::Nul, Vt} };             // $        ->      { ~ } 
    
    /* <declaration_list>:
          int      ->      { <declaration> <declaration_list> }
          char     ->      { <declaration> <declaration_list> }
          float    ->      { <declaration> <declaration_list> }
          void     ->      { <declaration> <declaration_list> }
          extern   ->      { <declaration> <declaration_list> }
          $        ->      { ~ } 
    */
    ASTable[declaration_list][Token::DT_int]                     = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // int      ->      { <declaration> <declaration_list> }
    ASTable[declaration_list][Token::DT_char]                    = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // char     ->      { <declaration> <declaration_list> }
    ASTable[declaration_list][Token::DT_float]                   = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // float    ->      { <declaration> <declaration_list> }
    ASTable[declaration_list][Token::Void]                       = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // void     ->      { <declaration> <declaration_list> }
    ASTable[declaration_list][Token::Extern]                     = { Item{declaration, Vn}, Item{declaration_list, Vn} };        // extern   ->      { <declaration_list> }
    ASTable[declaration_list][Token::Eof]                        = { Item{Token::Nul, Vt} };                                     // $ -> { ~ } 

    /*<declaration>:
          int      ->      { <data_type> <is_pointer> id <dec_tail> }
          char     ->      { <data_type> <is_pointer> id <dec_tail> }
          float    ->      { <data_type> <is_pointer> id <dec_tail> }
          void     ->      { <data_type> <is_pointer> id <dec_tail> }
          extern   ->      { extern type <is_pointer> id <dec_tail> }
    */
    ASTable[declaration][Token::DT_int]                          = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };                                // int      ->      { <data_type> <is_pointer> id <dec_tail> }
    ASTable[declaration][Token::DT_char]                         = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };                                // char     ->      { <data_type> <is_pointer> id <dec_tail> }
    ASTable[declaration][Token::DT_float]                        = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };                                // float    ->      { <data_type> <is_pointer> id <dec_tail> }
    ASTable[declaration][Token::Void]                            = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };                                // void     ->      { <data_type> <is_pointer> id <dec_tail> }
    ASTable[declaration][Token::Extern]                          = { Item{Token::Extern, Vt}, Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{dec_tail, Vn} };       // extern   ->      { extern type <is_pointer> id <dec_tail> }
    
    /*<data_type>:
          int      ->      { int }
          char     ->      { char }
          float    ->      { float }
          void     ->      { void }
    */
    ASTable[data_type][Token::DT_int]                            = { Item{Token::DT_int, Vt} };       // int      ->      { int }
    ASTable[data_type][Token::DT_char]                           = { Item{Token::DT_char, Vt} };      // char     ->      { char }
    ASTable[data_type][Token::DT_float]                          = { Item{Token::DT_float, Vt} };     // float    ->      { float }
    ASTable[data_type][Token::Void]                              = { Item{Token::Void, Vt} };         // void     ->      { void }

    /*<is_pointer>:
          id       ->      { ~ }
          *        ->      { * }
    */
    ASTable[is_pointer][Token::Identifier]                       = { Item{Token::Nul, Vt} };    // id       ->      { ~ }
    ASTable[is_pointer][Token::Mult]                             = { Item{Token::Mult, Vt} };   // *        ->      { * }

    /*<dec_tail>:
          ;        ->      { ; }
          =        ->      { = <expression> ; }
          [        ->      { [ <expression> ] ; }
          (        ->      { ( <params> ) <func_tail> }
    */
    ASTable[dec_tail][Token::Semicolon]                          = { Item{Token::Semicolon, Vt} };                                                                                        // ;        ->      { ; }
    ASTable[dec_tail][Token::Eq]                                 = { Item{Token::Eq, Vt}, Item{expression, Vn}, Item{Token::Semicolon, Vt} };                                             // =        ->      { = <expression> ; }
    ASTable[dec_tail][Token::OpenSquare]                         = { Item{Token::OpenSquare, Vt}, Item{expression, Vn}, Item{Token::CloseSquare, Vt}, Item{Token::Semicolon, Vt} };       // [        ->      { [ <expression> ] ; }
    ASTable[dec_tail][Token::OpenParen]                          = { Item{Token::OpenParen, Vt}, Item{params, Vn}, Item{Token::CloseParen, Vt}, Item{func_tail, Vn} };                    // (        ->      { ( <params> ) <func_tail> }

    /*<func_tail>:
          ;        ->      { ; }
          {        ->      { <compound_stmt> }
    */
    ASTable[func_tail][Token::Semicolon]                         = { Item{Token::Semicolon, Vt} };    // ;        ->      { ; }
    ASTable[func_tail][Token::OpenBrance]                        = { Item{compound_stmt, Vn} };       // {        ->      { <compound_stmt> }

    /*<params>:
          int      ->      { <param> <params_tail> }
          char     ->      { <param> <params_tail> }
          float    ->      { <param> <params_tail> }
          void     ->      { <param> <params_tail> }
          )        ->      { ~ }
    */
    ASTable[params][Token::DT_int]                               = { Item{param, Vn}, Item{params_tail, Vn} };   // int      ->      { <param> <params_tail> }
    ASTable[params][Token::DT_char]                              = { Item{param, Vn}, Item{params_tail, Vn} };   // char     ->      { <param> <params_tail> }
    ASTable[params][Token::DT_float]                             = { Item{param, Vn}, Item{params_tail, Vn} };   // float    ->      { <param> <params_tail> }
    ASTable[params][Token::Void]                                 = { Item{param, Vn}, Item{params_tail, Vn} };   // void     ->      { <param> <params_tail> }
    ASTable[params][Token::CloseParen]                           = { Item{Token::Nul, Vt} };                     // )        ->      { ~ }

    /*<param>:
          int      ->      { <data_type> <is_pointer> id }
          char     ->      { <data_type> <is_pointer> id }
          float    ->      { <data_type> <is_pointer> id }
          void     ->      { <data_type> <is_pointer> id }
    */
    ASTable[param][Token::DT_int]                                = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt} };  // int      ->      { <data_type> <is_pointer> id }
    ASTable[param][Token::DT_char]                               = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt} };  // char     ->      { <data_type> <is_pointer> id }
    ASTable[param][Token::DT_float]                              = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt} };  // float    ->      { <data_type> <is_pointer> id }
    ASTable[param][Token::Void]                                  = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt} };  // void     ->      { <data_type> <is_pointer> id }

    /*<params_tail>:
          )        ->      { ~ }
          ,        ->      { , <param> <params_tail> }
    */
    ASTable[params_tail][Token::CloseParen]                      = { Item{Token::Nul, Vt} };                                             // )        ->      { ~ }
    ASTable[params_tail][Token::Comma]                           = { Item{Token::Comma, Vt}, Item{param, Vn}, Item{params_tail, Vn} };   // ,        ->      { , <param> <params_tail> }
    
    /*<statement>:
          !        ->      { <expression_stmt> }
          -        ->      { <expression_stmt> }
          *        ->      { <expression_stmt> }
          &        ->      { <expression_stmt> }
          id       ->      { <expression_stmt> }
          ;        ->      { <expression_stmt> }
          num      ->      { <expression_stmt> }
          (        ->      { <expression_stmt> }
          {        ->      { <compound_stmt> }
          if       ->      { <selection_stmt> }
          while    ->      { <iteration_stmt> }
          for      ->      { <iteration_stmt> }
          return   ->      { <return_stmt> }
          break    ->      { <jump_stmt> }
          continue ->      { <jump_stmt> }
    */
    ASTable[statement][Token::Not]                               = { Item{expression_stmt, Vn} };        // !        ->      { <expression_stmt> }
    ASTable[statement][Token::Minus]                             = { Item{expression_stmt, Vn} };        // -        ->      { <expression_stmt> }
    ASTable[statement][Token::Mult]                              = { Item{expression_stmt, Vn} };        // *        ->      { <expression_stmt> }
    ASTable[statement][Token::Ampersand]                         = { Item{expression_stmt, Vn} };        // &        ->      { <expression_stmt> }
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
    ASTable[statement][Token::Break]                             = { Item{jump_stmt, Vn} };              // break    ->      { <jump_stmt> }
    ASTable[statement][Token::Continue]                          = { Item{jump_stmt, Vn} };              // continue ->      { <jump_stmt> }

    /*<expression_stmt>:
          ;        ->      { ; }
          !        ->      { <expression> ; }
          -        ->      { <expression> ; }
          *        ->      { <expression> ; }
          &        ->      { <expression> ; }
          id       ->      { <expression> ; }
          (        ->      { <expression> ; }
          num      ->      { <expression> ; }
    */
    ASTable[expression_stmt][Token::Semicolon]                   = { Item{Token::Semicolon, Vt} };                               // ;        ->      { ; }
    ASTable[expression_stmt][Token::Not]                         = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // !        ->      { <expression> ; }
    ASTable[expression_stmt][Token::Minus]                       = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // -        ->      { <expression> ; }
    ASTable[expression_stmt][Token::Mult]                        = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // *        ->      { <expression> ; }
    ASTable[expression_stmt][Token::Ampersand]                   = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // &        ->      { <expression> ; }
    ASTable[expression_stmt][Token::Identifier]                  = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // id       ->      { <expression> ; }
    ASTable[expression_stmt][Token::OpenParen]                   = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // (        ->      { <expression> ; }
    ASTable[expression_stmt][Token::Integer]                     = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };         // num      ->      { <expression> ; }
    ASTable[expression_stmt][Token::Character]                   = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[expression_stmt][Token::Float]                       = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };
    ASTable[expression_stmt][Token::String]                      = { Item{expression, Vn}, Item{Token::Semicolon, Vt} };

    /*<compound_stmt>:
          {        ->      { { <compound_list> } }
    */
    ASTable[compound_stmt][Token::OpenBrance]                    = { Item{Token::OpenBrance, Vt}, Item{compound_list, Vn}, Item{Token::CloseBrance, Vt} };       // {        ->      { { <compound_list> } }

    /*<compound_list>:
          !        ->      { <compound> <compound_list> }
          -        ->      { <compound> <compound_list> }
          *        ->      { <compound> <compound_list> }
          &        ->      { <compound> <compound_list> }
          int      ->      { <compound> <compound_list> }
          char     ->      { <compound> <compound_list> }
          float    ->      { <compound> <compound_list> }
          void     ->      { <compound> <compound_list> }
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
          break    ->      { <compound> <compound_list> }
          continue ->      { <compound> <compound_list> }
    */
    ASTable[compound_list][Token::Not]                           = { Item{compound, Vn}, Item{compound_list, Vn} };      // !        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Minus]                         = { Item{compound, Vn}, Item{compound_list, Vn} };      // -        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Mult]                          = { Item{compound, Vn}, Item{compound_list, Vn} };      // *        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Ampersand]                     = { Item{compound, Vn}, Item{compound_list, Vn} };      // &        ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::DT_int]                        = { Item{compound, Vn}, Item{compound_list, Vn} };      // int      ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::DT_char]                       = { Item{compound, Vn}, Item{compound_list, Vn} };      // char     ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::DT_float]                      = { Item{compound, Vn}, Item{compound_list, Vn} };      // float    ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Void]                          = { Item{compound, Vn}, Item{compound_list, Vn} };      // void     ->      { <compound> <compound_list> }
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
    ASTable[compound_list][Token::Break]                         = { Item{compound, Vn}, Item{compound_list, Vn} };      // break    ->      { <compound> <compound_list> }
    ASTable[compound_list][Token::Continue]                      = { Item{compound, Vn}, Item{compound_list, Vn} };      // continue ->      { <compound> <compound_list> }

    /*<compound>:
          int      ->      { <var_dec> }
          char     ->      { <var_dec> }
          float    ->      { <var_dec> }
          void     ->      { <var_dec> }
          !        ->      { <statement> }
          -        ->      { <statement> }
          *        ->      { <statement> }
          &        ->      { <statement> }
          id       ->      { <statement> }
          ;        ->      { <statement> }
          num      ->      { <statement> }
          (        ->      { <statement> }
          {        ->      { <statement> }
          if       ->      { <statement> }
          while    ->      { <statement> }
          for      ->      { <statement> }
          return   ->      { <statement> }
          break    ->      { <statement> }
          continue ->      { <statement> }
    */
    ASTable[compound][Token::DT_int]                             = { Item{var_dec, Vn} };        // int      ->      { <var_dec> }
    ASTable[compound][Token::DT_char]                            = { Item{var_dec, Vn} };        // char     ->      { <var_dec> }
    ASTable[compound][Token::DT_float]                           = { Item{var_dec, Vn} };        // float    ->      { <var_dec> }
    ASTable[compound][Token::Void]                               = { Item{var_dec, Vn} };        // void     ->      { <var_dec> }
    ASTable[compound][Token::Not]                                = { Item{statement, Vn} };      // !        ->      { <statement> }
    ASTable[compound][Token::Minus]                              = { Item{statement, Vn} };      // -        ->      { <statement> }
    ASTable[compound][Token::Mult]                               = { Item{statement, Vn} };      // *        ->      { <statement> }
    ASTable[compound][Token::Ampersand]                          = { Item{statement, Vn} };      // &        ->      { <statement> }
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
    ASTable[compound][Token::Break]                              = { Item{statement, Vn} };      // break    ->      { <statement> }
    ASTable[compound][Token::Continue]                           = { Item{statement, Vn} };      // continue ->      { <statement> }

    /*<var_dec>:
          int      ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
          char     ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
          float    ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
          void     ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
    */
    ASTable[var_dec][Token::DT_int]                              = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };         // int      ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
    ASTable[var_dec][Token::DT_char]                             = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };         // char     ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
    ASTable[var_dec][Token::DT_float]                            = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };         // float    ->      { <data_type> <is_pointer> id <var_dec_tail> ; }
    ASTable[var_dec][Token::Void]                                = { Item{data_type, Vn}, Item{is_pointer, Vn}, Item{Token::Identifier, Vt}, Item{var_dec_tail, Vn}, Item{Token::Semicolon, Vt} };         // void     ->      { <data_type> <is_pointer> id <var_dec_tail> ; }

    /*<var_dec_tail>:
          ;        ->      { ~ }
          [        ->      { [ <expression> ] }
          =        ->      { = <expression> }
    */
    ASTable[var_dec_tail][Token::Semicolon]                      = { Item{Token::Nul, Vt} };                                                                     // ;        ->      { ~ }
    ASTable[var_dec_tail][Token::OpenSquare]                     = { Item{Token::OpenSquare, Vt}, Item{expression, Vn}, Item{Token::CloseSquare, Vt} };          // [        ->      { [ <expression> ] }
    ASTable[var_dec_tail][Token::Eq]                             = { Item{Token::Eq, Vt}, Item{expression, Vn} };                                                // =        ->      { = <expression> }
    
    /*<selection_stmt>:
          if       ->      { if ( <expression> ) <statement> <else_part> }
    */
    ASTable[selection_stmt][Token::If]                           = { Item{Token::If, Vt}, Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt}, Item{statement, Vn}, Item{else_part, Vn} };     // if       ->      { if ( <expression> ) <statement> <else_part> }

    /*<else_part>:
          !        ->      { ~ }
          -        ->      { ~ }
          *        ->      { ~ }
          &        ->      { ~ }
          int      ->      { ~ }
          char     ->      { ~ }
          float    ->      { ~ }
          void     ->      { ~ }
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
          break    ->      { ~ }
          continue ->      { ~ }
    */
    ASTable[else_part][Token::Not]                               = { Item{Token::Nul, Vt} };                             // !        ->      { ~ }
    ASTable[else_part][Token::Minus]                             = { Item{Token::Nul, Vt} };                             // -        ->      { ~ }
    ASTable[else_part][Token::Mult]                              = { Item{Token::Nul, Vt} };                             // *        ->      { ~ }
    ASTable[else_part][Token::Ampersand]                         = { Item{Token::Nul, Vt} };                             // &        ->      { ~ }
    ASTable[else_part][Token::DT_int]                            = { Item{Token::Nul, Vt} };                             // int      ->      { ~ }
    ASTable[else_part][Token::DT_char]                           = { Item{Token::Nul, Vt} };                             // char      ->      { ~ }
    ASTable[else_part][Token::DT_float]                          = { Item{Token::Nul, Vt} };                             // float      ->      { ~ }
    ASTable[else_part][Token::Void]                              = { Item{Token::Nul, Vt} };                             // void      ->      { ~ }
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
    ASTable[else_part][Token::Break]                             = { Item{Token::Nul, Vt} };                             // break    ->      { ~ }
    ASTable[else_part][Token::Continue]                          = { Item{Token::Nul, Vt} };                             // continue ->      { ~ }

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
          !        ->      { <expression> }
          -        ->      { <expression> }
          *        ->      { <expression> }
          &        ->      { <expression> }
          id       ->      { <expression> }
          ;        ->      { ~ }
          num      ->      { <expression> }
          (        ->      { <expression> }
    */
    ASTable[return_tail][Token::Not]                             = { Item{expression, Vn} };     // !        ->      { <expression> }
    ASTable[return_tail][Token::Minus]                           = { Item{expression, Vn} };     // -        ->      { <expression> }
    ASTable[return_tail][Token::Mult]                            = { Item{expression, Vn} };     // *        ->      { <expression> }
    ASTable[return_tail][Token::Ampersand]                       = { Item{expression, Vn} };     // &        ->      { <expression> }
    ASTable[return_tail][Token::Identifier]                      = { Item{expression, Vn} };     // id       ->      { <expression> }
    ASTable[return_tail][Token::Semicolon]                       = { Item{Token::Nul, Vt} };     // ;        ->      { ~ }
    ASTable[return_tail][Token::Integer]                         = { Item{expression, Vn} };     // num      ->      { <expression> }
    ASTable[return_tail][Token::Character]                       = { Item{expression, Vn} };
    ASTable[return_tail][Token::Float]                           = { Item{expression, Vn} };
    ASTable[return_tail][Token::String]                          = { Item{expression, Vn} };
    ASTable[return_tail][Token::OpenParen]                       = { Item{expression, Vn} };     // (        ->      { <expression> }

    /*<jump_stmt>:
          break    ->      { break ; }
          continue ->      { continue ; }
    */
    ASTable[jump_stmt][Token::Break]                             = { Item{Token::Break, Vt}, Item{Token::Semicolon, Vt} };          // break    ->      { break ; }
    ASTable[jump_stmt][Token::Continue]                          = { Item{Token::Continue, Vt}, Item{Token::Semicolon, Vt} };       // continue ->      { continue ; }

    /*<expression>:
          !        ->      { <or_expression> <expression_tail> }
          -        ->      { <or_expression> <expression_tail> }
          *        ->      { <or_expression> <expression_tail> }
          &        ->      { <or_expression> <expression_tail> }
          id       ->      { <or_expression> <expression_tail> }
          num      ->      { <or_expression> <expression_tail> }
          (        ->      { <or_expression> <expression_tail> }
    */
    ASTable[expression][Token::Not]                              = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // !        ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Minus]                            = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // -        ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Mult]                             = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // *        ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Ampersand]                        = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // &        ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Identifier]                       = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // id       ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Integer]                          = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // num      ->      { <or_expression> <expression_tail> }
    ASTable[expression][Token::Character]                        = { Item{or_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::Float]                            = { Item{or_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::String]                           = { Item{or_expression, Vn}, Item{expression_tail, Vn} };
    ASTable[expression][Token::OpenParen]                        = { Item{or_expression, Vn}, Item{expression_tail, Vn} };         // (        ->      { <or_expression> <expression_tail> }

    /*<expression_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          =        ->      { = <or_expression> <expression_tail> }
    */
    ASTable[expression_tail][Token::Semicolon]                   = { Item{Token::Nul, Vt} };                                                          // ;        ->      { ~ }        
    ASTable[expression_tail][Token::CloseSquare]                 = { Item{Token::Nul, Vt} };                                                          // ]        ->      { ~ }
    ASTable[expression_tail][Token::CloseParen]                  = { Item{Token::Nul, Vt} };                                                          // )        ->      { ~ }
    ASTable[expression_tail][Token::Comma]                       = { Item{Token::Nul, Vt} };                                                          // ,        ->      { ~ }
    ASTable[expression_tail][Token::Eq]                          = { Item{Token::Eq, Vt}, Item{or_expression, Vn}, Item{expression_tail, Vn} };       // =        ->      { = <or_expression> <expression_tail> }
    
    /*<or_expression>:
          !        ->      { <and_expression> <or_expression_tail> }
          -        ->      { <and_expression> <or_expression_tail> }
          *        ->      { <and_expression> <or_expression_tail> }
          &        ->      { <and_expression> <or_expression_tail> }
          id       ->      { <and_expression> <or_expression_tail> }
          num      ->      { <and_expression> <or_expression_tail> }
          (        ->      { <and_expression> <or_expression_tail> }
    */
    ASTable[or_expression][Token::Not]                              = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // !        ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Minus]                            = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // -        ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Mult]                             = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // *        ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Ampersand]                        = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // &        ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Identifier]                       = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // id       ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Integer]                          = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // num      ->      { <and_expression> <or_expression_tail> }
    ASTable[or_expression][Token::Character]                        = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };
    ASTable[or_expression][Token::Float]                            = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };
    ASTable[or_expression][Token::String]                           = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };
    ASTable[or_expression][Token::OpenParen]                        = { Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // (        ->      { <and_expression> <or_expression_tail> }

    /*<or_expression_tail>:
          ;        ->      { ~ }
          =        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          ||       ->      { || <and_expression> <or_expression_tail> }
    */
    ASTable[or_expression_tail][Token::Semicolon]                = { Item{Token::Nul, Vt} };                                                                // ;        ->      { ~ }
    ASTable[or_expression_tail][Token::Eq]                       = { Item{Token::Nul, Vt} };                                                                // =        ->      { ~ }
    ASTable[or_expression_tail][Token::CloseSquare]              = { Item{Token::Nul, Vt} };                                                                // ]        ->      { ~ }
    ASTable[or_expression_tail][Token::CloseParen]               = { Item{Token::Nul, Vt} };                                                                // )        ->      { ~ }
    ASTable[or_expression_tail][Token::Comma]                    = { Item{Token::Nul, Vt} };                                                                // ,        ->      { ~ }
    ASTable[or_expression_tail][Token::Or]                       = { Item{Token::Or, Vt}, Item{and_expression, Vn}, Item{or_expression_tail, Vn} };         // ||       ->      { || <and_expression> <or_expression_tail> }
    
    /*<and_expression>:
          !        ->      { <relational_expression> <and_expression_tail> }
          -        ->      { <relational_expression> <and_expression_tail> }
          *        ->      { <relational_expression> <and_expression_tail> }
          &        ->      { <relational_expression> <and_expression_tail> }
          id       ->      { <relational_expression> <and_expression_tail> }
          num      ->      { <relational_expression> <and_expression_tail> }
          (        ->      { <relational_expression> <and_expression_tail> }
    */
    ASTable[and_expression][Token::Not]                              = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // !        ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Minus]                            = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // -        ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Mult]                             = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // *        ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Ampersand]                        = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // &        ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Identifier]                       = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // id       ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Integer]                          = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // num      ->      { <relational_expression> <and_expression_tail> }
    ASTable[and_expression][Token::Character]                        = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };
    ASTable[and_expression][Token::Float]                            = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };
    ASTable[and_expression][Token::String]                           = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };
    ASTable[and_expression][Token::OpenParen]                        = { Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };         // (        ->      { <relational_expression> <and_expression_tail> }

    /*<and_expression_tail>:
          ;        ->      { ~ }
          =        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          ||       ->      { ~ }
          &&       ->      { && <relational_expression> <and_expression_tail> }
    */
    ASTable[and_expression_tail][Token::Semicolon]                = { Item{Token::Nul, Vt} };                                                                     // ;        ->      { ~ }
    ASTable[and_expression_tail][Token::Eq]                       = { Item{Token::Nul, Vt} };                                                                     // =        ->      { ~ }
    ASTable[and_expression_tail][Token::CloseSquare]              = { Item{Token::Nul, Vt} };                                                                     // ]        ->      { ~ }
    ASTable[and_expression_tail][Token::CloseParen]               = { Item{Token::Nul, Vt} };                                                                     // )        ->      { ~ }
    ASTable[and_expression_tail][Token::Comma]                    = { Item{Token::Nul, Vt} };                                                                     // ,        ->      { ~ }
    ASTable[and_expression_tail][Token::Or]                       = { Item{Token::Nul, Vt} };                                                                     // ||       ->      { ~ }
    ASTable[and_expression_tail][Token::And]                      = { Item{Token::And, Vt}, Item{relational_expression, Vn}, Item{and_expression_tail, Vn} };     // &&       ->      { && <relational_expression> <and_expression_tail> }
    
    /*<relational_expression>:
          !        ->      { <additive_expression> <relational_expression_tail> }
          -        ->      { <additive_expression> <relational_expression_tail> }
          *        ->      { <additive_expression> <relational_expression_tail> }
          &        ->      { <additive_expression> <relational_expression_tail> }
          id       ->      { <additive_expression> <relational_expression_tail> }
          num      ->      { <additive_expression> <relational_expression_tail> }
          (        ->      { <additive_expression> <relational_expression_tail> }
    */
    ASTable[relational_expression][Token::Not]                    = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // !        ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Minus]                  = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // -        ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Mult]                   = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // *        ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Ampersand]              = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // &        ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Identifier]             = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // id       ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Integer]                = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // num      ->      { <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression][Token::Character]              = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} }; 
    ASTable[relational_expression][Token::Float]                  = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} }; 
    ASTable[relational_expression][Token::String]                 = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} }; 
    ASTable[relational_expression][Token::OpenParen]              = { Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };    // (        ->      { <additive_expression> <relational_expression_tail> }
    
    /*<relational_expression_tail>:
          ;        ->      { ~ }
          ]        ->      { ~ }
          )        ->      { ~ }
          ,        ->      { ~ }
          =        ->      { ~ }
          ||       ->      { ~ }
          &&       ->      { ~ }
          <=       ->      { <relop> <additive_expression> <relational_expression_tail> }
          <        ->      { <relop> <additive_expression> <relational_expression_tail> }
          >        ->      { <relop> <additive_expression> <relational_expression_tail> }
          >=       ->      { <relop> <additive_expression> <relational_expression_tail> }
          ==       ->      { <relop> <additive_expression> <relational_expression_tail> }
          !=       ->      { <relop> <additive_expression> <relational_expression_tail> }
    */
    ASTable[relational_expression_tail][Token::Semicolon]                   = { Item{Token::Nul, Vt} };                                                                      // ;        ->      { ~ }        
    ASTable[relational_expression_tail][Token::CloseSquare]                 = { Item{Token::Nul, Vt} };                                                                      // ]        ->      { ~ }
    ASTable[relational_expression_tail][Token::CloseParen]                  = { Item{Token::Nul, Vt} };                                                                      // )        ->      { ~ }
    ASTable[relational_expression_tail][Token::Comma]                       = { Item{Token::Nul, Vt} };                                                                      // ,        ->      { ~ }
    ASTable[relational_expression_tail][Token::Eq]                          = { Item{Token::Nul, Vt} };                                                                      // =        ->      { ~ }
    ASTable[relational_expression_tail][Token::Or]                          = { Item{Token::Nul, Vt} };                                                                      // ||       ->      { ~ }
    ASTable[relational_expression_tail][Token::And]                         = { Item{Token::Nul, Vt} };                                                                      // &&       ->      { ~ }
    ASTable[relational_expression_tail][Token::Less_Eq]                     = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // <=       ->      { <relop> <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression_tail][Token::Less]                        = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // <        ->      { <relop> <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression_tail][Token::Greater]                     = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // >        ->      { <relop> <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression_tail][Token::Greater_Eq]                  = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // >=       ->      { <relop> <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression_tail][Token::Eq_Eq]                       = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // ==       ->      { <relop> <additive_expression> <relational_expression_tail> }
    ASTable[relational_expression_tail][Token::Not_Eq]                      = { Item{relop, Vn}, Item{additive_expression, Vn}, Item{relational_expression_tail, Vn} };      // !=       ->      { <relop> <additive_expression> <relational_expression_tail> }
    
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
          !        ->      { <term> <additive_expression_tail> }
          -        ->      { <term> <additive_expression_tail> }
          *        ->      { <term> <additive_expression_tail> }
          &        ->      { <term> <additive_expression_tail> }
          id       ->      { <term> <additive_expression_tail> }
          num      ->      { <term> <additive_expression_tail> }
          (        ->      { <term> <additive_expression_tail> }
    */
    ASTable[additive_expression][Token::Not]                     = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // !        ->      { <term> <additive_expression_tail> }
    ASTable[additive_expression][Token::Minus]                   = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // -        ->      { <term> <additive_expression_tail> }
    ASTable[additive_expression][Token::Mult]                    = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // *        ->      { <term> <additive_expression_tail> }
    ASTable[additive_expression][Token::Ampersand]               = { Item{term, Vn}, Item{additive_expression_tail, Vn} };       // &        ->      { <term> <additive_expression_tail> }
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
          =        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          ||       ->      { ~ }
          &&       ->      { ~ }
          +        ->      { <addop> <term> <additive_expression_tail> }
          -        ->      { <addop> <term> <additive_expression_tail> }
   */
    ASTable[additive_expression_tail][Token::Semicolon]          = { Item{Token::Nul, Vt} };                                                     // ;        ->      { ~ }
    ASTable[additive_expression_tail][Token::CloseSquare]        = { Item{Token::Nul, Vt} };                                                     // ]        ->      { ~ }
    ASTable[additive_expression_tail][Token::CloseParen]         = { Item{Token::Nul, Vt} };                                                     // )        ->      { ~ }
    ASTable[additive_expression_tail][Token::Comma]              = { Item{Token::Nul, Vt} };                                                     // ,        ->      { ~ }
    ASTable[additive_expression_tail][Token::Eq]                 = { Item{Token::Nul, Vt} };                                                     // =        ->      { ~ }
    ASTable[additive_expression_tail][Token::Less_Eq]            = { Item{Token::Nul, Vt} };                                                     // <=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Less]               = { Item{Token::Nul, Vt} };                                                     // <        ->      { ~ }
    ASTable[additive_expression_tail][Token::Greater]            = { Item{Token::Nul, Vt} };                                                     // >        ->      { ~ }
    ASTable[additive_expression_tail][Token::Greater_Eq]         = { Item{Token::Nul, Vt} };                                                     // >=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Eq_Eq]              = { Item{Token::Nul, Vt} };                                                     // ==       ->      { ~ }
    ASTable[additive_expression_tail][Token::Not_Eq]             = { Item{Token::Nul, Vt} };                                                     // !=       ->      { ~ }
    ASTable[additive_expression_tail][Token::Or]                 = { Item{Token::Nul, Vt} };                                                     // ||       ->      { ~ }
    ASTable[additive_expression_tail][Token::And]                = { Item{Token::Nul, Vt} };                                                     // &&       ->      { ~ }
    ASTable[additive_expression_tail][Token::Plus]               = { Item{addop, Vn}, Item{term, Vn}, Item{additive_expression_tail, Vn} };      // +        ->      { <addop> <term> <additive_expression_tail> }
    ASTable[additive_expression_tail][Token::Minus]              = { Item{addop, Vn}, Item{term, Vn}, Item{additive_expression_tail, Vn} };      // -        ->      { <addop> <term> <additive_expression_tail> }
    
    /*<addop>:
          +        ->      { + }
          -        ->      { - }
    */
    ASTable[addop][Token::Plus]                                  = { Item{Token::Plus, Vt} };    // +        ->      { + }
    ASTable[addop][Token::Minus]                                 = { Item{Token::Minus, Vt} };   // -        ->      { - }

    /*<term>:
          *        ->      { <factor> <term_tail> }
          !        ->      { <factor> <term_tail> }
          -        ->      { <factor> <term_tail> }
          &        ->      { <factor> <term_tail> }
          id       ->      { <factor> <term_tail> }
          num      ->      { <factor> <term_tail> }
          (        ->      { <factor> <term_tail> }
    */
    ASTable[term][Token::Mult]                                   = { Item{factor, Vn}, Item{term_tail, Vn} };    // *        ->      { <factor> <term_tail> }
    ASTable[term][Token::Not]                                    = { Item{factor, Vn}, Item{term_tail, Vn} };    // !        ->      { <factor> <term_tail> }
    ASTable[term][Token::Minus]                                  = { Item{factor, Vn}, Item{term_tail, Vn} };    // -        ->      { <factor> <term_tail> }
    ASTable[term][Token::Ampersand]                              = { Item{factor, Vn}, Item{term_tail, Vn} };    // &        ->      { <factor> <term_tail> }
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
          =        ->      { ~ }
          <=       ->      { ~ }
          <        ->      { ~ }
          >        ->      { ~ }
          >=       ->      { ~ }
          ==       ->      { ~ }
          !=       ->      { ~ }
          +        ->      { ~ }
          -        ->      { ~ }
          ||       ->      { ~ }
          &&       ->      { ~ }
          *        ->      { <mulop> <factor> <term_tail> }
          /        ->      { <mulop> <factor> <term_tail> }
          %        ->      { <mulop> <factor> <term_tail> }
    */
    ASTable[term_tail][Token::Semicolon]                         = { Item{Token::Nul, Vt} };                                     // ;        ->      { ~ }
    ASTable[term_tail][Token::CloseSquare]                       = { Item{Token::Nul, Vt} };                                     // ]        ->      { ~ }
    ASTable[term_tail][Token::CloseParen]                        = { Item{Token::Nul, Vt} };                                     // )        ->      { ~ }
    ASTable[term_tail][Token::Comma]                             = { Item{Token::Nul, Vt} };                                     // ,        ->      { ~ }
    ASTable[term_tail][Token::Eq]                                = { Item{Token::Nul, Vt} };                                     // =        ->      { ~ }
    ASTable[term_tail][Token::Less_Eq]                           = { Item{Token::Nul, Vt} };                                     // <=       ->      { ~ }
    ASTable[term_tail][Token::Less]                              = { Item{Token::Nul, Vt} };                                     // <        ->      { ~ }
    ASTable[term_tail][Token::Greater]                           = { Item{Token::Nul, Vt} };                                     // >        ->      { ~ }
    ASTable[term_tail][Token::Greater_Eq]                        = { Item{Token::Nul, Vt} };                                     // >=       ->      { ~ }
    ASTable[term_tail][Token::Eq_Eq]                             = { Item{Token::Nul, Vt} };                                     // ==       ->      { ~ }
    ASTable[term_tail][Token::Not_Eq]                            = { Item{Token::Nul, Vt} };                                     // !=       ->      { ~ }
    ASTable[term_tail][Token::Plus]                              = { Item{Token::Nul, Vt} };                                     // +        ->      { ~ }
    ASTable[term_tail][Token::Minus]                             = { Item{Token::Nul, Vt} };                                     // -        ->      { ~ }
    ASTable[term_tail][Token::Or]                                = { Item{Token::Nul, Vt} };                                     // ||       ->      { ~ }
    ASTable[term_tail][Token::And]                               = { Item{Token::Nul, Vt} };                                     // &&       ->      { ~ }
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
          (        ->      { ( <expression> ) <factor_tail> }
          *        ->      { * <factor> }
          !        ->      { ! <factor> }
          -        ->      { - <factor> }
          &        ->      { & <factor> }
          id       ->      { id <factor_tail> }
          num      ->      { num <factor_tail> }
    */
    ASTable[factor][Token::OpenParen]                            = { Item{Token::OpenParen, Vt}, Item{expression, Vn}, Item{Token::CloseParen, Vt}, Item{factor_tail, Vn} };  // (        ->      { ( <expression> ) <factor_tail> }
    ASTable[factor][Token::Mult]                                 = { Item{Token::Mult, Vt}, Item{factor, Vn} };                                                               // *        ->      { * <factor> }
    ASTable[factor][Token::Not]                                  = { Item{Token::Not, Vt}, Item{factor, Vn} };                                                                // !        ->      { ! <factor> }
    ASTable[factor][Token::Minus]                                = { Item{Token::Minus, Vt}, Item{factor, Vn} };                                                              // -        ->      { - <factor> }
    ASTable[factor][Token::Ampersand]                            = { Item{Token::Ampersand, Vt}, Item{factor, Vn} };                                                          // &        ->      { & <factor> }
    ASTable[factor][Token::Identifier]                           = { Item{Token::Identifier, Vt}, Item{factor_tail, Vn} };                                                    // id       ->      { id <factor_tail> }
    ASTable[factor][Token::String]                               = { Item{Token::String, Vt}, Item{factor_tail, Vn} };                                                        // num      ->      { num <factor_tail> }
    ASTable[factor][Token::Integer]                              = { Item{Token::Integer, Vt} };
    ASTable[factor][Token::Character]                            = { Item{Token::Character, Vt} };
    ASTable[factor][Token::Float]                                = { Item{Token::Float, Vt} };
    
    /*<factor_tail>:
          ;        ->      { ~ }
          [        ->      { [ <expression> ] <factor_tail> }
          ]        ->      { ~ }
          (        ->      { ( <args> ) <factor_tail> }
          )        ->      { ~ }
          ,        ->      { ~ }
          =        ->      { ~ }
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
          ||       ->      { ~ }
          &&       ->      { ~ }
    */
    ASTable[factor_tail][Token::Semicolon]                           = { Item{Token::Nul, Vt} };                                                                                          // ;        ->      { ~ }
    ASTable[factor_tail][Token::OpenSquare]                          = { Item{Token::OpenSquare, Vt}, Item{expression, Vn}, Item{Token::CloseSquare, Vt}, Item{factor_tail, Vn} };        // [        ->      { [ <expression> ] <factor_tail> }
    ASTable[factor_tail][Token::CloseSquare]                         = { Item{Token::Nul, Vt} };                                                                                          // ]        ->      { ~ }
    ASTable[factor_tail][Token::OpenParen]                           = { Item{Token::OpenParen, Vt}, Item{args, Vn}, Item{Token::CloseParen, Vt}, Item{factor_tail, Vn} };                // (        ->      { ( <args> ) <factor_tail> }
    ASTable[factor_tail][Token::CloseParen]                          = { Item{Token::Nul, Vt} };                                                                                          // )        ->      { ~ }
    ASTable[factor_tail][Token::Comma]                               = { Item{Token::Nul, Vt} };                                                                                          // ,        ->      { ~ }
    ASTable[factor_tail][Token::Eq]                                  = { Item{Token::Nul, Vt} };                                                                                          // =        ->      { ~ }
    ASTable[factor_tail][Token::Less_Eq]                             = { Item{Token::Nul, Vt} };                                                                                          // <=       ->      { ~ }
    ASTable[factor_tail][Token::Less]                                = { Item{Token::Nul, Vt} };                                                                                          // <        ->      { ~ }
    ASTable[factor_tail][Token::Greater]                             = { Item{Token::Nul, Vt} };                                                                                          // >        ->      { ~ }
    ASTable[factor_tail][Token::Greater_Eq]                          = { Item{Token::Nul, Vt} };                                                                                          // >=       ->      { ~ }
    ASTable[factor_tail][Token::Eq_Eq]                               = { Item{Token::Nul, Vt} };                                                                                          // ==       ->      { ~ }
    ASTable[factor_tail][Token::Not_Eq]                              = { Item{Token::Nul, Vt} };                                                                                          // !=       ->      { ~ }
    ASTable[factor_tail][Token::Plus]                                = { Item{Token::Nul, Vt} };                                                                                          // +        ->      { ~ }
    ASTable[factor_tail][Token::Minus]                               = { Item{Token::Nul, Vt} };                                                                                          // -        ->      { ~ }
    ASTable[factor_tail][Token::Mult]                                = { Item{Token::Nul, Vt} };                                                                                          // *        ->      { ~ }
    ASTable[factor_tail][Token::Div]                                 = { Item{Token::Nul, Vt} };                                                                                          // /        ->      { ~ }
    ASTable[factor_tail][Token::Mod]                                 = { Item{Token::Nul, Vt} };                                                                                          // %        ->      { ~ }
    ASTable[factor_tail][Token::Or]                                  = { Item{Token::Nul, Vt} };                                                                                          // ||       ->      { ~ }
    ASTable[factor_tail][Token::And]                                 = { Item{Token::Nul, Vt} };                                                                                          // &&       ->      { ~ }

    /*<args>:
          !        ->      { <expression> <arg_tail> }
          -        ->      { <expression> <arg_tail> }
          *        ->      { <expression> <arg_tail> }
          &        ->      { <expression> <arg_tail> }
          id       ->      { <expression> <arg_tail> }
          num      ->      { <expression> <arg_tail> }
          (        ->      { <expression> <arg_tail> }
          )        ->      { ~ }
    */
    ASTable[args][Token::Not]                                    = { Item{expression, Vn}, Item{arg_tail, Vn} };         // !        ->      { <expression> <arg_tail> }
    ASTable[args][Token::Minus]                                  = { Item{expression, Vn}, Item{arg_tail, Vn} };         // -        ->      { <expression> <arg_tail> }
    ASTable[args][Token::Mult]                                   = { Item{expression, Vn}, Item{arg_tail, Vn} };         // *        ->      { <expression> <arg_tail> }
    ASTable[args][Token::Ampersand]                              = { Item{expression, Vn}, Item{arg_tail, Vn} };         // &        ->      { <expression> <arg_tail> }
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
}