# mycc
A simple compiler similar of c

# run
./mycc source1.c source2.c ... -l lib1.a lib2.a ... -o output.out

# process
file -> [pret] -> source -> [lexical] -> tokens -> [parse] -> ast -> [3ac_gen] -> 3ac -> [3ac_opt] -> opt_3ac -> [reg_alloc] -> [as_gen] -> [as_ld]