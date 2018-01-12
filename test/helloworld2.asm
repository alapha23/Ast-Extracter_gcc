
a.out:     file format elf64-x86-64


Disassembly of section .init:

0000000000400390 <_init>:
  400390:	48 83 ec 08          	sub    $0x8,%rsp
  400394:	48 8b 05 5d 0c 20 00 	mov    0x200c5d(%rip),%rax        # 600ff8 <__gmon_start__>
  40039b:	48 85 c0             	test   %rax,%rax
  40039e:	74 02                	je     4003a2 <_init+0x12>
  4003a0:	ff d0                	callq  *%rax
  4003a2:	48 83 c4 08          	add    $0x8,%rsp
  4003a6:	c3                   	retq   

Disassembly of section .text:

00000000004003b0 <_start>:
  4003b0:	31 ed                	xor    %ebp,%ebp
  4003b2:	49 89 d1             	mov    %rdx,%r9
  4003b5:	5e                   	pop    %rsi
  4003b6:	48 89 e2             	mov    %rsp,%rdx
  4003b9:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  4003bd:	50                   	push   %rax
  4003be:	54                   	push   %rsp
  4003bf:	49 c7 c0 90 05 40 00 	mov    $0x400590,%r8
  4003c6:	48 c7 c1 20 05 40 00 	mov    $0x400520,%rcx
  4003cd:	48 c7 c7 9c 04 40 00 	mov    $0x40049c,%rdi
  4003d4:	ff 15 16 0c 20 00    	callq  *0x200c16(%rip)        # 600ff0 <__libc_start_main@GLIBC_2.2.5>
  4003da:	f4                   	hlt    
  4003db:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004003e0 <deregister_tm_clones>:
  4003e0:	55                   	push   %rbp
  4003e1:	b8 20 10 60 00       	mov    $0x601020,%eax
  4003e6:	48 3d 20 10 60 00    	cmp    $0x601020,%rax
  4003ec:	48 89 e5             	mov    %rsp,%rbp
  4003ef:	74 17                	je     400408 <deregister_tm_clones+0x28>
  4003f1:	b8 00 00 00 00       	mov    $0x0,%eax
  4003f6:	48 85 c0             	test   %rax,%rax
  4003f9:	74 0d                	je     400408 <deregister_tm_clones+0x28>
  4003fb:	5d                   	pop    %rbp
  4003fc:	bf 20 10 60 00       	mov    $0x601020,%edi
  400401:	ff e0                	jmpq   *%rax
  400403:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  400408:	5d                   	pop    %rbp
  400409:	c3                   	retq   
  40040a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400410 <register_tm_clones>:
  400410:	be 20 10 60 00       	mov    $0x601020,%esi
  400415:	55                   	push   %rbp
  400416:	48 81 ee 20 10 60 00 	sub    $0x601020,%rsi
  40041d:	48 89 e5             	mov    %rsp,%rbp
  400420:	48 c1 fe 03          	sar    $0x3,%rsi
  400424:	48 89 f0             	mov    %rsi,%rax
  400427:	48 c1 e8 3f          	shr    $0x3f,%rax
  40042b:	48 01 c6             	add    %rax,%rsi
  40042e:	48 d1 fe             	sar    %rsi
  400431:	74 15                	je     400448 <register_tm_clones+0x38>
  400433:	b8 00 00 00 00       	mov    $0x0,%eax
  400438:	48 85 c0             	test   %rax,%rax
  40043b:	74 0b                	je     400448 <register_tm_clones+0x38>
  40043d:	5d                   	pop    %rbp
  40043e:	bf 20 10 60 00       	mov    $0x601020,%edi
  400443:	ff e0                	jmpq   *%rax
  400445:	0f 1f 00             	nopl   (%rax)
  400448:	5d                   	pop    %rbp
  400449:	c3                   	retq   
  40044a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000400450 <__do_global_dtors_aux>:
  400450:	80 3d c5 0b 20 00 00 	cmpb   $0x0,0x200bc5(%rip)        # 60101c <_edata>
  400457:	75 17                	jne    400470 <__do_global_dtors_aux+0x20>
  400459:	55                   	push   %rbp
  40045a:	48 89 e5             	mov    %rsp,%rbp
  40045d:	e8 7e ff ff ff       	callq  4003e0 <deregister_tm_clones>
  400462:	c6 05 b3 0b 20 00 01 	movb   $0x1,0x200bb3(%rip)        # 60101c <_edata>
  400469:	5d                   	pop    %rbp
  40046a:	c3                   	retq   
  40046b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  400470:	f3 c3                	repz retq 
  400472:	0f 1f 40 00          	nopl   0x0(%rax)
  400476:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40047d:	00 00 00 

0000000000400480 <frame_dummy>:
  400480:	55                   	push   %rbp
  400481:	48 89 e5             	mov    %rsp,%rbp
  400484:	5d                   	pop    %rbp
  400485:	eb 89                	jmp    400410 <register_tm_clones>

0000000000400487 <add>:
  400487:	55                   	push   %rbp
  400488:	48 89 e5             	mov    %rsp,%rbp
  40048b:	89 7d fc             	mov    %edi,-0x4(%rbp)
  40048e:	89 75 f8             	mov    %esi,-0x8(%rbp)
  400491:	8b 45 fc             	mov    -0x4(%rbp),%eax
  400494:	83 e8 01             	sub    $0x1,%eax
  400497:	2b 45 f8             	sub    -0x8(%rbp),%eax
  40049a:	5d                   	pop    %rbp
  40049b:	c3                   	retq   

