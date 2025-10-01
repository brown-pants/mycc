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
    SymbolTable::GetInstance().insert("mycc_putchar", ".global", new FunctionSymbol(Token::Void, false, std::vector<Token::Type>{ Token::DT_int }));
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
    /* <declaration> -> type <is_pointer> id <dec_tail> */
    case Parser::declaration:
    {
        const Token &type = node.tokens[0];
        const Token &id = node.tokens[1];
        const Parser::TreeNode &is_pointer = node.childs[0];
        const Parser::TreeNode &dec_tail = node.childs[1];
        /* <is_pointer> -> * | ~ */
        bool isPointer = (is_pointer.tokens.empty() ? false : true);
        /* <dec_tail> -> ; */
        if (dec_tail.tokens[0].type() == Token::Semicolon)
        {
            // dec ptr
            if (isPointer)
            {
                dec_var(false, type, id, "", "ptr");
            }
            // dec var
            else
            {
                dec_var(false, type, id, "");
            }
        }
        /* <dec_tail> -> = <expression> ; */
        else if (dec_tail.tokens[0].type() == Token::Eq)
        {
            std::string value = "";
            const Parser::TreeNode &expression = dec_tail.childs[0];
            value = do_expression(expression);
            if (!isNumber(value))
            {
                Debug::InitialNotConstant(dec_tail.tokens[0]);
                m_hasError = true;
            }
             // dec ptr
            if (isPointer)
            {
                dec_var(false, type, id, value, "ptr");
            }
            // dec var
            else
            {
                dec_var(false, type, id, value);
            }
        }
        /* <dec_tail> -> [ num ] ; */
        else if (dec_tail.tokens[0].type() == Token::OpenSquare)
        {
            if (isPointer)
            {
                // Sorry, pointer arrays are not currently supported.
                m_hasError = true;
                Debug::NotSupportedPointerArray(id);
            }
            // dec arr
            const Token &num = dec_tail.tokens[1];
            dec_var(false, type, id, "", "arr", std::stoll(num.lexeme()));
        }
        /* ( <params> ) <compound_stmt> */
        else if (dec_tail.tokens[0].type() == Token::OpenParen)
        {
            if (type.type() == Token::Void && isPointer)
            {
                Token ptrType(type.type(), type.lexeme() + "*", type.line());
                Debug::TypeError(ptrType);
                m_hasError = true;
            }
            // dec func
            dec_function(type, isPointer, id, dec_tail);
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
    /* <var_dec> -> type <is_pointer> id <var_dec_tail> ; */
    case Parser::var_dec:
    {
        const Token &type = node.tokens[0];
        const Token &id = node.tokens[1];
        const Parser::TreeNode &is_pointer = node.childs[0];
        const Parser::TreeNode &var_dec_tail = node.childs[1];
        /* <is_pointer> -> * | ~ */
        bool isPointer = (is_pointer.tokens.empty() ? false : true);
        /* <var_dec_tail> -> ~ */
        if (var_dec_tail.tokens.empty())
        {
            // dec ptr
            if (isPointer)
            {
                dec_var(true, type, id, "", "ptr");
            }
            // dec var
            else
            {
                dec_var(true, type, id, "");
            }
        }
        /* <var_dec_tail> -> = <expression> */
        else if (var_dec_tail.tokens[0].type() == Token::Eq)
        {
            const Parser::TreeNode &expression = var_dec_tail.childs[0];
            std::string value = do_expression(expression);
            // dec ptr
            if (isPointer)
            {
                dec_var(true, type, id, value, "ptr");
            }
            // dec var
            else
            {
                dec_var(true, type, id, value);
            }
        }
        /* <var_dec_tail> -> [ num ] */
        else
        {
            if (isPointer)
            {
                // Sorry, pointer arrays are not currently supported.
                m_hasError = true;
                Debug::NotSupportedPointerArray(id);
            }
            const Token &num = var_dec_tail.tokens[1];
            dec_var(true, type, id, "", "arr", std::stoll(num.lexeme()));
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

void TACGenerator::dec_var(bool local, const Token &type, const Token &id, const std::string &value, const std::string &arr_ptr, int arrSize)
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
            const std::string &suffix = scopeStack.top();
            newName += "~" + suffix.substr(suffix.find('~') + 1);
        }
        std::string data_type = (type.type() == Token::DT_int ? "int" : "char");
        // array
        if (arr_ptr == "arr")
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new ArrSymbol(type.type(), arrSize, newName));
            code.push_back({ local ? Op_local_var : Op_global_var, std::to_string(dataSizeMap[type.type()] * arrSize), "arr_" + data_type, newName });
        }
        // ptr or var
        else
        {
            std::string size;
            std::string sym_type;
            if (arr_ptr == "ptr")
            {
                SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new PtrSymbol(type.type(), newName));
                sym_type = "ptr_" + data_type;
                size = "8";
            }
            else
            {
                SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new VarSymbol(type.type(), newName));
                sym_type = "var_" + data_type;
                size = std::to_string(dataSizeMap[type.type()]);
            }
            if (local)
            {
                code.push_back({ Op_local_var, size, sym_type, newName });
                if (value != "")
                {
                    code.push_back({ Op_assign, value, "", newName});
                }
            }
            else if (value != "")
            {
                code.push_back({ Op_global_init, value, sym_type, newName });
            }
            else
            {
                code.push_back({ Op_global_var, size, sym_type, newName });
            }
        }
    }
}

