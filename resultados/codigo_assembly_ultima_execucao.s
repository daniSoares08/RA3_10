.data
.align 3
const_zero_double: .double 0.0
const_one_double: .double 1.0
temp_pow_base: .double 0.0
mem_VAR: .double 0.0
mem_FLAG: .double 0.0
resultado_0: .double 0.0
resultado_1: .double 0.0
resultado_2: .double 0.0
resultado_3: .double 0.0
resultado_4: .double 0.0
resultado_5: .double 0.0
resultado_6: .double 0.0
resultado_7: .double 0.0
resultado_8: .double 0.0
resultado_9: .double 0.0
resultado_10: .double 0.0
resultado_11: .double 0.0
resultado_12: .double 0.0
resultado_13: .double 0.0
resultado_14: .double 0.0
resultado_15: .double 0.0
resultado_16: .double 0.0
resultado_17: .double 0.0
resultado_18: .double 0.0
resultado_19: .double 0.0
resultado_20: .double 0.0
resultado_21: .double 0.0
pilha_topo: .word 0
pilha_valores: .space 4096

const_0: .double 3.14
const_1: .double 2.0
const_2: .double 10
const_3: .double 3
const_4: .double 9
const_5: .double 4
const_6: .double 2
const_7: .double 8
const_8: .double 7.0
const_9: .double 2.0
const_10: .double 5.0
const_11: .double 1.5
const_12: .double 4
const_13: .double 3
const_14: .double 42
const_15: .double 1
const_16: .double 2
const_17: .double 3
const_18: .double 4
const_19: .double 0
const_20: .double 1
const_21: .double 8
const_22: .double 2
const_23: .double 1
const_24: .double 1
const_25: .double 1
const_26: .double 1
const_27: .double 1
const_28: .double 2
const_29: .double 3
const_30: .double 4
const_31: .double 5
const_32: .double 6
const_33: .double 0
const_34: .double 1
.syntax unified
.fpu vfpv3
.global _start
.text
_start:
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ ---- comando 0 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_0
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_1
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_0
    vstr d0, [r0]
    @ ---- comando 1 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_2
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_3
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d0
    vcvt.s32.f64 s1, d1
    vmov r0, s0
    vmov r1, s1
    cmp r1, #0
    beq int_div_zero_0
    bl dividir_inteiros_assinados
    vmov s0, r2
    vcvt.f64.s32 d0, s0
    bl push_d0
    b int_div_fim_0
int_div_zero_0:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
int_div_fim_0:
    bl pop_d0
    ldr r0, =resultado_1
    vstr d0, [r0]
    @ ---- comando 2 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_4
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_5
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d0
    vcvt.s32.f64 s1, d1
    vmov r0, s0
    vmov r1, s1
    cmp r1, #0
    beq int_div_zero_1
    bl dividir_inteiros_assinados
    vmov s0, r3
    vcvt.f64.s32 d0, s0
    bl push_d0
    b int_div_fim_1
int_div_zero_1:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
int_div_fim_1:
    bl pop_d0
    ldr r0, =resultado_2
    vstr d0, [r0]
    @ ---- comando 3 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_6
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_7
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcvt.s32.f64 s0, d1
    vmov r0, s0
    ldr r1, =temp_pow_base
    vstr d0, [r1]
    ldr r1, =const_one_double
    vldr d0, [r1]
pow_loop_2:
    cmp r0, #0
    beq pow_fim_2
    ldr r1, =temp_pow_base
    vldr d2, [r1]
    vmul.f64 d0, d0, d2
    sub r0, r0, #1
    b pow_loop_2
pow_fim_2:
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_3
    vstr d0, [r0]
    @ ---- comando 4 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_8
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_9
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vdiv.f64 d0, d0, d1
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_4
    vstr d0, [r0]
    @ ---- comando 5 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_10
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_11
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vsub.f64 d0, d0, d1
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_5
    vstr d0, [r0]
    @ ---- comando 6 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_12
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_13
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vmul.f64 d0, d0, d1
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_6
    vstr d0, [r0]
    @ ---- comando 7 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_14
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =mem_VAR
    vstr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_7
    vstr d0, [r0]
    @ ---- comando 8 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =mem_VAR
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_8
    vstr d0, [r0]
    @ ---- comando 9 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ RES 2: resultado do comando 7
    ldr r0, =resultado_7
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_9
    vstr d0, [r0]
    @ ---- comando 10 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_15
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_16
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    blt cmp_true_4
    mov r0, #0
    b cmp_done_4