000000000040049c <main>:
  40049c:	55                   	push   %rbp
  40049d:	48 89 e5             	mov    %rsp,%rbp
  4004a0:	c7 45 fc 93 57 00 00 	movl   $0x5793,-0x4(%rbp)
  4004a7:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  4004ab:	8b 45 fc             	mov    -0x4(%rbp),%eax
  4004ae:	0f af 45 fc          	imul   -0x4(%rbp),%eax
  4004b2:	89 45 ec             	mov    %eax,-0x14(%rbp)
  4004b5:	b8 df d8 00 00       	mov    $0xd8df,%eax
  4004ba:	99                   	cltd   
  4004bb:	f7 7d fc             	idivl  -0x4(%rbp)
  4004be:	89 55 e8             	mov    %edx,-0x18(%rbp)
  4004c1:	c7 45 e4 97 2f 01 00 	movl   $0x12f97,-0x1c(%rbp)
  4004c8:	c7 45 fc 0a 00 00 00 	movl   $0xa,-0x4(%rbp)
  4004cf:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  4004d3:	c7 00 01 00 00 00    	movl   $0x1,(%rax)
  4004d9:	8b 45 fc             	mov    -0x4(%rbp),%eax
  4004dc:	48 98                	cltq   
  4004de:	48 83 c0 01          	add    $0x1,%rax
  4004e2:	48 8d 14 85 00 00 00 	lea    0x0(,%rax,4),%rdx
  4004e9:	00 
  4004ea:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  4004ee:	48 01 d0             	add    %rdx,%rax
  4004f1:	c7 00 03 00 00 00    	movl   $0x3,(%rax)
  4004f7:	48 c7 45 d8 b0 05 40 	movq   $0x4005b0,-0x28(%rbp)
  4004fe:	00 
  4004ff:	eb 11                	jmp    400512 <main+0x76>
  400501:	83 6d fc 01          	subl   $0x1,-0x4(%rbp)
  400505:	8b 45 fc             	mov    -0x4(%rbp),%eax
  400508:	48 98                	cltq   
  40050a:	48 c1 e0 02          	shl    $0x2,%rax
  40050e:	48 01 45 f0          	add    %rax,-0x10(%rbp)
  400512:	83 7d fc 00          	cmpl   $0x0,-0x4(%rbp)
  400516:	7f e9                	jg     400501 <main+0x65>
  400518:	b8 00 00 00 00       	mov    $0x0,%eax
  40051d:	5d                   	pop    %rbp
  40051e:	c3                   	retq   
  40051f:	90                   	nop

0000000000400520 <__libc_csu_init>:
  400520:	41 57                	push   %r15
  400522:	41 56                	push   %r14
  400524:	49 89 d7             	mov    %rdx,%r15
  400527:	41 55                	push   %r13
  400529:	41 54                	push   %r12
  40052b:	4c 8d 25 1e 09 20 00 	lea    0x20091e(%rip),%r12        # 600e50 <__frame_dummy_init_array_entry>
  400532:	55                   	push   %rbp
  400533:	48 8d 2d 1e 09 20 00 	lea    0x20091e(%rip),%rbp        # 600e58 <__init_array_end>
  40053a:	53                   	push   %rbx
  40053b:	41 89 fd             	mov    %edi,%r13d
  40053e:	49 89 f6             	mov    %rsi,%r14
  400541:	4c 29 e5             	sub    %r12,%rbp
  400544:	48 83 ec 08          	sub    $0x8,%rsp
  400548:	48 c1 fd 03          	sar    $0x3,%rbp
  40054c:	e8 3f fe ff ff       	callq  400390 <_init>
  400551:	48 85 ed             	test   %rbp,%rbp
  400554:	74 20                	je     400576 <__libc_csu_init+0x56>
  400556:	31 db                	xor    %ebx,%ebx
  400558:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  40055f:	00 
  400560:	4c 89 fa             	mov    %r15,%rdx
  400563:	4c 89 f6             	mov    %r14,%rsi
  400566:	44 89 ef             	mov    %r13d,%edi
  400569:	41 ff 14 dc          	callq  *(%r12,%rbx,8)
  40056d:	48 83 c3 01          	add    $0x1,%rbx
  400571:	48 39 dd             	cmp    %rbx,%rbp
  400574:	75 ea                	jne    400560 <__libc_csu_init+0x40>
  400576:	48 83 c4 08          	add    $0x8,%rsp
  40057a:	5b                   	pop    %rbx
  40057b:	5d                   	pop    %rbp
  40057c:	41 5c                	pop    %r12
  40057e:	41 5d                	pop    %r13
  400580:	41 5e                	pop    %r14
  400582:	41 5f                	pop    %r15
  400584:	c3                   	retq   
  400585:	90                   	nop
  400586:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40058d:	00 00 00 

0000000000400590 <__libc_csu_fini>:
  400590:	f3 c3                	repz retq 

Disassembly of section .fini:

0000000000400594 <_fini>:
  400594:	48 83 ec 08          	sub    $0x8,%rsp
  400598:	48 83 c4 08          	add    $0x8,%rsp
  40059c:	c3                   	retq   