void TACGenerator::dec_function(const Token &type, bool returnPointer, const Token &id, const Parser::TreeNode &dec_tail)
{
    curFunName = id.lexeme();

    code.push_back({ Op_begin_func, "", "", curFunName });
    scopeStack.push(curFunName + "~1"); // push scope '{func name}~1'

    scope_counter = 1;

    std::vector<Token::Type> params_type;
    dec_params(dec_tail.childs[0], params_type); // <params>
    SymbolTable::GetInstance().insert(curFunName, ".global", new FunctionSymbol(type.type(), returnPointer, params_type));

    scope_counter = 0;
    generate_3ac(dec_tail.childs[1]);   // <compound_stmt>
    
    code.push_back({ Op_label, "", "", curFunName + "_exit" }); // func_exit:

    code.push_back({ Op_end_func, "", "", curFunName });
}

void TACGenerator::dec_params(const Parser::TreeNode &node, std::vector<Token::Type> &params_type)
{
    /* <param> -> type <is_pointer> id */
    if (node.vn_type == Parser::param)
    {
        const Token &type = node.tokens[0];
        const Token &id = node.tokens[1];
        const Parser::TreeNode &is_pointer = node.childs[0];
        /* <is_pointer> -> * | ~ */
        bool isPointer = (is_pointer.tokens.empty() ? false : true);

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
            Symbol *symbol = nullptr;
            if (isPointer)
            {
                symbol = new PtrSymbol(type.type(), newName);
            }
            else
            {
                symbol = new VarSymbol(type.type(), newName);
            }
            SymbolTable::GetInstance().insert(varName, scopeStack.top(), symbol);
            std::string data_type;
            if (type.type() == Token::DT_char)
            {
                data_type = "char";
            }
            else
            {
                data_type = "int";
            }
            if (isPointer)
            {
                data_type = "ptr_" + data_type;
            }
            else
            {
                data_type = "var_" + data_type;
            }
            code.push_back({ Op_dec_param, data_type, "", newName });
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
    /* <expression> -> <relational_expression> <expression_tail> */
    if (node.vn_type == Parser::expression)
    {
        const Parser::TreeNode &relational_expression = node.childs[0];
        const Parser::TreeNode *expression_tail = &node.childs[1];
        std::string left_value = do_expression(relational_expression);
        std::stack<std::string> assign_stack;
        assign_stack.push(left_value);
        /* <expression_tail> -> = <relational_expression> <expression_tail> | ~ */
        const Token &eq_token = expression_tail->tokens[0];
        while (!expression_tail->childs.empty())
        {
            const Parser::TreeNode &relational_expression = expression_tail->childs[0];
            assign_stack.push(do_expression(relational_expression));
            expression_tail = &expression_tail->childs[1];
        }
        while (assign_stack.size() > 1)
        {
            std::string right_value = assign_stack.top();
            assign_stack.pop();
            left_value = assign_stack.top();
            assign_stack.pop();
            if (left_value == "")
            {
                return "";
            }
            if (left_value[0] == '&'|| isNumber(left_value) || left_value.substr(0, 2) == "t^")
            {
                Debug::InvalidOperands(eq_token);
                m_hasError = true;
                return "";
            }
            code.push_back({ Op_assign, right_value, "", left_value});
            assign_stack.push(left_value);
        }
        return left_value;
    }
    /* <relational_expression> -> <additive_expression> <relational_expression_tail> */
    else if (node.vn_type == Parser::relational_expression)
    {
        const Parser::TreeNode &additive_expression = node.childs[0];
        const Parser::TreeNode *relational_expression_tail = &node.childs[1];
        std::string temp1 = do_expression(additive_expression);
        /* <relational_expression_tail> -> <relop> <additive_expression> <relational_expression_tail> | ~ */
        while (!relational_expression_tail->childs.empty())
        {
            const Parser::TreeNode &relop = relational_expression_tail->childs[0];
            const Parser::TreeNode &additive_expression = relational_expression_tail->childs[1];
            relational_expression_tail = &relational_expression_tail->childs[2];
            std::string temp2 = do_expression(additive_expression);

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

            // Both of arg1 and arg2 is number
            if (isNumber(temp1) && isNumber(temp2))
            {
                int condition;
                long long num1 = std::stoll(temp1);
                long long num2 = std::stoll(temp2);
                if (op == Op_eq_eq)
                {
                    condition = num1 == num2;
                }
                else if (op == Op_not_eq)
                {
                    condition = num1 != num2;
                }
                else if (op == Op_greater_eq)
                {
                    condition = num1 >= num2;
                }
                else if (op == Op_greater)
                {
                    condition = num1 > num2;
                }
                else if (op == Op_less_eq)
                {
                    condition = num1 <= num2;
                }
                else if (op == Op_less)
                {
                    condition = num1 < num2;
                }
                temp1 = std::to_string(condition);
            }
            else
            {
                std::string temp3 = new_temp();
                code.push_back({ op, temp1, temp2, temp3 });
                temp1 = temp3;
            }
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

            unsigned int pointerStride_1 = getPointerStride(temp1);
            unsigned int pointerStride_2 = getPointerStride(temp2);
            // Both of arg1 and arg2 is number
            if (isNumber(temp1) && isNumber(temp2))
            {
                long long num;
                long long num1 = std::stoll(temp1);
                long long num2 = std::stoll(temp2);
                if (op == Op_add)
                {
                    num = num1 + num2;
                }
                else if (op == Op_sub)
                {
                    num = num1 - num2;
                }
                temp1 = std::to_string(num);
            }
            // Both of arg1 and arg2 is pointer
            else if (pointerStride_1 && pointerStride_2)
            {
                Debug::InvalidOperands(addop.tokens[0]);
                m_hasError = true;
            }
            // var - pointer
            else if (op == Op_sub && !pointerStride_1 && pointerStride_2)
            {
                Debug::InvalidOperands(addop.tokens[0]);
                m_hasError = true;
            }
            // Either arg1 or agr2 is pointer
            else if (pointerStride_1 || pointerStride_2)
            {
                unsigned int stride;
                std::string ptr;
                std::string idx;
                // arg1 is pointer
                if (pointerStride_1)
                {
                    stride = pointerStride_1;
                    ptr = temp1;
                    idx = temp2;
                }
                // arg2 is pointer
                else
                {
                    stride = pointerStride_2;
                    ptr = temp2;
                    idx = temp1;
                }
                std::string temp = new_temp();
                pointerTemps.insert(std::pair<std::string, unsigned int>(temp, stride));
                // idx is num
                if (isNumber(idx))
                {
                    long long offset = std::stoll(idx) * stride;
                    code.push_back({ op, ptr, std::to_string(offset), temp });
                }
                // idx is var
                else
                {
                    std::string offset_temp = new_temp();
                    code.push_back({ Op_mult, idx, std::to_string(stride), offset_temp });
                    code.push_back({ op, ptr, offset_temp, temp });
                }
                temp1 = temp;
            }
            // Neither arg1 nor agr2 is pointer
            else
            {
                std::string temp3 = new_temp();
                code.push_back({ op, temp1, temp2, temp3 });
                temp1 = temp3;
            }
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

            /* <mulop> -> * | / | % */
            OpType op;
            if (mulop.tokens[0].type() == Token::Mult)
            {
                op = Op_mult;
            }
            else if (mulop.tokens[0].type() == Token::Div)
            {
                op = Op_div;
            }
            else if (mulop.tokens[0].type() == Token::Mod)
            {
                op = Op_mod;
            }

            // Both of arg1 and arg2 is number
            if (isNumber(temp1) && isNumber(temp2))
            {
                long long num;
                long long num1 = std::stoll(temp1);
                long long num2 = std::stoll(temp2);
                if (op == Op_mult)
                {
                    num = num1 * num2;
                }
                else if (op == Op_div)
                {
                    num = num1 / num2;
                }
                else if (op == Op_mod)
                {
                    num = num1 % num2;
                }
                temp1 = std::to_string(num);
            }
            // At least one of temp1 and temp2 is a pointer.
            else if (getPointerStride(temp1) || getPointerStride(temp2))
            {
                Debug::InvalidOperands(mulop.tokens[0]);
                m_hasError = true;
            }
            else
            {
                std::string temp3 = new_temp();
                code.push_back({ op, temp1, temp2, temp3 });
                temp1 = temp3;
            }
        }
        return temp1;
    }
    /* <factor> -> ( <expression> ) | id <id_tail> | & <factor> | * <factor> | num */
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
            return do_id_tail(id_tail, id);
        }
        /* <factor> -> & <factor> */
        else if (node.tokens[0].type() == Token::Ampersand)
        {
            const Token &ampersand = node.tokens[0];
            const Parser::TreeNode &factor = node.childs[0];
            std::string addr = do_expression(factor);
            if (isNumber(addr) || getPointerStride(addr) != 0 || addr[0] == '&' || addr.substr(0, 2) == "t^")
            {
                Debug::InvalidOperands(ampersand);
                m_hasError = true;
                return "";
            }
            else if (addr[0] == '*')
            {
                return addr.substr(1);
            }
            else
            {
                return ampersand.lexeme() + addr;
            }
        }
        /* <factor> -> * <factor> */
        else if (node.tokens[0].type() == Token::Mult)
        {
            const Token &star = node.tokens[0];
            const Parser::TreeNode &factor = node.childs[0];
            std::string addr = do_expression(factor);
            if (isNumber(addr) || getPointerStride(addr) == 0)
            {
                Debug::InvalidOperands(star);
                m_hasError = true;
                return "";
            }
            if (addr[0] == '&')
            {
                return addr.substr(1);
            }
            else
            {
                return star.lexeme() + addr;
            }
        }
        /* <factor> -> ! <factor> */
        else if (node.tokens[0].type() == Token::Not)
        {
            const Parser::TreeNode &factor = node.childs[0];
            std::string var = do_expression(factor);
            if (isNumber(var))
            {
                long long result = !std::stoll(var);
                return std::to_string(result);
            }
            std::string temp = new_temp();
            code.push_back({ Op_not, var, "", temp });
            return temp;
        }
        /* <factor> -> - <factor> */
        else if (node.tokens[0].type() == Token::Minus)
        {
            const Parser::TreeNode &factor = node.childs[0];
            std::string var = do_expression(factor);
            if (getPointerStride(var) != 0)
            {
                Debug::InvalidOperands(node.tokens[0]);
                m_hasError = true;
            }
            if (isNumber(var))
            {
                long long result = -std::stoll(var);
                return std::to_string(result);
            }
            std::string temp = new_temp();
            code.push_back({ Op_neg, var, "", temp });
            return temp;
        }
        /* <factor> -> num */
        else
        {
            const Token &num = node.tokens[0];
            if (num.lexeme()[0] == '\'')
            {
                return std::to_string(num.lexeme()[1]);
            }
            return num.lexeme();
        }
    }
    return "";
}

