def printList(lst) : 
    for v in lst : 
        print('\t', end = '')
        print(v)

def isVn(v) : 
    return v[0] == '<' and v[-1] == '>'

file = open('../Grammar.txt', 'r')
content = file.read()
file.close()
lines = content.split('\n')

Vn = [] # 非终结符
Vt = [] # 终结符

S = dict()

print("LL(1)文法:")

for line in lines : 
    if not '->' in line : 
        continue
    print('\t', end = '')
    print(line)
    left, right = line.split('->')
    left = left.split()[0]
    right = right.split()
    if not left in S : 
        S[left] = [right]
    else : 
        S[left].append(right)

    if not left in Vn : 
        Vn.append(left)
    for v in right : 
        if not isVn(v) and not v in Vt : 
            Vt.append(v)

print("非终结符:")
printList(Vn)
print("终结符:")
printList(Vt)

from collections import defaultdict

FIRST = defaultdict(set)
PRODUCTION_FIRST = defaultdict(set)
FOLLOW = defaultdict(set)
Nullable = dict()

#求FIRST集
def getFIRST(epsilon = '~'):
    # 初始化终结符的FIRST集
    for terminal in Vt:
        FIRST[terminal].add(terminal)
    # 初始化非终结符的FIRST集
    for non_terminal in S:
        FIRST[non_terminal] = set()
    # 迭代计算直到收敛
    changed = True
    while changed:
        changed = False
        for non_terminal in S:
            for production in S[non_terminal]:
                # 计算当前产生式右侧的FIRST集
                current_first = set()
                all_epsilon = True
                for symbol in production:
                    # 添加FIRST(symbol)的非ε元素
                    current_first.update(FIRST[symbol] - {epsilon})
                    # 如果当前符号不能推导出ε，则停止
                    if epsilon not in FIRST[symbol]:
                        all_epsilon = False
                        break
                # 如果所有符号都能推导出ε，则添加ε
                if all_epsilon:
                    current_first.add(epsilon)
                # 记录产生式右侧的FIRST集
                production_key = (non_terminal, tuple(production))
                if PRODUCTION_FIRST[production_key] != current_first:
                    PRODUCTION_FIRST[production_key] = current_first.copy()
                # 更新非终结符的FIRST集
                before_size = len(FIRST[non_terminal])
                FIRST[non_terminal].update(current_first)
                if len(FIRST[non_terminal]) > before_size:
                    changed = True

#求FOLLOW集
def getFOLLOW(epsilon = '~') : 
    FOLLOW[Vn[0]].add('$')  # 规则1: 开始符号的FOLLOW包含$
    while True:
        updated = False
        for non_terminal in S:
            for production in S[non_terminal]:
                # 遍历产生式右侧的每个符号
                trailer = FOLLOW[non_terminal].copy()  # 初始化trailer为FOLLOW(A)
                for symbol in reversed(production):    # 从右向左处理
                    if symbol in S:  # 只处理非终结符
                        # 规则2: 将trailer加入FOLLOW(B)
                        before_size = len(FOLLOW[symbol])
                        FOLLOW[symbol].update(trailer)
                        if len(FOLLOW[symbol]) > before_size:
                            updated = True
                        # 更新trailer: 如果当前符号可以推导出ε，则继续包含FIRST(剩余部分)
                        if epsilon in FIRST[symbol]:
                            trailer.update(FIRST[symbol] - {epsilon})
                        else:
                            trailer = FIRST[symbol].copy()  # 否则重置为FIRST(symbol)
                    else:  # 终结符
                        trailer = {symbol}  # 重置为当前终结符 
        if not updated:
            break

#求Nullable属性
def getNullable() : 
    for vn in Vn : 
        if '~' in FIRST[vn] : 
            Nullable[vn] = True
        else : 
            Nullable[vn] = False

getFIRST()
getFOLLOW()
getNullable()

print("FIRST & FOLLOW & Nullable:")

for vn in Vn : 
    #print FIRST
    print('\t', end = '')
    print(f'FIRST({vn}) ', end = '')
    n = 30 - len(vn)
    print(n * " ", end = '')
    print("= { ", end = '')
    first_lst = []
    for p in S[vn] : 
        first_lst += list(PRODUCTION_FIRST[(vn, tuple(p))])
    for v in sorted(first_lst) : 
        print(v, end = " ")
    print("}")

    #print FOLLOW
    print('\t', end = '')
    print(f'FOLLOW({vn}) ', end = '')
    n = 29 - len(vn)
    print(n * " ", end = '')
    print("= { ", end = '')
    for v in FOLLOW[vn] : 
        print(v, end = " ")
    print("}")

    #print Nullable
    print('\t', end = '')
    print(f'Nullable({vn}) ', end = '')
    n = 27 - len(vn)
    print(n * " ", end = '')
    print("= { ", end = '')
    print(Nullable[vn], end = " ")
    print("}")

    print()

Vt.remove('~')
Vt.append('$')
Table = defaultdict(dict)

#求LL1分析表
def getAnalysisTable() : 
    for vn in Vn : 
        for vt in Vt : 
            if vt in FIRST[vn] : 
                for s in S[vn] : 
                    if vt in PRODUCTION_FIRST[(vn, tuple(s))] : 
                        Table[vn][vt] = s
                        break
            elif Nullable[vn] and vt in FOLLOW[vn] : 
                 Table[vn][vt] = ['~']

getAnalysisTable()

print("LL(1)分析表:")

for vn in Vn : 
    print(f"\t{vn}:\n", end="")
    for vt in Vt : 
        if vt in Table[vn] : 
            print(f"\t\t{vt}\t ->\t ", end = '')
            print("{ ", end = '')
            for v in Table[vn][vt] : 
                print(v, end = " ")
            print("}")
    print()
    