cmp_true_4:
    mov r0, #1
cmp_done_4:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq if_fim_3
    ldr r0, =const_17
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_18
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
if_fim_3:
    bl pop_d0
    ldr r0, =resultado_10
    vstr d0, [r0]
    @ ---- comando 11 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
while_ini_5:
    ldr r0, =mem_VAR
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_19
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    bgt cmp_true_6
    mov r0, #0
    b cmp_done_6
cmp_true_6:
    mov r0, #1
cmp_done_6:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq while_fim_5
    ldr r0, =mem_VAR
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_20
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vsub.f64 d0, d0, d1
    bl push_d0
    b while_ini_5
while_fim_5:
    bl pop_d0
    ldr r0, =resultado_11
    vstr d0, [r0]
    @ ---- comando 12 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ literal logico TRUE
    ldr r0, =const_one_double
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =mem_FLAG
    vstr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_12
    vstr d0, [r0]
    @ ---- comando 13 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =mem_FLAG
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_13
    vstr d0, [r0]
    @ ---- comando 14 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =mem_FLAG
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq if_fim_7
    ldr r0, =const_21
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_22
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vmul.f64 d0, d0, d1
    bl push_d0
if_fim_7:
    bl pop_d0
    ldr r0, =resultado_14
    vstr d0, [r0]
    @ ---- comando 15 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
while_ini_8:
    ldr r0, =mem_FLAG
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq while_fim_8
    ldr r0, =const_23
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_24
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
    b while_ini_8
while_fim_8:
    bl pop_d0
    ldr r0, =resultado_15
    vstr d0, [r0]
    @ ---- comando 16 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
while_ini_9:
    @ literal logico FALSE
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq while_fim_9
    ldr r0, =const_25
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_26
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
    b while_ini_9
while_fim_9:
    bl pop_d0
    ldr r0, =resultado_16
    vstr d0, [r0]
    @ ---- comando 17 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ literal logico TRUE
    ldr r0, =const_one_double
    vldr d0, [r0]
    bl push_d0
    @ literal logico FALSE
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d2, [r0]
    vcmp.f64 d0, d2
    vmrs APSR_nzcv, FPSCR
    bne logico_verd_10
    vcmp.f64 d1, d2
    vmrs APSR_nzcv, FPSCR
    bne logico_verd_10
    mov r0, #0
    b logico_fim_10
logico_verd_10:
    mov r0, #1
logico_fim_10:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_17
    vstr d0, [r0]
    @ ---- comando 18 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ literal logico TRUE
    ldr r0, =const_one_double
    vldr d0, [r0]
    bl push_d0
    @ literal logico TRUE
    ldr r0, =const_one_double
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d2, [r0]
    vcmp.f64 d0, d2
    vmrs APSR_nzcv, FPSCR
    beq logico_falso_11
    vcmp.f64 d1, d2
    vmrs APSR_nzcv, FPSCR
    beq logico_falso_11
    mov r0, #1
    b logico_fim_11
logico_falso_11:
    mov r0, #0
logico_fim_11:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_18
    vstr d0, [r0]
    @ ---- comando 19 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    @ literal logico FALSE
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq not_verd_12
    mov r0, #0
    b not_fim_12
not_verd_12:
    mov r0, #1
not_fim_12:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =resultado_19
    vstr d0, [r0]
    @ ---- comando 20 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
    ldr r0, =const_27
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_28
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    blt cmp_true_14
    mov r0, #0
    b cmp_done_14
cmp_true_14:
    mov r0, #1
cmp_done_14:
    bl bool_from_r0
    bl push_d0
    ldr r0, =const_29
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_30
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    bgt cmp_true_15
    mov r0, #0
    b cmp_done_15