std::string TACGenerator::do_id_tail(const Parser::TreeNode &id_tail, const Token &id)
{
    Symbol *id_sym = getSymbol(id.lexeme());

    if (id_sym == nullptr)
    {
        Debug::VarUndefined(id);
        m_hasError = true;
        return "";
    }
    
    std::string newName;
    Token::Type data_type;
    
    if (id_sym->type == Symbol::Var)
    {
        VarSymbol *sym = static_cast<VarSymbol *>(id_sym);
        newName = sym->newName;
        data_type = sym->data_type;
    }
    else if (id_sym->type == Symbol::Ptr)
    {
        PtrSymbol *sym = static_cast<PtrSymbol *>(id_sym);
        newName = sym->newName;
        data_type = sym->data_type;
    }
    else if (id_sym->type == Symbol::Arr)
    {
        ArrSymbol *sym = static_cast<ArrSymbol *>(id_sym);
        newName = sym->newName;
        data_type = sym->data_type;
    }

    /* <id_tail> -> ~ */
    if (id_tail.childs.empty())
    {
        if (id_sym->type == Symbol::Func)
        {
            Debug::NotNormalVar(id);
            m_hasError = true;
            return "";
        }
        if (id_sym->type == Symbol::Arr)
        {
            return '&' + newName;
        }
        return newName;
    }
    /* <id_tail> -> [ <expression> ] */
    else if (id_tail.tokens[0].type() == Token::OpenSquare)
    {
        if (id_sym->type != Symbol::Arr && id_sym->type != Symbol::Ptr)
        {
            Debug::NotArray(id);
            m_hasError = true;
            return "";
        }

        const Parser::TreeNode &expression = id_tail.childs[0];
        unsigned int data_size = dataSizeMap[data_type];
        std::string idx = do_expression(expression);
        std::string offset_str;

        // idx is a num
        if (isNumber(idx))
        {
            long long offset = std::stoll(idx) * data_size;
            offset_str = std::to_string(offset);
        }
        // idx is a var
        else
        {
            offset_str = new_temp();
            code.push_back({ Op_mult, idx, std::to_string(data_size), offset_str });
        }
        
        std::string prefix = (id_sym->type == Symbol::Arr ? "&" : "");
        std::string ptr_temp = new_temp();
        pointerTemps.insert(std::pair<std::string, unsigned int>(ptr_temp, data_size));
        code.push_back({ Op_add, prefix + newName, offset_str, ptr_temp });
        return "*" + ptr_temp;
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
        int paramsCount = passing_params(args);
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
        std::string returnType = std::string(func_sym->return_pointer ? "ptr_" : "var_") + (func_sym->return_type == Token::DT_char ? "char" : "int");
        if (func_sym->return_type == Token::Void)
        {
            returnType = "void";
        }
        code.push_back({ Op_call_func, std::to_string(paramsCount), returnType, id.lexeme() });
        if (func_sym->return_type == Token::Void)
        {
            return "0";
        }
        std::string return_value = new_temp();
        if (func_sym->return_pointer)
        {
            unsigned int stride = dataSizeMap[func_sym->return_type];
            pointerTemps.insert(std::pair<std::string, unsigned int>(return_value, stride));
        }
        code.push_back({ Op_assign, "~ret", "", return_value });
        return return_value;
    }
    return "";
}

