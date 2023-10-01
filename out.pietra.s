	.text
	.file	"PLang"
	.globl	pietra_main                     # -- Begin function pietra_main
	.p2align	4, 0x90
	.type	pietra_main,@function
pietra_main:                            # @pietra_main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf
	movl	$.L__unnamed_2, %edi
	xorl	%eax, %eax
	callq	printf
	movq	$0, (%rsp)
	cmpq	$9, (%rsp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %L1
                                        # =>This Inner Loop Header: Depth=1
	movq	(%rsp), %rsi
	movl	$.L__unnamed_3, %edi
	xorl	%eax, %eax
	callq	printf
	incq	(%rsp)
	cmpq	$9, (%rsp)
	jle	.LBB0_2
.LBB0_3:                                # %L2
	movl	$.L__unnamed_4, %edi
	xorl	%eax, %eax
	callq	printf
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	pietra_main, .Lfunc_end0-pietra_main
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %L3
	pushq	%rax
	.cfi_def_cfa_offset 16
	callq	pietra_main
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"essa linguagem esta sendo craida 100% por Github -> @pedroGoffi\n"
	.size	.L__unnamed_1, 65

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	"begin\n"
	.size	.L__unnamed_2, 7

	.type	.L__unnamed_3,@object           # @2
.L__unnamed_3:
	.asciz	"Hey x = %i!\n"
	.size	.L__unnamed_3, 13

	.type	.L__unnamed_4,@object           # @3
.L__unnamed_4:
	.asciz	"End!\n"
	.size	.L__unnamed_4, 6

	.section	".note.GNU-stack","",@progbits