cmp_true_15:
    mov r0, #1
cmp_done_15:
    bl bool_from_r0
    bl push_d0
    bl pop_d1
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d2, [r0]
    vcmp.f64 d0, d2
    vmrs APSR_nzcv, FPSCR
    bne logico_verd_16
    vcmp.f64 d1, d2
    vmrs APSR_nzcv, FPSCR
    bne logico_verd_16
    mov r0, #0
    b logico_fim_16
logico_verd_16:
    mov r0, #1
logico_fim_16:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq if_fim_13
    ldr r0, =const_31
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_32
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vadd.f64 d0, d0, d1
    bl push_d0
if_fim_13:
    bl pop_d0
    ldr r0, =resultado_20
    vstr d0, [r0]
    @ ---- comando 21 ----
    ldr r0, =pilha_topo
    mov r1, #0
    str r1, [r0]
while_ini_17:
    ldr r0, =mem_VAR
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_33
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    bgt cmp_true_18
    mov r0, #0
    b cmp_done_18
cmp_true_18:
    mov r0, #1
cmp_done_18:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq not_verd_19
    mov r0, #0
    b not_fim_19
not_verd_19:
    mov r0, #1
not_fim_19:
    bl bool_from_r0
    bl push_d0
    bl pop_d0
    ldr r0, =const_zero_double
    vldr d1, [r0]
    vcmp.f64 d0, d1
    vmrs APSR_nzcv, FPSCR
    beq while_fim_17
    ldr r0, =mem_VAR
    vldr d0, [r0]
    bl push_d0
    ldr r0, =const_34
    vldr d0, [r0]
    bl push_d0
    bl pop_d1
    bl pop_d0
    vsub.f64 d0, d0, d1
    bl push_d0
    b while_ini_17
while_fim_17:
    bl pop_d0
    ldr r0, =resultado_21
    vstr d0, [r0]
fim:
    b fim

push_d0:
    ldr r1, =pilha_topo
    ldr r2, [r1]
    ldr r3, =pilha_valores
    add r3, r3, r2
    vstr d0, [r3]
    add r2, r2, #8
    str r2, [r1]
    bx lr

pop_d0:
    ldr r1, =pilha_topo
    ldr r2, [r1]
    cmp r2, #0
    beq pop_d0_zero
    sub r2, r2, #8
    str r2, [r1]
    ldr r3, =pilha_valores
    add r3, r3, r2
    vldr d0, [r3]
    bx lr
pop_d0_zero:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bx lr

pop_d1:
    ldr r1, =pilha_topo
    ldr r2, [r1]
    cmp r2, #0
    beq pop_d1_zero
    sub r2, r2, #8
    str r2, [r1]
    ldr r3, =pilha_valores
    add r3, r3, r2
    vldr d1, [r3]
    bx lr
pop_d1_zero:
    ldr r0, =const_zero_double
    vldr d1, [r0]
    bx lr

bool_from_r0:
    cmp r0, #0
    beq bool_false
    ldr r0, =const_one_double
    vldr d0, [r0]
    bx lr
bool_false:
    ldr r0, =const_zero_double
    vldr d0, [r0]
    bx lr

dividir_inteiros_assinados:
    mov r2, #0
    mov r3, #0
    cmp r0, #0
    bge div_dividendo_ok
    rsb r0, r0, #0
    eor r2, r2, #1
    mov r3, #1
div_dividendo_ok:
    cmp r1, #0
    bge div_divisor_ok
    rsb r1, r1, #0
    eor r2, r2, #1
div_divisor_ok:
    mov r12, #0
div_loop:
    cmp r0, r1
    blt div_fim_loop
    sub r0, r0, r1
    add r12, r12, #1
    b div_loop
div_fim_loop:
    cmp r2, #0
    beq div_sinal_quociente_ok
    rsb r12, r12, #0
div_sinal_quociente_ok:
    cmp r3, #0
    beq div_sinal_resto_ok
    rsb r0, r0, #0
div_sinal_resto_ok:
    mov r2, r12
    mov r3, r0
    bx lr