int TACGenerator::passing_params(const Parser::TreeNode &node)
{
    /* <args> -> <expression> <arg_tail> | ~*/
    /* <arg_tail> -> , <expression> <arg_tail> | ~*/
    if (!node.childs.empty())
    {
        const Parser::TreeNode &expression = node.childs[0];
        const Parser::TreeNode &arg_tail = node.childs[1];
        int cnt = passing_params(arg_tail);
        std::string p = do_expression(expression);
        code.push_back({ Op_param, "", "", p });
        return 1 + cnt;
    }
    return 0;
}

bool TACGenerator::isNumber(const std::string &str) const
{
    return str[0] == '-' || std::isdigit(str[0]);
}

unsigned int TACGenerator::getPointerStride(const std::string &str) const
{
    // num or *ptr or null
    if (isNumber(str) || str[0] == '*' || str == "")
    {
        return 0;
    }

    auto iter = pointerTemps.find(str);
    // pointer temp
    if (iter != pointerTemps.end())
    {
        return iter->second;
    }

    bool sign = false;
    std::string var = str;
    if (str[0] == '&')
    {
        sign = true;
        var = str.substr(1);
    }

    int idx = var.find('~');
    Symbol *sym;
    // global var
    if (idx == std::string::npos)
    {
        sym = SymbolTable::GetInstance().find(var, ".global");
    }
    // local var
    else
    {
        std::string id = var.substr(idx);
        std::string scope = curFunName + id;
        var = var.substr(0, idx);
        sym = SymbolTable::GetInstance().find(var, scope);
    }
    // int temp
    if (sym == nullptr)
    {
        return 0;
    }
    // &var
    if (sign)
    {
        Token::Type type = static_cast<VarSymbol *>(sym)->data_type;
        return dataSizeMap.find(type)->second;
    }
    // pointer
    else if (sym->type == Symbol::Ptr)
    {
        Token::Type type = static_cast<PtrSymbol *>(sym)->data_type;
        return dataSizeMap.find(type)->second;
    }
    // array
    else if (sym->type == Symbol::Arr)
    {
        Token::Type type = static_cast<ArrSymbol *>(sym)->data_type;
        return dataSizeMap.find(type)->second;
    }
    // var
    return 0;
}