#include "TACGenerator.h"
#include "../../symtab/SymbolTable.h"
#include "../../debug/Debug.h"

TACGenerator::TACGenerator(const Parser::TreeNode &root)
    : root(root), temp_counter(0U), lable_counter(0U), scope_counter(0U), m_hasError(false),
    dataSizeMap
    ({
        { Token::DT_int, 8 },
        { Token::DT_char, 1 },
        { Token::DT_float, 4 }
    })
{
    scopeStack.push(".global");
}

const std::vector<TACGenerator::Quaternion> &TACGenerator::exec()
{
    SymbolTable::GetInstance().insert("mycc_putchar", ".global", new FunctionSymbol(Token::Void, std::vector<Token::Type>{ Token::DT_int }));
    generate_3ac(root);
    return code;
}

bool TACGenerator::hasError() const
{
    return m_hasError;
}

std::string TACGenerator::new_temp()
{
    return std::string("t^") + std::to_string(temp_counter ++);
}

std::string TACGenerator::new_lable()
{
    return std::string(".lable") + std::to_string(lable_counter ++);
}

Symbol *TACGenerator::getSymbol(const std::string &symbol_name)
{
    std::stack<std::string> scopeStack_copy = scopeStack;
    while (!scopeStack_copy.empty())
    {
        Symbol *sym = SymbolTable::GetInstance().find(symbol_name, scopeStack_copy.top());
        scopeStack_copy.pop();
        if (sym)
        {
            return sym;
        }
    }
    return nullptr;
}

void TACGenerator::generate_3ac(const Parser::TreeNode &node)
{
    switch (node.vn_type)
    {
    /* <program> -> <declaration_list> */
    case Parser::program:
    /* <declaration_list> -> <declaration> <declaration_list> | ~ */
    case Parser::declaration_list:
    /* <compound_list> -> <compound> <compound_list> | ~ */
    case Parser::compound_list:
    /* <compound> -> <var_dec> | <statement> */
    case Parser::compound:
    /* <statement> -> <expression_stmt> | <compound_stmt> | <selection_stmt> | <iteration_stmt> | <return_stmt> */
    case Parser::statement:
    {
        for (const Parser::TreeNode &child : node.childs)
        {
            generate_3ac(child);
        }
        break;
    }
    /* <declaration> -> type id <dec_tail> */
    case Parser::declaration:
    {
        const Token &type = node.tokens[0];
        const Token &id = node.tokens[1];
        const Parser::TreeNode &dec_tail = node.childs[0];
        /* <dec_tail> -> ; */
        if (dec_tail.tokens[0].type() == Token::Semicolon)
        {
            // dec var
            dec_var(false, type, id);
        }
        /* <dec_tail> -> [ num ] ; */
        else if (dec_tail.tokens[0].type() == Token::OpenSquare)
        {
            // dec arr
            const Token &num = dec_tail.tokens[1];
            dec_var(false, type, id, true, std::stoll(num.lexeme()));
        }
        /* ( <params> ) <compound_stmt> */
        else if (dec_tail.tokens[0].type() == Token::OpenParen)
        {
            // dec func
            dec_function(type, id, dec_tail);
        }
        break;
    }
    /* <compound_stmt> -> { <compound_list> } */
    case Parser::compound_stmt:
    {
        if (++ scope_counter != 1)
        {
            scopeStack.push(curFunName + '~' + std::to_string(scope_counter));
        }
        generate_3ac(node.childs[0]);
        scopeStack.pop();
        break;
    }
    /* <var_dec> -> type id <var_dec_tail> ; */
    case Parser::var_dec:
    {
        const Token &type = node.tokens[0];
        const Token &id = node.tokens[1];
        const Parser::TreeNode &var_dec_tail = node.childs[0];
        /* <var_dec_tail> -> ~ */
        if (var_dec_tail.tokens.empty())
        {
            dec_var(true, type, id);
        }
        /* <var_dec_tail> -> [ num ] */
        else
        {
            const Token &num = var_dec_tail.tokens[1];
            dec_var(true, type, id, true, std::stoll(num.lexeme()));
        }
        break;
    }
    /* <expression_stmt> -> <expression> ; | ; */
    case Parser::expression_stmt:
    {
        if (!node.childs.empty())
        {
            do_expression(node.childs[0]);
        }
        break;
    }
    /* <selection_stmt> -> if ( <expression> ) <statement> <else_part> */
    case Parser::selection_stmt:
    {
        const Parser::TreeNode &expression = node.childs[0];
        const Parser::TreeNode &statement = node.childs[1];
        const Parser::TreeNode &else_part = node.childs[2];
        const Parser::TreeNode *else_statement = nullptr;
        /* <else_part> -> else <statement> | ~ */
        if (!else_part.childs.empty())
        {
            else_statement = &else_part.childs[0];
        }
        std::string condition = do_expression(expression);
        std::string lable1 = new_lable();
        std::string lable2 = new_lable();
        code.push_back({ Op_if, condition, "", lable1});    // if (contidion) goto L1
        code.push_back({ Op_goto, "", "", lable2});         // goto L2
        code.push_back({ Op_label, "", "", lable1});        // L1:
        generate_3ac(statement);                            // statements
        if (else_statement)
        {// else
            std::string lable3 = new_lable();
            code.push_back({ Op_goto, "", "", lable3});     // goto L3
            code.push_back({ Op_label, "", "", lable2});    // L2:
            generate_3ac(*else_statement);                  // else_statement
            code.push_back({ Op_label, "", "", lable3});    // L3:
        }
        else
        {// no else part
            code.push_back({ Op_label, "", "", lable2});    // L2:
        }
        break;
    }
    /* <iteration_stmt> -> while ( <expression> ) <statement> | for ( <expression> ; <expression> ; <expression> ) <statement> */
    case Parser::iteration_stmt:
    {
        bool is_for = node.tokens[0].type() == Token::For;
        const Parser::TreeNode *condition_node = &node.childs[0];
        if (is_for)
        {
            condition_node = &node.childs[1];
        }
        const Parser::TreeNode &statement = is_for ? node.childs[3] : node.childs[1];
        std::string lable1 = new_lable();
        std::string lable2 = new_lable();
        std::string lable3 = new_lable();
        if (is_for)
        {
            do_expression(node.childs[0]);
        }
        code.push_back({ Op_label, "", "", lable1 });       // L1:
        std::string condition = do_expression(*condition_node);
        code.push_back({ Op_if, condition, "", lable2 });   // if (condition) goto L2
        code.push_back({ Op_goto, "", "", lable3 });        // goto L3
        code.push_back({ Op_label, "", "", lable2 });       // L2:
        generate_3ac(statement);                            // do statement
        if (is_for)
        {
            do_expression(node.childs[2]);
        }
        code.push_back({ Op_goto, "", "", lable1 });        // goto L1
        code.push_back({ Op_label, "", "", lable3 });       // L3:
        break;
    }
    /* <return_stmt> -> return <return_tail> ; */
    case Parser::return_stmt:
    {
        const Parser::TreeNode &return_tail = node.childs[0];
        /* <return_tail> -> <expression> | ~ */
        if (!return_tail.childs.empty())
        {
            const Parser::TreeNode &expression = return_tail.childs[0];
            std::string return_value = do_expression(expression);
            code.push_back({ Op_assign, return_value, "", "~ret" });    // return return_value
        }
        code.push_back({ Op_goto, "", "", curFunName + "_exit" });      // goto func_exit
        break;
    }
    }

}

