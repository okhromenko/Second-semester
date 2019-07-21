#include "linear_sequence.h"
#include <stdio.h>
#include <string.h>

#define TST(T) ((TypeSequence*)(T))
#define TIT(T) ((TypeIterator*)(T))

typedef struct{ 
    LSQ_BaseTypeT *data_;
    LSQ_IntegerIndexT size_, capacity_;
} TypeSequence;

typedef struct{ 
    LSQ_IntegerIndexT index_;
    TypeSequence *sequence_;
} TypeIterator;

LSQ_HandleT LSQ_CreateSequence(void){ 
    TypeSequence *sequence = (TypeSequence*)malloc(sizeof (TypeSequence));
    sequence->size_ = 0;
    sequence->capacity_ = 10;
    sequence->data_ = (LSQ_BaseTypeT*)calloc((size_t)sequence->capacity_, sizeof (LSQ_BaseTypeT));
    if (sequence->data_ == NULL ) return NULL;
    return sequence;
}

extern void LSQ_DestroySequence(LSQ_HandleT handle){
    if(handle == LSQ_HandleInvalid) return;
    free(TST(handle)-> data_);
    free(handle);
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
    if (handle == LSQ_HandleInvalid) return 0;
    return (TST(handle)-> size_);
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
    return iterator != NULL && !LSQ_IsIteratorPastRear(iterator) && !LSQ_IsIteratorBeforeFirst(iterator);
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
    if (iterator == NULL) return 0;
    return TIT(iterator)->index_ >= TIT(iterator)->sequence_->size_;
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
    if (iterator == NULL) return 0;
    return TIT(iterator)->index_ < 0;
}

LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
    if (iterator == NULL) return NULL;
    if (!LSQ_IsIteratorDereferencable(iterator)) return NULL;
    return TIT(iterator)->sequence_->data_ + TIT(iterator)->index_;
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
    TypeIterator *iterator = (TypeIterator *)malloc(sizeof (TypeIterator));
    if(iterator == NULL || handle == LSQ_HandleInvalid) return NULL;

    iterator->index_ = index;
    iterator->sequence_ = TST(handle);
    return (LSQ_IteratorT)iterator;
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
    return  handle == LSQ_HandleInvalid ? NULL : LSQ_GetElementByIndex(handle, 0);
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
    LSQ_IntegerIndexT i = TST(handle)->size_ == 0 ? 0 : TST(handle)->size_;
    return  handle == LSQ_HandleInvalid ? NULL : LSQ_GetElementByIndex(handle, i);
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator){
    if (iterator == NULL) return;
    TIT(iterator)->sequence_ = NULL;
    free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
    if (iterator == NULL) return;
    TIT(iterator)->index_++;
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator){
    if (iterator == NULL) return;
    TIT(iterator)->index_--;
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
    if (iterator == NULL) return;
    TIT(iterator)->index_ += shift;
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
    if (iterator == NULL) return;
    TIT(iterator)->index_ = pos;
}

extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
    TypeIterator *iterator = TIT(LSQ_GetElementByIndex(handle, 0));

    if (iterator == NULL) return;
    LSQ_InsertElementBeforeGiven(iterator, element);
    LSQ_DestroyIterator(iterator);
}

extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
    if (handle == LSQ_HandleInvalid) return;

    TypeIterator *iterator = TIT(LSQ_GetElementByIndex(handle, TST(handle)->size_));
    if (iterator == NULL) return;
    LSQ_InsertElementBeforeGiven(iterator, element);
    LSQ_DestroyIterator(iterator);
}

extern void resizeSequense(LSQ_HandleT handle, int resize){
    if(resize == 1 && TST(handle)->size_ == TST(handle)->capacity_){
        TST(handle)->data_ = (LSQ_BaseTypeT*)realloc(TST(handle)->data_ , sizeof(LSQ_BaseTypeT)*(TST(handle)->capacity_)*2);
        TST(handle)->capacity_ *= 2;
    }
    else{
        if(resize == -1 && TST(handle)->size_ == TST(handle)->capacity_ - TST(handle)->capacity_ % 4){
            TST(handle)->data_ = (LSQ_BaseTypeT*)realloc(TST(handle)->data_, sizeof(LSQ_BaseTypeT)*(TST(handle)->capacity_ - TST(handle)->capacity_ % 4));
            TST(handle)->capacity_ = TST(handle)->capacity_ - TST(handle)->capacity_ % 4;
        }
    }
}

extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){
    if (iterator == NULL || TIT(iterator)->sequence_ == NULL) return;
    resizeSequense(TIT(iterator)->sequence_, 1);
    ++TIT(iterator)->sequence_->size_;
    memmove(TIT(iterator)->sequence_->data_ + TIT(iterator)->index_ + 1,
            TIT(iterator)->sequence_->data_ + TIT(iterator)->index_,
            (TIT(iterator)->sequence_->size_ - TIT(iterator)->index_ - 1)*sizeof(LSQ_BaseTypeT));
    *(TIT(iterator)->sequence_->data_ + TIT(iterator)->index_) = newElement;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle){
    if( handle == LSQ_HandleInvalid )return;
    TypeIterator *iterator = TIT(LSQ_GetElementByIndex(handle, 0));
    if (iterator == NULL) return;
    LSQ_DeleteGivenElement(iterator);
    LSQ_DestroyIterator(iterator);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle){
    if( handle == LSQ_HandleInvalid ) return;

    TypeIterator *iterator = TIT(LSQ_GetElementByIndex(handle,TST(handle)->size_ - 1));
    if (iterator == NULL) return;
    LSQ_DeleteGivenElement(iterator);
    LSQ_DestroyIterator(iterator);
}

extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
    if( iterator == NULL ) return;
    --TIT(iterator)->sequence_->size_;
    memmove(TIT(iterator)->sequence_->data_ + TIT(iterator)->index_ ,
            TIT(iterator)->sequence_->data_ + TIT(iterator)->index_ + 1,
            (TIT(iterator)->sequence_->size_ - TIT(iterator)->index_)*sizeof(LSQ_BaseTypeT));
    (TIT(iterator)->sequence_,-1);
}
