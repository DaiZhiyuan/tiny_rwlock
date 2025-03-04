# tiny_rwlock

rwlock implementation(From Linux)

## LL/SC（Load-Link/Store-Conditional）

```
● make
● make check
 a2c:   885ffc41        ldaxr   w1, [x2]
 a38:   88037c41        stxr    w3, w1, [x2]
 a68:   885f7c41        ldxr    w1, [x2]
 a70:   8803fc41        stlxr   w3, w1, [x2]
 aac:   885ffc43        ldaxr   w3, [x2]
 ab4:   88037c41        stxr    w3, w1, [x2]
program use load/store exclusives instruction

● ./bin/rwlock_test
Reader 1 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 1 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 1 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 1 reading resource
Writer 2 writing to resource
Reader 1 reading resource
Reader 2 reading resource
Reader 3 reading resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 1 writing to resource
Test completed.
```

## LSE（Large System Extension）

```
● make -f Makefile.lse
● make -f Makefile.lse check
 a38:   88e17c43        casa    w1, w3, [x2]
 aa4:   88e37c41        casa    w3, w1, [x2]
program use LSE instruction

● ./bin/rwlock_test
Reader 1 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 1 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 1 reading resource
Reader 4 reading resource
Reader 2 reading resource
Reader 3 reading resource
Reader 4 reading resource
Reader 1 reading resource
Writer 2 writing to resource
Reader 1 reading resource
Reader 3 reading resource
Reader 2 reading resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 2 writing to resource
Writer 1 writing to resource
Writer 1 writing to resource
Test completed.
```