void TACGenerator::dec_var(bool local, const Token &type, const Token &id, bool isArray, int arrSize)
{
    if (type.type() == Token::Void)
    {
        Debug::TypeError(type);
        m_hasError = true;
        return;
    }
    else if (SymbolTable::GetInstance().find(id.lexeme(), scopeStack.top()) != nullptr)
    {
        Debug::Redeclare(id);
        m_hasError = true;
        return;
    }
    else
    {
        std::string newName = id.lexeme();
        if (local)
        {
            //newName += "~" + std::to_string(scope_counter);
            const std::string &suffix = scopeStack.top();
            newName += "~" + suffix.substr(suffix.find('~') + 1);
        }
        if (!isArray)
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new VarSymbol(type.type(), newName));
            code.push_back({ local ? Op_local_var : Op_global_var, std::to_string(dataSizeMap[type.type()]), "", newName });
        }
        else
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new ArrSymbol(type.type(), arrSize, newName));
            code.push_back({ local ? Op_local_var : Op_global_var, std::to_string(dataSizeMap[type.type()] * arrSize), "", newName });
        }
    }
}

void TACGenerator::dec_function(const Token &type, const Token &id, const Parser::TreeNode &dec_tail)
{
    curFunName = id.lexeme();

    code.push_back({ Op_begin_func, "", "", curFunName });
    scopeStack.push(curFunName + "~1"); // push scope '{func name}~0'

    scope_counter = 1;

    std::vector<Token::Type> params_type;
    dec_params(dec_tail.childs[0], params_type); // <params>
    SymbolTable::GetInstance().insert(curFunName, ".global", new FunctionSymbol(type.type(), params_type));

    scope_counter = 0;
    generate_3ac(dec_tail.childs[1]);   // <compound_stmt>
    
    code.push_back({ Op_label, "", "", curFunName + "_exit" }); // func_exit:

    code.push_back({ Op_end_func, "", "", curFunName });
}

