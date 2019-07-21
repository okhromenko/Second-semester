#include<stdio.h>
#include<stdlib.h>
#include "linear_sequence.h"
#include "lin.ñ"

int main(){

    LSQ_HandleT *array = LSQ_CreateSequence();

    LSQ_InsertRearElement(array, 10);
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_InsertRearElement(array, 12);
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_InsertRearElement(array, 15);
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_IteratorT *iterator = LSQ_GetFrontElement(array);
    printf("1 elem: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_ShiftPosition(iterator, 1);
    printf("2 elem: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_DeleteFrontElement(array);
    printf("2 elem: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("%d elems\n\n", LSQ_GetSize(array));

    for (int i = 0; i < 100; ++i)
        LSQ_InsertRearElement(array, i);
    printf("%d elems\n", LSQ_GetSize(array));

    LSQ_SetPosition(iterator, 20);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("20 elem: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_SetPosition(iterator, 50);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("50 elem: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_RewindOneElement(iterator);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("50 - 1 elem: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_InsertElementBeforeGiven(iterator, -1);
    printf("Inserted elem: %d\n\n", *LSQ_DereferenceIterator(iterator));

    LSQ_DestroyIterator(iterator);
    LSQ_DestroySequence(array);

    return 0;
}
