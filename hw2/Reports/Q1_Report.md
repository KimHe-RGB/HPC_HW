# Report

## val_test01

Compling with:

```bash
valgrind --tool=memcheck ./val_test01
```

Error logs:

```bash
==6586== Memcheck, a memory error detector
==6586== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==6586== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==6586== Command: ./val_test01
==6586==
==6586== error calling PR_SET_PTRACER, vgdb might block

TEST01
  C++ version.
  A sample code for analysis by VALGRIND.
  0  1
  1  1
  2  2
  3  3
  4  5
  5  8
  6  13
  7  21
  8  34
  9  55
==6586== Invalid write of size 4
==6586==    at 0x109410: f(int) (val_test01.cpp:82)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==  Address 0x4dd0ce8 is 0 bytes after a block of size 40 alloc'd
==6586==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6586==    by 0x1092DB: f(int) (val_test01.cpp:72)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==
==6586== Invalid read of size 4
==6586==    at 0x109467: f(int) (val_test01.cpp:83)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==  Address 0x4dd0ce8 is 0 bytes after a block of size 40 alloc'd
==6586==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6586==    by 0x1092DB: f(int) (val_test01.cpp:72)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==
  10  89
==6586== Mismatched free() / delete / delete []
==6586==    at 0x484CA8F: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6586==    by 0x1094AA: f(int) (val_test01.cpp:86)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==  Address 0x4dd0cc0 is 0 bytes inside a block of size 40 alloc'd
==6586==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6586==    by 0x1092DB: f(int) (val_test01.cpp:72)
==6586==    by 0x109269: main (val_test01.cpp:40)
==6586==

TEST01
  Normal end of execution.
==6586==
==6586== HEAP SUMMARY:
==6586==     in use at exit: 0 bytes in 0 blocks
==6586==   total heap usage: 3 allocs, 3 frees, 76,840 bytes allocated
==6586==
==6586== All heap blocks were freed -- no leaks are possible
==6586==
==6586== For lists of detected and suppressed errors, rerun with: -s
==6586== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
```

### Issue Analysis

At Line 81, `x[i] = x[i-1] + x[i-2];` will read/write to out-of-bound index when `n=10`. So we need to change the for loop index range

At Line 86, `delete [] x` is a mismatched delete. Instead we should `free(x);`

## val_test02

Compling with:

```bash
valgrind --tool=memcheck --track-origins=yes ./val_test02
```

Error logs:

```bash
==6610== Memcheck, a memory error detector
==6610== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==6610== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==6610== Command: ./val_test02
==6610==
==6610== error calling PR_SET_PTRACER, vgdb might block

TEST02:
  C++ version
  A sample code for analysis by VALGRIND.
  0  0
  1  2
==6610== Conditional jump or move depends on uninitialised value(s)
==6610==    at 0x498E93E: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x499D0E9: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x1093BE: junk_data() (val_test02.cpp:104)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==  Uninitialised value was created by a heap allocation
==6610==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6610==    by 0x1092C1: junk_data() (val_test02.cpp:77)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==
==6610== Use of uninitialised value of size 8
==6610==    at 0x498E81B: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x498E968: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x499D0E9: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x1093BE: junk_data() (val_test02.cpp:104)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==  Uninitialised value was created by a heap allocation
==6610==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6610==    by 0x1092C1: junk_data() (val_test02.cpp:77)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==
==6610== Conditional jump or move depends on uninitialised value(s)
==6610==    at 0x498E82D: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x498E968: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x499D0E9: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x1093BE: junk_data() (val_test02.cpp:104)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==  Uninitialised value was created by a heap allocation
==6610==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6610==    by 0x1092C1: junk_data() (val_test02.cpp:77)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==
==6610== Conditional jump or move depends on uninitialised value(s)
==6610==    at 0x498E99E: std::ostreambuf_iterator<char, std::char_traits<char> > std::num_put<char, std::ostreambuf_iterator<char, std::char_traits<char> > >::_M_insert_int<long>(std::ostreambuf_iterator<char, std::char_traits<char> >, std::ios_base&, char, long) const (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x499D0E9: std::ostream& std::ostream::_M_insert<long>(long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==6610==    by 0x1093BE: junk_data() (val_test02.cpp:104)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==  Uninitialised value was created by a heap allocation
==6610==    at 0x484A2F3: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==6610==    by 0x1092C1: junk_data() (val_test02.cpp:77)
==6610==    by 0x109259: main (val_test02.cpp:37)
==6610==
  2  0
  3  6
  4  8
  5  0
  6  0
  7  0
  8  0
  9  0

TEST02
  Normal end of execution.
==6610==
==6610== HEAP SUMMARY:
==6610==     in use at exit: 0 bytes in 0 blocks
==6610==   total heap usage: 3 allocs, 3 frees, 76,840 bytes allocated
==6610==
==6610== All heap blocks were freed -- no leaks are possible
==6610==
==6610== For lists of detected and suppressed errors, rerun with: -s
==6610== ERROR SUMMARY: 24 errors from 4 contexts (suppressed: 0 from 0)
```

### Issue Analysis

At Line 81, `x[i] = x[i-1] + x[i-2];` will read/write to out-of-bound index when `n=10`. So we need to change the for loop index range

At Line 86, `delete [] x` is a mismatched delete. Instead we should `free(x);`
