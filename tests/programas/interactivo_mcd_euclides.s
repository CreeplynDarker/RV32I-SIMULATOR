# =====================================================================
#  MCD (Maximo Comun Divisor) - Algoritmo de Euclides por restas
#  RISC-V RV32I base (sin extension M: no usa div/rem, solo sub)
#
#  Lee dos enteros POSITIVOS por consola, imprime su MCD.
#  Convencion de ecall estilo SPIM/CPUlator: a7 = servicio, a0 = arg.
#    a7=4  print_string  (a0 = puntero a cadena)
#    a7=5  read_int      (resultado en a0)
#    a7=1  print_int     (a0 = entero)
#    a7=10 exit
# =====================================================================

.data
prompt1:  .string "Primer numero:  "
prompt2:  .string "Segundo numero: "
result:   .string "MCD = "
newline:  .string "\n"

.text
.globl main
main:
    # ---- Pedir y leer el primer numero -> s0 ----
    la   a0, prompt1        # a0 = direccion de "Primer numero:  "
    li   a7, 4              # servicio 4 = print_string
    ecall
    li   a7, 5              # servicio 5 = read_int
    ecall
    mv   s0, a0             # s0 = primer numero

    # ---- Pedir y leer el segundo numero -> s1 ----
    la   a0, prompt2
    li   a7, 4
    ecall
    li   a7, 5
    ecall
    mv   s1, a0             # s1 = segundo numero

    # ---- Algoritmo de Euclides por restas ----
    # while (s0 != s1):
    #     if (s0 < s1):  s1 -= s0
    #     else:          s0 -= s1
gcd_loop:
    beq  s0, s1, gcd_done   # si son iguales, ese es el MCD
    blt  s0, s1, sub_from_s1
    sub  s0, s0, s1         # s0 >= s1 : s0 = s0 - s1
    j    gcd_loop
sub_from_s1:
    sub  s1, s1, s0         # s0 <  s1 : s1 = s1 - s0
    j    gcd_loop
gcd_done:
    # s0 (== s1) contiene el MCD

    # ---- Imprimir "MCD = " y el resultado ----
    la   a0, result
    li   a7, 4
    ecall
    mv   a0, s0            # a0 = MCD
    li   a7, 1            # servicio 1 = print_int
    ecall
    la   a0, newline
    li   a7, 4
    ecall

    # ---- Salir ----
    li   a7, 10           # servicio 10 = exit
    ecall