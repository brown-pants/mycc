#include "TACGenerator.h"
#include "../../symtab/SymbolTable.h"
#include "../../debug/Debug.h"

TACGenerator::TACGenerator(const Parser::TreeNode &root)
    : root(root), temp_counter(0U), label_counter(0U), str_counter(0U), scope_counter(0U), m_hasError(false),
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

std::string TACGenerator::new_label()
{
    return std::string(".lable") + std::to_string(label_counter ++);
}

std::string TACGenerator::new_string()
{
    return std::string(".str") + std::to_string(str_counter ++);
}

std::string TACGenerator::getStrName(const std::string &str)
{
    auto iter = strNames.find(str);
    if (iter == strNames.end())
    {
        std::string str_name = new_string();
        strNames.insert(std::pair<std::string, std::string>(str, str_name));
        code.insert(code.begin(), { Op_dec_string, str, "", str_name });
        return str_name;
    }
    return iter->second;
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
    /* <declaration> -> <data_type> <is_pointer> id <dec_tail> | extern <data_type> <is_pointer> id <dec_tail> */
    case Parser::declaration:
    {
        bool has_extern = false;
        if (node.tokens[0].type() == Token::Extern)
        {
            has_extern = true;
        }
        const Parser::TreeNode &data_type = node.childs[0];
        const Parser::TreeNode &is_pointer = node.childs[1];
        const Parser::TreeNode &dec_tail = node.childs[2];
        const Token &id = node.tokens[0 + has_extern];
        const Token &type = data_type.tokens[0];
        /* <is_pointer> -> * | ~ */
        bool isPointer = (is_pointer.tokens.empty() ? false : true);
        /* <dec_tail> -> ; */
        if (dec_tail.tokens[0].type() == Token::Semicolon)
        {
            // extern var;
            if (has_extern)
            {
                Symbol *sym = SymbolTable::GetInstance().find(id.lexeme(), scopeStack.top());
                std::string data_type;
                if (sym != nullptr)
                {
                    Debug::Redeclare(id);
                    m_hasError = true;
                    break;
                }
                if (isPointer)
                {
                    sym = new PtrSymbol(type.type(), id.lexeme());
                    data_type = "ptr_";
                }
                else
                {
                    sym = new VarSymbol(type.type(), id.lexeme());
                    data_type = "var_";
                }
                SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), sym);
                data_type += (type.type() == Token::DT_int ? "int" : "char");
                code.push_back({ Op_extern_var, data_type, "", id.lexeme() });
            }
            // dec ptr
            else if (isPointer)
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
            // extern var = ...;
            if (has_extern)
            {
                Debug::InvalidOperands(dec_tail.tokens[0]);
                m_hasError = true;
            }
            std::string value = "";
            const Parser::TreeNode &expression = dec_tail.childs[0];
            value = do_expression(expression);
            if (!isNumber(value) && value[0] != '&')
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
        /* <dec_tail> -> [ <expression> ] ; */
        else if (dec_tail.tokens[0].type() == Token::OpenSquare)
        {
            const Parser::TreeNode &expression = dec_tail.childs[0];
            std::string num = do_expression(expression);
            if (!isNumber(num))
            {
                Debug::NotConstant(id);
                m_hasError = true;
                num = "0";
            }
            int size = std::stoll(num);
            // extern arr[N];
            if (has_extern)
            {
                Symbol *sym = SymbolTable::GetInstance().find(id.lexeme(), scopeStack.top());
                std::string data_type = "arr_";
                if (sym != nullptr)
                {
                    Debug::Redeclare(id);
                    m_hasError = true;
                    break;
                }
                if (isPointer)
                {
                    // Sorry, pointer arrays are not currently supported.
                    m_hasError = true;
                    Debug::NotSupportedPointerArray(id);
                }
                else
                {
                    sym = new ArrSymbol(type.type(), size, id.lexeme());
                    data_type += (type.type() == Token::DT_int ? "int" : "char");
                    code.push_back({ Op_extern_var, data_type, "", id.lexeme() });
                    SymbolTable::GetInstance().insert(id.lexeme(), scopeStack.top(), sym);
                    return;
                }
            }
            else if (isPointer)
            {
                // Sorry, pointer arrays are not currently supported.
                m_hasError = true;
                Debug::NotSupportedPointerArray(id);
            }
            // dec arr
            dec_var(false, type, id, "", "arr", size);
        }
        /* ( <params> ) <func_tail> */
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
    /* <var_dec> -> <data_type> <is_pointer> id <var_dec_tail> ; */
    case Parser::var_dec:
    {
        const Parser::TreeNode &data_type = node.childs[0];
        const Parser::TreeNode &is_pointer = node.childs[1];
        const Parser::TreeNode &var_dec_tail = node.childs[2];
        const Token &id = node.tokens[0];
        const Token &type = data_type.tokens[0];
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
        /* <var_dec_tail> -> [ <expression> ] */
        else
        {
            const Parser::TreeNode &expression = var_dec_tail.childs[0];
            std::string num = do_expression(expression);
            if (!isNumber(num))
            {
                Debug::NotConstant(id);
                m_hasError = true;
                num = "0";
            }
            if (isPointer)
            {
                // Sorry, pointer arrays are not currently supported.
                m_hasError = true;
                Debug::NotSupportedPointerArray(id);
            }
            dec_var(true, type, id, "", "arr", std::stoll(num));
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
        if (isNumber(condition))
        {
            if (std::stoll(condition))
            {
                generate_3ac(statement);
            }
            else if (else_statement)
            {
                generate_3ac(*else_statement);
            }
            return;
        }
        std::string label1 = new_label();
        std::string label2 = new_label();
        code.push_back({ Op_if, condition, "", label1});    // if (contidion) goto L1
        code.push_back({ Op_goto, "", "", label2});         // goto L2
        code.push_back({ Op_label, "", "", label1});        // L1:
        generate_3ac(statement);                            // statements
        if (else_statement)
        {// else
            std::string label3 = new_label();
            code.push_back({ Op_goto, "", "", label3});     // goto L3
            code.push_back({ Op_label, "", "", label2});    // L2:
            generate_3ac(*else_statement);                  // else_statement
            code.push_back({ Op_label, "", "", label3});    // L3:
        }
        else
        {// no else part
            code.push_back({ Op_label, "", "", label2});    // L2:
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
        if (is_for)
        {
            do_expression(node.childs[0]);
        }
        std::string label1 = new_label();
        code.push_back({ Op_label, "", "", label1 });           // L1:
        int L1_row = code.size() - 1;
        std::string condition = do_expression(*condition_node);
        // condition == 0
        if (isNumber(condition) && !std::stoll(condition))
        {
            label_counter --;
            code.erase(code.begin() + L1_row);                  // erase L1
            break;
        }
        std::string label3 = new_label();
        std::string label4;
        breakStack.push(label3);
        if (is_for)
        {
            label4 = new_label();
            continueStack.push(label4);
        }
        else
        {
            continueStack.push(label1);
        }
        // condition != 1
        if (!isNumber(condition))
        {
            std::string label2 = new_label();
            code.push_back({ Op_if, condition, "", label2 });   // if (condition) goto L2
            code.push_back({ Op_goto, "", "", label3 });        // goto L3
            code.push_back({ Op_label, "", "", label2 });       // L2:
        }
        generate_3ac(statement);                                // do statement
        if (is_for)
        {
            code.push_back({ Op_label, "", "", label4 });       // L4:
            do_expression(node.childs[2]);
        }
        code.push_back({ Op_goto, "", "", label1 });            // goto L1
        code.push_back({ Op_label, "", "", label3 });           // L3:
        // clear jump stack
        if (!breakStack.empty() && breakStack.top() == label3)
        {
            breakStack.pop();
        }
        if (!continueStack.empty() && (is_for && continueStack.top() == label4 || !is_for && continueStack.top() == label1))
        {
            continueStack.pop();
        }
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
    /* <jump_stmt> -> break ; | continue ; */
    case Parser::jump_stmt:
    {
        const Token &token = node.tokens[0];
        std::stack<std::string> &_stack = (token.type() == Token::Break ? breakStack : continueStack);
        if (_stack.empty())
        {
            Debug::NotWithinLoop(token);
            m_hasError = true;
        }
        else
        {
            std::string jmp_label = _stack.top();
            _stack.pop();
            code.push_back({ Op_goto, "", "", jmp_label });
        }
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
                    code.push_back({ Op_assign, value, "", newName });
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
    FunctionSymbol *func_sym = static_cast<FunctionSymbol *>(SymbolTable::GetInstance().find(curFunName, ".global"));

    // <dec_tail> -> ( <params> ) <func_tail>
    const Parser::TreeNode &params = dec_tail.childs[0];
    const Parser::TreeNode &func_tail = dec_tail.childs[1];

    // <func_tail> -> ;
    if (func_tail.childs.empty())
    {
        std::vector<Token::Type> params_type;
        dec_params(params, params_type, false); // get params type without defining
        const FunctionSymbol &sym = FunctionSymbol(type.type(), returnPointer, params_type);
        if (func_sym != nullptr && *func_sym != sym)
        {
            Debug::Redeclare(id);
            m_hasError = true;
        }
        else if (func_sym == nullptr)
        {
            SymbolTable::GetInstance().insert(curFunName, ".global", new FunctionSymbol(type.type(), returnPointer, params_type));
        }
        return;
    }

    // <func_tail> -> <compound_stmt>
    const Parser::TreeNode &compound_stmt = func_tail.childs[0];
    code.push_back({ Op_begin_func, "", "", curFunName });
    scopeStack.push(curFunName + "~1"); // push scope '{func name}~1'

    scope_counter = 1;

    if (definedFunctions.find(curFunName) != definedFunctions.end())
    {
        Debug::Redeclare(id);
        m_hasError = true;
        scopeStack.pop();
        return;
    }

    std::vector<Token::Type> params_type;
    dec_params(params, params_type);

    const FunctionSymbol &sym = FunctionSymbol(type.type(), returnPointer, params_type);
    if ((func_sym != nullptr && *func_sym != sym))
    {
        Debug::Redeclare(id);
        m_hasError = true;
        scopeStack.pop();
        return;
    }

    definedFunctions.insert(curFunName);
    if (func_sym == nullptr)
    {
        SymbolTable::GetInstance().insert(curFunName, ".global", new FunctionSymbol(type.type(), returnPointer, params_type));
    }

    scope_counter = 0;
    generate_3ac(compound_stmt);
    
    code.push_back({ Op_label, "", "", curFunName + "_exit" }); // func_exit:

    code.push_back({ Op_end_func, "", "", curFunName });
}

void TACGenerator::dec_params(const Parser::TreeNode &node, std::vector<Token::Type> &params_type, bool defineParams)
{
    /* <param> -> <data_type> <is_pointer> id */
    if (node.vn_type == Parser::param)
    {
        const Parser::TreeNode &data_type = node.childs[0];
        const Parser::TreeNode &is_pointer = node.childs[1];
        const Token &id = node.tokens[0];
        const Token &type = data_type.tokens[0];
        /* <is_pointer> -> * | ~ */
        bool isPointer = (is_pointer.tokens.empty() ? false : true);

        std::string varName = id.lexeme();
        if (type.type() == Token::Void)
        {
            Debug::TypeError(type);
            m_hasError = true;
            return;
        }
        else if (defineParams && SymbolTable::GetInstance().find(varName, scopeStack.top()) != nullptr)
        {
            Debug::Redeclare(id);
            m_hasError = true;
            return;
        }
        else if (defineParams)
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
            std::string size;
            if (type.type() == Token::DT_char)
            {
                data_type = "char";
                size = "1";
            }
            else
            {
                data_type = "int";
                size = "8";
            }
            if (isPointer)
            {
                data_type = "ptr_" + data_type;
                size = "8";
            }
            else
            {
                data_type = "var_" + data_type;
            }
            code.push_back({ Op_dec_param, size, data_type, newName });
        }
        params_type.push_back(type.type());
    }
    else
    {
        /* <params> -> <param> <params_tail> | ~ */
        /* <params_tail> -> , <param> <params_tail> | ~ */
        for (const Parser::TreeNode &child : node.childs)
        {
            dec_params(child, params_type, defineParams);
        }
    }

}

std::string TACGenerator::do_expression(const Parser::TreeNode &node)
{
    /* <expression> -> <or_expression> <expression_tail> */
    if (node.vn_type == Parser::expression)
    {
        const Parser::TreeNode &or_expression = node.childs[0];
        const Parser::TreeNode *expression_tail = &node.childs[1];
        std::stack<const Parser::TreeNode *> assign_stack;
        assign_stack.push(&or_expression);
        /* <expression_tail> -> = <or_expression> <expression_tail> | ~ */
        const Token &eq_token = expression_tail->tokens[0];
        while (!expression_tail->childs.empty())
        {
            const Parser::TreeNode &or_expression = expression_tail->childs[0];
            assign_stack.push(&or_expression);
            expression_tail = &expression_tail->childs[1];
        }
        std::string right_value = "";
        while (assign_stack.size() > 0)
        {
            const Parser::TreeNode &expression = *assign_stack.top();
            assign_stack.pop();
            std::string left_value = do_expression(expression);
            if (right_value == "")
            {
                right_value = left_value;
                continue;
            }
            if (left_value == "")
            {
                return "";
            }
            if (left_value[0] == '&' || isNumber(left_value) || left_value.substr(0, 2) == "t^")
            {
                Debug::InvalidOperands(eq_token);
                m_hasError = true;
                return "";
            }
            code.push_back({ Op_assign, right_value, "", left_value });
            right_value = left_value;
        }
        return right_value;
    }
    /* <or_expression> -> <and_expression> <or_expression_tail> */
    else if (node.vn_type == Parser::or_expression)
    {
        const Parser::TreeNode &and_expression = node.childs[0];
        const Parser::TreeNode *or_expression_tail = &node.childs[1];
        std::string value = do_expression(and_expression);
        /* <or_expression_tail> -> || <and_expression> <or_expression_tail> | ~ */
        if (or_expression_tail->childs.empty())
        {
            return value;
        }

        std::string lable_true = new_label();
        std::string lable_false = new_label();
        std::string lable_end = new_label();
        bool sign = false;

        while (value != "")
        {
            if (isNumber(value) && std::stoll(value))
            {
                if (!sign)
                {
                    label_counter -= 3;
                    return "1"; 
                }
                code.push_back({ Op_goto, "", "", lable_true });
                break;
            }
            if (!isNumber(value))
            {
                sign = true;
                code.push_back({ Op_if, value, "", lable_true });
            }
            if (!or_expression_tail->childs.empty())
            {
                const Parser::TreeNode &and_expression = or_expression_tail->childs[0];
                or_expression_tail = &or_expression_tail->childs[1];
                value = do_expression(and_expression);
            }
            else
            {
                value = "";
            }
        }
        if (!sign)
        {
            label_counter -= 3;
            return "0"; 
        }
        std::string temp = new_temp();
        code.push_back({ Op_goto, "", "", lable_false });       //      goto lable_false
        code.push_back({ Op_label, "", "", lable_true });       //  label_true:
        code.push_back({ Op_assign, "1", "", temp });           //      temp = 1
        code.push_back({ Op_goto, "", "", lable_end });         //      goto label_end
        code.push_back({ Op_label, "", "", lable_false });      //  lable_false:
        code.push_back({ Op_assign, "0", "", temp });           //      temp = 0
        code.push_back({ Op_label, "", "", lable_end });        //  lable_end:
        return temp;
    }
    /* <and_expression> -> <relational_expression> <and_expression_tail> */
    else if (node.vn_type == Parser::and_expression)
    {
        const Parser::TreeNode &relational_expression = node.childs[0];
        const Parser::TreeNode *and_expression_tail = &node.childs[1];
        std::string value = do_expression(relational_expression);
        /* <and_expression_tail> -> && <relational_expression> <and_expression_tail> | ~ */
        if (and_expression_tail->childs.empty())
        {
            return value;
        }

        std::string lable_false = new_label();
        std::string lable_true = new_label();
        std::string lable_end = new_label();
        bool sign = false;

        while (value != "")
        {
            if (isNumber(value) && !std::stoll(value))
            {
                if (!sign)
                {
                    label_counter -= 3;
                    return "0";
                }
                code.push_back({ Op_goto, "", "", lable_false });
                break;
            }
            if (!isNumber(value))
            {
                sign = true;
                std::string nor_temp = new_temp();
                code.push_back({ Op_not, value, "", nor_temp });
                code.push_back({ Op_if, nor_temp, "", lable_false });
            }
            if (!and_expression_tail->childs.empty())
            {
                const Parser::TreeNode &relational_expression = and_expression_tail->childs[0];
                and_expression_tail = &and_expression_tail->childs[1];
                value = do_expression(relational_expression);
            }
            else
            {
                value = "";
            }
        }
        if (!sign)
        {
            label_counter -= 3;
            return "1";
        }
        std::string temp = new_temp();
        code.push_back({ Op_goto, "", "", lable_true });        //      goto lable_true
        code.push_back({ Op_label, "", "", lable_false });      //  lable_false:
        code.push_back({ Op_assign, "0", "", temp });           //      temp = 0
        code.push_back({ Op_goto, "", "", lable_end });         //      goto label_end
        code.push_back({ Op_label, "", "", lable_true });       //  lable_true:
        code.push_back({ Op_assign, "1", "", temp });           //      temp = 1
        code.push_back({ Op_label, "", "", lable_end });        //  lable_end:
        return temp;
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
            // Both of arg1 and arg2 is pointer or A - pointer
            else if (pointerStride_1 && pointerStride_2 || op == Op_sub && pointerStride_2)
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
                    if (offset == 0)
                    {
                        code.push_back({ Op_assign, ptr, "", temp });
                    }
                    else
                    {
                        code.push_back({ op, ptr, std::to_string(offset), temp });
                    }
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
            else if (temp1 == "0" && op == Op_add)
            {
                temp1 = temp2;
            }
            else if (temp1 == "0" && op == Op_sub)
            {
                std::string temp3 = new_temp();
                code.push_back({ Op_neg, temp2, "", temp3 });
                temp1 = temp3;
            }
            else if (temp2 == "0")
            {
                continue;
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
            else if (op == Op_mult && (temp1 == "0" || temp2 == "0"))
            {
                temp1 = "0";
            }
            else if (op == Op_mult && temp1 == "1")
            {
                temp1 = temp2;
            }
            else if (op == Op_mult && temp1 == "-1")
            {
                std::string temp3 = new_temp();
                code.push_back({ Op_neg, temp2, "", temp3 });
                temp1 = temp3;
            }
            else if ((op == Op_mult || op == Op_div) && temp2 == "1")
            {
                continue;
            }
            else if ((op == Op_mult || op == Op_div) && temp2 == "-1")
            {
                std::string temp3 = new_temp();
                code.push_back({ Op_neg, temp1, "", temp3 });
                temp1 = temp3;
            }
            else if (op == Op_mod && (temp2 == "1" || temp2 == "-1"))
            {
                temp1 = "0";
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
            if (getPointerStride(addr) != 0 || addr[0] == '&' || addr.substr(0, 2) == "t^")
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
            if (getPointerStride(addr) == 0)
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
            // character
            if (num.lexeme()[0] == '\'')
            {
                return std::to_string(num.lexeme()[1]);
            }
            // string
            else if (num.lexeme()[0] == '\"')
            {
                return '&' + getStrName(num.lexeme());
            }
            // integer
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
            if (offset == 0)
            {
                return id_sym->type == Symbol::Arr ? newName : "*" + newName;
            }
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

    // string
    if (str.substr(0, 2) == "&.")
    {
        return dataSizeMap.find(Token::DT_char)->second;
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
        Token::Type type;
        if (sym->type == Symbol::Arr)
        {
            type = static_cast<ArrSymbol *>(sym)->data_type;
        }
        else
        {
            type = static_cast<VarSymbol *>(sym)->data_type;
        }
        return dataSizeMap.find(type)->second;
    }
    // pointer
    else if (sym->type == Symbol::Ptr)
    {
        Token::Type type = static_cast<PtrSymbol *>(sym)->data_type;
        return dataSizeMap.find(type)->second;
    }
    // var
    return 0;
}