/* Fun with C "polymorphism" */

// generic poly.h

/* stole the #args idea from qemu.git:/include/qemu/compiler.h.
   Genius. Compile errors are really scary though */

#define POLY_ACTION(base, this, ...)       \
    POLY_ACTION_(base, this, __VA_ARGS__, 2, 1, 0)

#define POLY_ACTION_(base, this, func, a1, a2, count, ...)   \
    POLY_ACTION##count(base, this, func, a1, a2)

#define POLY_ACTION_NARGS(base, this, func, ...)                        \
    base##VTable[((struct base*)this)->instanceType].func((struct base*)this, __VA_ARGS__)

#define POLY_ACTION0(base, this, func, ...)                             \
    base##VTable[((struct base*)this)->instanceType].func((struct base*)this)

#define POLY_ACTION1(base, this, func, a2, ...)          \
    POLY_ACTION_NARGS(base, this, func, __VA_ARGS__)

#define POLY_ACTION2(base, this, func, ...)             \
    POLY_ACTION_NARGS(base, this, func, __VA_ARGS__)


// animal.h

struct Animal {
    enum {
        Dog,
        Cat,
        AnimalMaxInst
    } instanceType;
};

struct AnimalInterface {
    int (*makeSound)(struct Animal*, int);
    int (*serve)(struct Animal*);
} AnimalVTable[AnimalMaxInst];

#define ANIMAL_ACTION(...)     \
    POLY_ACTION(Animal, __VA_ARGS__)


// dog.h
#include <stdlib.h>
#include <stdio.h>

struct Dog {
    struct Animal base;

    int dogData;
};

static int bark(struct Animal* this, int arg)
{
    for (int i = 0; i < arg; i++)
        printf("%s ", __func__);
    printf("\n");
}

static int doServe(struct Animal* this)
{
    printf("dog serves\n");
}

struct Dog* makeDog(void)
{
    struct Dog* d = malloc(sizeof(*d));
    d->base.instanceType = Dog;

    AnimalVTable[Dog] = (struct AnimalInterface) {
        .makeSound = bark,
        .serve     = doServe,
    };

    return d;
}


// cat.h

struct Cat {
    struct Animal base;

    int catData;
};

static int meow(struct Animal* this, int arg)
{
    for (int i = 0; i < arg; i++)
        printf("%s ", __func__);
    printf("\n");
}

static int noServe(struct Animal* this)
{
    printf("cat does nothing\n");
}

struct Cat* makeCat(void)
{
    struct Cat* a = malloc(sizeof(*a));
    a->base.instanceType = Cat;

    AnimalVTable[Cat] = (struct AnimalInterface) {
        .makeSound = meow,
        .serve     = noServe,
    };

    return a;
}


// test
int main(void)
{
    struct Dog* dog = makeDog();
    struct Cat* cat = makeCat();

    ANIMAL_ACTION(dog, makeSound, 1);
    ANIMAL_ACTION(cat, makeSound, 2);

    ANIMAL_ACTION(dog, serve);
    ANIMAL_ACTION(cat, serve);

    /* could be:
         animal->makeSound(animal);
       but it implies having a "vtable" in each animal instance */

    return 0;
}
