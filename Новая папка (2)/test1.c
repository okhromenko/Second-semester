#include<stdio.h>
#include<stdlib.h>
#include "lin.c"
#include "linear_sequence.h"


int main(){

    LSQ_HandleT *array = LSQ_CreateSequence();

    LSQ_InsertFrontElement(array, 10);

    LSQ_InsertRearElement(array, 12);

    LSQ_IteratorT *iterator = LSQ_GetFrontElement(array);
    LSQ_InsertElementBeforeGiven(iterator, 25);

    LSQ_InsertRearElement(array, 15);


//    LSQ_IteratorT *iterator = LSQ_GetFrontElement(array);
    printf("1 element: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("cardinality: %d\n", LSQ_GetSize(array));

    LSQ_ShiftPosition(iterator, 1);
    printf("2 element: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("cardinality: %d\n", LSQ_GetSize(array));

    LSQ_ShiftPosition(iterator, 1);
    printf("3 element: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("cardinality: %d\n", LSQ_GetSize(array));

    LSQ_ShiftPosition(iterator, 1);
    printf("4 element: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("cardinality: %d\n", LSQ_GetSize(array));

    printf("-----------------------------\n");
    LSQ_RewindOneElement(iterator);

    LSQ_DeleteRearElement(array);
    printf("Delete 2 element:\n");
    printf("2 element: %d, ", *LSQ_DereferenceIterator(iterator));
    printf("cardinality: %d\n\n", LSQ_GetSize(array));

    for (int i = 0; i < 100; ++i)
        LSQ_InsertRearElement(array, i);
    printf("cardinality: %d\n", LSQ_GetSize(array));

    LSQ_SetPosition(iterator, 20);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("20 element: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_SetPosition(iterator, 50);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("50 element: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_RewindOneElement(iterator);
    if (LSQ_IsIteratorDereferencable(iterator))
        printf("Rewind one element: %d\n", *LSQ_DereferenceIterator(iterator));

    LSQ_InsertElementBeforeGiven(iterator, -1);
    printf("Inserted element: %d\n\n", *LSQ_DereferenceIterator(iterator));


    LSQ_DestroyIterator(iterator);
    LSQ_DestroySequence(array);

    return 0;
}
