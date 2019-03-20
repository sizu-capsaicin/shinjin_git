section .data
hello_world	db	"Hello World!", 0x0a
len		equ	$ - hello_world

section .text
global _start

_start:
	mov rax, 0x04			; System call number for write = 4
	mov rdi, 1			; Write to stdout
	mov rsi, hello_world		; The address to "Hello World"
	mov rdx, len			; The size of write
	syscall				; Involke the kernel
	mov rax, 0x01			; System call number for exit = 1
	mov rdi, 0			; Exit Success = 0i
	syscall				; Involke the kernel
