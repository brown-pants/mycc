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
    /* <declaration> -> type <determine_pointer> */
    case Parser::declaration:
    {
        const Token &type = node.tokens[0];
        const Parser::TreeNode &determine_pointer = node.childs[0];
        bool isPointer;
        Token id;
        /* <determine_pointer> -> * id <dec_tail> */
        if (determine_pointer.tokens[0].type() == Token::Mult)
        {
            isPointer = true;
            id = determine_pointer.tokens[1];
        }
        /* <determine_pointer> -> id <dec_tail> */
        else
        {
            isPointer = false;
            id = determine_pointer.tokens[0];
        }
        const Parser::TreeNode &dec_tail = determine_pointer.childs[0];
        /* <dec_tail> -> ; */
        if (dec_tail.tokens[0].type() == Token::Semicolon)
        {
            // dec ptr
            if (isPointer)
            {
                dec_var(false, type, id, "ptr");
            }
            // dec var
            else
            {
                dec_var(false, type, id);
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
                break;
            }
            // dec arr
            const Token &num = dec_tail.tokens[1];
            dec_var(false, type, id, "arr", std::stoll(num.lexeme()));
        }
        /* ( <params> ) <compound_stmt> */
        else if (dec_tail.tokens[0].type() == Token::OpenParen)
        {
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
    /* <var_dec> -> type <var_determine_pointer> ; */
    case Parser::var_dec:
    {
        const Token &type = node.tokens[0];
        const Parser::TreeNode &var_determine_pointer = node.childs[0];
        bool isPointer;
        Token id;
        /* <var_determine_pointer> -> * id <var_dec_tail> */
        if (var_determine_pointer.tokens[0].type() == Token::Mult)
        {
            isPointer = true;
            id = var_determine_pointer.tokens[1];
        }
        /* <var_determine_pointer> -> id <var_dec_tail> */
        else
        {
            isPointer = false;
            id = var_determine_pointer.tokens[0];
        }
        const Parser::TreeNode &var_dec_tail = var_determine_pointer.childs[0];
        /* <var_dec_tail> -> ~ */
        if (var_dec_tail.tokens.empty())
        {
            // dec ptr
            if (isPointer)
            {
                dec_var(true, type, id, "ptr");
            }
            // dec var
            else
            {
                dec_var(true, type, id);
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
                break;
            }
            const Token &num = var_dec_tail.tokens[1];
            dec_var(true, type, id, "arr", std::stoll(num.lexeme()));
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

void TACGenerator::dec_var(bool local, const Token &type, const Token &id, const std::string &arr_ptr, int arrSize)
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
        std::string data_type = (type.type() == Token::DT_int ? "int" : "char");
        if (arr_ptr == "arr")
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new ArrSymbol(type.type(), arrSize, newName));
            code.push_back({ local ? Op_local_var : Op_global_var, std::to_string(dataSizeMap[type.type()] * arrSize), "arr_" + data_type, newName });
        }
        else if (arr_ptr == "ptr")
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new PtrSymbol(type.type(), newName));
            code.push_back({ local ? Op_local_var : Op_global_var, "8", "ptr_" + data_type, newName });
        }
        else
        {
            SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), new VarSymbol(type.type(), newName));
            code.push_back({ local ? Op_local_var : Op_global_var, std::to_string(dataSizeMap[type.type()]), "var_" + data_type, newName });
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
    /* <param> -> type <param_determine_pointer> */
    if (node.vn_type == Parser::param)
    {
        Token type = node.tokens[0];
        const Parser::TreeNode &param_determine_pointer = node.childs[0];
        bool isPointer;
        Token id;
        /* <param_determine_pointer> -> * id */
        if (param_determine_pointer.tokens[0].type() == Token::Mult)
        {
            isPointer = true;
            id = param_determine_pointer.tokens[1];
        }
        /* <param_determine_pointer> -> id */
        else
        {
            isPointer = false;
            id = param_determine_pointer.tokens[0];
        }
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
                return std::to_string(condition);
            }
            else
            {
                std::string temp3 = new_temp();
                code.push_back({ op, temp1, temp2, temp3 });
                return temp3;
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
                    code.push_back({ Op_add, ptr, std::to_string(offset), temp });
                }
                // idx is var
                else
                {
                    std::string offset_temp = new_temp();
                    code.push_back({ Op_mult, idx, std::to_string(stride), offset_temp });
                    code.push_back({ Op_add, ptr, offset_temp, temp });
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
            return do_id_tail(id_tail, id);
        }
        /* <factor> -> & id <id_tail> */
        else if (node.tokens[0].type() == Token::Ampersand)
        {
            const Token &ampersand = node.tokens[0];
            const Token &id = node.tokens[1];
            const Parser::TreeNode &id_tail = node.childs[0];
            return do_id_tail(id_tail, id, Token(), ampersand);
        }
        /* <factor> -> * <star_tail> */
        else if (node.tokens[0].type() == Token::Mult)
        {
            const Token &star = node.tokens[0];
            const Parser::TreeNode &star_tail = node.childs[0];
            /* <star_tail> -> ( <expression> ) <determine_assign> */
            if (star_tail.tokens[0].type() == Token::OpenParen)
            {
                const Parser::TreeNode &expression = star_tail.childs[0];
                const Parser::TreeNode &determine_assign = star_tail.childs[1];
                std::string left_value = do_expression(expression);
                if (left_value[0] == '&')
                {
                    std::string var = left_value.substr(1);
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
                        sym = SymbolTable::GetInstance().find(var.substr(0, idx), scope);
                    }
                    if (sym->type == Symbol::Arr)
                    {
                        left_value = left_value.substr(1) + "[0]";
                    }
                    else
                    {
                        left_value = var;
                    }
                }
                else if (isNumber(left_value))
                {
                    Debug::InvalidOperands(star);
                    m_hasError = true;
                    return "";
                }
                else
                {
                    left_value = "*" + left_value;
                }
                /* <determine_assign> -> = <expression> */
                if (!determine_assign.tokens.empty())
                {
                    const Parser::TreeNode &expression = determine_assign.childs[0];
                    std::string right_value = do_expression(expression);
                    code.push_back({ Op_assign, right_value, "", left_value });
                }
                return left_value;
            }
            /* <star_tail> -> id <id_tail> */
            else if (star_tail.tokens[0].type() == Token::Identifier)
            {
                const Token &id = star_tail.tokens[0];
                const Parser::TreeNode &id_tail = star_tail.childs[0];
                return do_id_tail(id_tail, id, star);
            }
            /* <star_tail> -> & id <id_tail> */
            else if (star_tail.tokens[0].type() == Token::Ampersand)
            {
                const Token &ampersand = star_tail.tokens[0];
                const Token &id = star_tail.tokens[1];
                const Parser::TreeNode &id_tail = star_tail.childs[0];
                return do_id_tail(id_tail, id, star, ampersand);
            }
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

