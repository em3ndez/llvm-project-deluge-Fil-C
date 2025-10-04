	.text
	.p2align 4
	.globl	foo
	.type	foo, @function
foo:
	leal	(%rdi,%rsi), %eax
	ret