void TACGenerator::dec_params(const Parser::TreeNode &node, std::vector<Token::Type> &params_type)
{
    /* <param> -> type id */
    if (node.vn_type == Parser::param)
    {
        Token type = node.tokens[0];
        Token id = node.tokens[1];
        std::string varName = id.lexeme();

        if (type.type() == Token::Void)
        {
            Debug::TypeError(type);
            m_hasError = true;
            return;
        }
        else if (SymbolTable::GetInstance().find(varName, scopeStack.top()) != nullptr)
        {
            Debug::Redeclare(id);
            m_hasError = true;
            return;
        }
        else
        {
            std::string newName = varName + "~" + std::to_string(scope_counter);
            SymbolTable::GetInstance().insert(varName, scopeStack.top(), new VarSymbol(type.type(), newName));
            code.push_back({ Op_dec_param, std::to_string(dataSizeMap[type.type()]), "", newName });
            params_type.push_back(type.type());
        }
    }
    else
    {
        /* <params> -> <param> <params_tail> | ~ */
        /* <params_tail> -> , <param> <params_tail> | ~ */
        for (const Parser::TreeNode &child : node.childs)
        {
            dec_params(child, params_type);
        }
    }

}

std::string TACGenerator::do_expression(const Parser::TreeNode &node)
{
    /* <expression> -> <additive_expression> <expression_tail> */
    if (node.vn_type == Parser::expression)
    {
        const Parser::TreeNode &additive_expression = node.childs[0];
        const Parser::TreeNode &expression_tail = node.childs[1];
        std::string temp1 = do_expression(additive_expression);
        /* <expression_tail> -> <relop> <additive_expression> | ~ */
        if (!expression_tail.childs.empty())
        {
            const Parser::TreeNode &relop = expression_tail.childs[0];
            const Parser::TreeNode &additive_expression = expression_tail.childs[1];
            std::string temp2 = do_expression(additive_expression);
            std::string temp3 = new_temp();
            /* <relop> -> <= | < | > | >= | == | != */
            OpType op;
            if (relop.tokens[0].type() == Token::Less_Eq)
            {
                op = Op_less_eq;
            }
            else if (relop.tokens[0].type() == Token::Less)
            {
                op = Op_less;
            }
            else if (relop.tokens[0].type() == Token::Greater)
            {
                op = Op_greater;
            }
            else if (relop.tokens[0].type() == Token::Greater_Eq)
            {
                op = Op_greater_eq;
            }
            else if (relop.tokens[0].type() == Token::Eq_Eq)
            {
                op = Op_eq_eq;
            }
            else if (relop.tokens[0].type() == Token::Not_Eq)
            {
                op = Op_not_eq;
            }
            code.push_back({ op, temp1, temp2, temp3 });
            return temp3;
        }
        return temp1;
    }
    /* <additive_expression> -> <term> <additive_expression_tail> */
    else if (node.vn_type == Parser::additive_expression)
    {
        const Parser::TreeNode &term = node.childs[0];
        const Parser::TreeNode *additive_expression_tail = &node.childs[1];
        std::string temp1 = do_expression(term);
        /* <additive_expression_tail> -> <addop> <term> <additive_expression_tail> | ~ */
        while (!additive_expression_tail->childs.empty())
        {
            const Parser::TreeNode &addop = additive_expression_tail->childs[0];
            const Parser::TreeNode &term = additive_expression_tail->childs[1];
            additive_expression_tail = &additive_expression_tail->childs[2];
            std::string temp2 = do_expression(term);
            /* <addop> -> + | - */
            OpType op;
            if (addop.tokens[0].type() == Token::Plus)
            {
                op = Op_add;
            }
            else if (addop.tokens[0].type() == Token::Minus)
            {
                op = Op_sub;
            }
            std::string temp3 = new_temp();
            code.push_back({ op, temp1, temp2, temp3 });
            temp1 = temp3;
        }
        return temp1;
    }
    /* <term> -> <factor> <term_tail> */
    else if (node.vn_type == Parser::term)
    {
        const Parser::TreeNode &factor = node.childs[0];
        const Parser::TreeNode *term_tail = &node.childs[1];
        std::string temp1 = do_expression(factor);
        /* <term_tail> -> <mulop> <factor> <term_tail> | ~ */
        while (!term_tail->childs.empty())
        {
            const Parser::TreeNode &mulop = term_tail->childs[0];
            const Parser::TreeNode &factor = term_tail->childs[1];
            term_tail = &term_tail->childs[2];
            std::string temp2 = do_expression(factor);
            /* <addop> -> * | / */
            OpType op;
            if (mulop.tokens[0].type() == Token::Mult)
            {
                op = Op_mult;
            }
            else if (mulop.tokens[0].type() == Token::Div)
            {
                op = Op_div;
            }
            std::string temp3 = new_temp();
            code.push_back({ op, temp1, temp2, temp3 });
            temp1 = temp3;
        }
        return temp1;
    }
    /* <factor> -> ( <expression> ) | id <id_tail> | num */
    else if (node.vn_type == Parser::factor)
    {
        /* <factor> -> ( <expression> ) */
        if (node.tokens[0].type() == Token::OpenParen)
        {
            const Parser::TreeNode &expression = node.childs[0];
            return do_expression(expression);
        }
        /* <factor> -> id <id_tail> */
        else if (node.tokens[0].type() == Token::Identifier)
        {
            const Token &id = node.tokens[0];
            const Parser::TreeNode &id_tail = node.childs[0];
            Symbol *id_sym = getSymbol(id.lexeme());
            if (id_sym == nullptr)
            {
                Debug::VarUndefined(id);
                m_hasError = true;
                return "";
            }
            /* <id_tail> -> ~ */
            if (id_tail.childs.empty())
            {
                if (id_sym->type != Symbol::Var)
                {
                    Debug::NotNormalVar(id);
                    m_hasError = true;
                    return "";
                }
                VarSymbol *var_sym = static_cast<VarSymbol *>(id_sym);
                return var_sym->newName;
                
            }
            /* <id_tail> -> <arr_idx> */
            else if (id_tail.tokens.empty())
            {
                if (id_sym->type != Symbol::Arr)
                {
                    Debug::NotArray(id);
                    m_hasError = true;
                    return "";
                }
                ArrSymbol *arr_sym = static_cast<ArrSymbol *>(id_sym);
                /* <arr_idx> -> [ <expression> ] <arr_idx_tail> */
                const Parser::TreeNode &arr_idx = id_tail.childs[0];
                const Parser::TreeNode &expression = arr_idx.childs[0];
                const Parser::TreeNode &arr_idx_tail = arr_idx.childs[1];
                std::string idx = do_expression(expression);
                std::string temp = new_temp();
                code.push_back({ Op_mult, idx, std::to_string(dataSizeMap[arr_sym->data_type]), temp });
                std::string arr_str = arr_sym->newName + '[' + temp + ']';
                /* <arr_idx_tail> -> ~ */
                if (arr_idx_tail.childs.empty())
                {
                    return arr_str;
                }
                /* <arr_idx_tail> -> = <expression> */
                else
                {
                    const Parser::TreeNode &expression = arr_idx_tail.childs[0];
                    std::string temp = do_expression(expression);
                    code.push_back({ Op_assign, temp, "", arr_str });
                    return arr_str;
                }
            }
            /* <id_tail> -> = <expression> */
            else if (id_tail.tokens[0].type() == Token::Eq)
            {
                VarSymbol *val_sym = static_cast<VarSymbol *>(id_sym);
                const Parser::TreeNode &expression = id_tail.childs[0];
                std::string temp = do_expression(expression);
                code.push_back({ Op_assign, temp, "", val_sym->newName });
                return val_sym->newName;
            }
            /* <id_tail> -> ( <args> ) */
            else if (id_tail.tokens[0].type() == Token::OpenParen)
            {
                if (id_sym->type != Symbol::Func)
                {
                    Debug::NotFunction(id);
                    m_hasError = true;
                    return "";
                }
                FunctionSymbol *func_sym = static_cast<FunctionSymbol *>(id_sym);
                const Parser::TreeNode &args = id_tail.childs[0];
                int paramsCount = pasing_params(args);
                if (paramsCount > func_sym->params_type.size())
                {
                    Debug::ParamsError(id, "many");
                    m_hasError = true;
                    return "";
                }
                else if (paramsCount < func_sym->params_type.size())
                {
                    Debug::ParamsError(id, "few");
                    m_hasError = true;
                    return "";
                }
                code.push_back({ Op_call_func, std::to_string(paramsCount), "", id.lexeme() });
                std::string return_value = new_temp();
                code.push_back({ Op_assign, "~ret", "", return_value });
                return return_value;
            }
        }
        /* <factor> -> num */
        else
        {
            const Token &num = node.tokens[0];
            // std::string temp = new_temp();
            // SymbolTable::GetInstance().insert(temp, ".global", new VarSymbol(num.type(), temp));
            // code.push_back({ Op_assign, num.lexeme(), "", temp });
            return num.lexeme();
        }
    }
    return "";
}

int TACGenerator::pasing_params(const Parser::TreeNode &node)
{
    /* <args> -> <expression> <arg_tail> | ~*/
    /* <arg_tail> -> , <expression> <arg_tail> | ~*/
    if (!node.childs.empty())
    {
        const Parser::TreeNode &expression = node.childs[0];
        const Parser::TreeNode &arg_tail = node.childs[1];
        int cnt = pasing_params(arg_tail);
        std::string p = do_expression(expression);
        code.push_back({ Op_param, "", "", p });
        return 1 + cnt;
    }
    return 0;
}
