mov ayb 0
mov ben 5
loop:
cmp ben 0
je end
add ayb 1
jmp loop
end:
mov [0] ayb
dump_memory()