std::string TACGenerator::do_id_tail(const Parser::TreeNode &id_tail, const Token &id, const Token &star, const Token &ampersand)
{
    bool hasStar = (star.type() == Token::Mult);
    bool hasAmpersand = (ampersand.type() == Token::Ampersand);

    Symbol *id_sym = getSymbol(id.lexeme());

    if (id_sym == nullptr)
    {
        Debug::VarUndefined(id);
        m_hasError = true;
        return "";
    }
    if (hasStar && !hasAmpersand && id_sym->type == Symbol::Var)
    {
        Debug::InvalidOperands(star);
        m_hasError = true;
        return "";
    }
    if (hasAmpersand && (id_sym->type == Symbol::Ptr || id_sym->type == Symbol::Func))
    {
        Debug::DereferencingError(id, id_sym->type == Symbol::Ptr ? "pointer" : "function");
        m_hasError = true;
        return "";
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
        if (hasAmpersand && id_sym->type == Symbol::Arr)
        {
            Debug::DereferencingError(id, "array");
            m_hasError = true;
            return "";
        }

        std::string newName;
        
        if (id_sym->type == Symbol::Var)
        {
            newName = static_cast<VarSymbol *>(id_sym)->newName;
        }
        else if (id_sym->type == Symbol::Ptr)
        {
            newName = static_cast<PtrSymbol *>(id_sym)->newName;
        }
        else if (id_sym->type == Symbol::Arr)
        {
            newName = static_cast<ArrSymbol *>(id_sym)->newName;
        }

        if (hasStar && !hasAmpersand)
        {
            if (id_sym->type == Symbol::Arr)
            {
                return newName + "[0]";
            }
            return star.lexeme() + newName;
        }
        else if (hasAmpersand && !hasStar)
        {
            return ampersand.lexeme() + newName;
        }
        else if (id_sym->type == Symbol::Arr)
        {
            return "&" + newName;
        }
        else
        {
            return newName;
        }
    }
    /* <id_tail> -> [ <expression> ] <determine_assign> */
    else if (id_tail.tokens[0].type() == Token::OpenSquare)
    {
        if (id_sym->type != Symbol::Arr && id_sym->type != Symbol::Ptr)
        {
            Debug::NotArray(id);
            m_hasError = true;
            return "";
        }
        if (hasStar && !hasAmpersand)
        {
            Debug::InvalidOperands(star);
            m_hasError = true;
            return "";
        }

        const Parser::TreeNode &expression = id_tail.childs[0];
        const Parser::TreeNode &determine_assign = id_tail.childs[1];
        std::string idx = do_expression(expression);

        if (hasAmpersand && !hasStar)
        {
            if (!determine_assign.tokens.empty())
            {
                Debug::AssignToErrorType(id, "array");
                m_hasError = true;
                return "";
            }
            std::string temp = new_temp();
            Token::Type type = static_cast<ArrSymbol *>(id_sym)->data_type;
            unsigned int stride = dataSizeMap[type];
            pointerTemps.insert(std::pair<std::string, unsigned int>(temp, stride));
            if (isNumber(idx))
            {
                long long offset = std::stoll(idx) * stride;
                code.push_back({ Op_add, "&" + id.lexeme(), std::to_string(offset), temp });
            }
            else
            {
                std::string offset_temp = new_temp();
                code.push_back({ Op_mult, idx, std::to_string(stride), offset_temp });
                code.push_back({ Op_add, "&" + id.lexeme(), offset_temp, temp });
            }
            return temp;
        }
        Token::Type data_type;
        std::string newName;
        if (id_sym->type == Symbol::Ptr)
        {
            PtrSymbol *ptr_sym = static_cast<PtrSymbol *>(id_sym);
            data_type = ptr_sym->data_type;
            newName = ptr_sym->newName;
        }
        else
        {
            ArrSymbol *arr_sym = static_cast<ArrSymbol *>(id_sym);
            data_type = arr_sym->data_type;
            newName = arr_sym->newName;
        }
        std::string result;

        // idx is a num
        if (isNumber(idx))
        {
            long long offset = std::stoll(idx) * dataSizeMap[data_type];
            if (id_sym->type == Symbol::Ptr)
            {
                std::string ptr_temp = new_temp();
                pointerTemps.insert(std::pair<std::string, unsigned int>(ptr_temp, dataSizeMap[data_type]));
                code.push_back({ Op_add, newName, std::to_string(offset), ptr_temp });
                result = "*" + ptr_temp;
            }
            else
            {
                result = newName + '[' + std::to_string(offset) + ']';
            }
        }
        // idx is a var
        else
        {
            std::string offset = new_temp();
            code.push_back({ Op_mult, idx, std::to_string(dataSizeMap[data_type]), offset });
            if (id_sym->type == Symbol::Ptr)
            {
                std::string ptr_temp = new_temp();
                pointerTemps.insert(std::pair<std::string, unsigned int>(ptr_temp, dataSizeMap[data_type]));
                code.push_back({ Op_add, newName, offset, ptr_temp });
                result = "*" + ptr_temp;
            }
            else
            {
                result = newName + '[' + offset + ']';
            }
        }
        
        /* <determine_assign> -> ~ */
        if (determine_assign.childs.empty())
        {
            return result;
        }
        /* <determine_assign> -> = <expression> */
        else
        {
            const Parser::TreeNode &expression = determine_assign.childs[0];
            std::string temp = do_expression(expression);
            code.push_back({ Op_assign, temp, "", result });
            return result;
        }
    }
    /* <id_tail> -> = <expression> */
    else if (id_tail.tokens[0].type() == Token::Eq)
    {
        // assignment to expression with error type
        if (!hasStar && id_sym->type == Symbol::Arr)
        {
            Debug::AssignToErrorType(id, "array");
            m_hasError = true;
            return "";
        }
        if (hasAmpersand && id_sym->type == Symbol::Arr)
        {
            Debug::DereferencingError(id, "array");
            m_hasError = true;
            return "";
        }
        if (id_sym->type == Symbol::Func)
        {
            Debug::AssignToErrorType(id, "function");
            m_hasError = true;
            return "";
        }
        if (!hasStar && hasAmpersand)
        {
            Debug::InvalidOperands(ampersand);
            m_hasError = true;
            return "";
        }
        std::string newName;
        if (id_sym->type == Symbol::Var)
        {
            newName = static_cast<VarSymbol *>(id_sym)->newName;
        }
        else if (id_sym->type == Symbol::Ptr)
        {
            newName = static_cast<PtrSymbol *>(id_sym)->newName;
        }
        else if (id_sym->type == Symbol::Arr)
        {
            newName = static_cast<ArrSymbol *>(id_sym)->newName;
        }
        const Parser::TreeNode &expression = id_tail.childs[0];
        std::string temp = do_expression(expression);
        std::string left_value;
        if (hasStar && !hasAmpersand)
        {
            if (id_sym->type == Symbol::Arr)
            {
                left_value = newName + "[0]";
            }
            else
            {
                left_value = star.lexeme() + newName;
            }
        }
        else
        {
            left_value = newName;
        }
        code.push_back({ Op_assign, temp, "", left_value });
        return left_value;
    }
    /* <id_tail> -> ( <args> ) <determine_assign> */
    else if (id_tail.tokens[0].type() == Token::OpenParen)
    {
        if (id_sym->type != Symbol::Func)
        {
            Debug::NotFunction(id);
            m_hasError = true;
            return "";
        }
        FunctionSymbol *func_sym = static_cast<FunctionSymbol *>(id_sym);
        if (hasStar && !func_sym->return_pointer)
        {
            Debug::InvalidOperands(star);
            m_hasError = true;
            return "";
        }
        const Parser::TreeNode &args = id_tail.childs[0];
        const Parser::TreeNode &determine_assign = id_tail.childs[1];
        /* <determine_assign> -> = <expression> */
        if (!hasStar && !determine_assign.childs.empty())
        {
            Debug::InvalidOperands(determine_assign.tokens[0]);
            m_hasError = true;
            return "";
        }
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
        if (!func_sym->return_pointer && hasStar)
        {
            Debug::InvalidOperands(star);
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
            return "";
        }
        std::string return_value = new_temp();
        if (hasStar)
        {
            unsigned int stride = dataSizeMap[func_sym->return_type];
            pointerTemps.insert(std::pair<std::string, unsigned int>(return_value, stride));
        }
        code.push_back({ Op_assign, "~ret", "", return_value });
        /* <determine_assign> -> = <expression> */
        if (!determine_assign.childs.empty())
        {
            const Parser::TreeNode &expression = determine_assign.childs[0];
            std::string right_value = do_expression(expression);
            code.push_back({ Op_assign, right_value, "", star.lexeme() + return_value });
        }
        return hasStar ? star.lexeme() + return_value : return_value;
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
    // num or *ptr
    if (isNumber(str) || str[0] == '*')
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