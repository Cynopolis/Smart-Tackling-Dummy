/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include <Arduino.h>

#define MAX_STREAM_LENGTH 100

template <typename ItemType>
class DataStream{
    public:
        DataStream() = default;
        ~DataStream() = default;

        /**
         * @brief insert an item into the stream at the given index
         * @param item the item to insert
         * @param index the index to insert the item at
         * @returns None.
         */
        void insert(ItemType item, unsigned int index);

        /**
         * @brief prepend an item to the beggining of the stream
         * @param item the item to prepend
         * @returns None.
         */
        void prepend(ItemType item);

        /**
         * @brief get the item at the given index
         * @param index the index to get the item at
         * @returns the item at the given index
         */
        ItemType peek(unsigned int index);

        /**
         * @brief pop the item at the given index
         * @param index the index to pop the item at
         * @returns the item at the given index
         */
        ItemType pop(unsigned int index = 0);

        /**
         * @brief get the length of the stream
         * @returns the length of the stream
         */
        unsigned int size();

        /**
         * @brief get the maximum length of the stream
         * @returns the maximum length of the stream
         */
        unsigned int maxLength();

        /**
         * @brief set a new maximum length for the stream
         * @param newMaxLength the new maximum length for the stream
         * @returns None.
         */
        void setMaxLength(unsigned int newMaxLength);

        /**
         * @brief set the datastream header
         * @param header a pointer to an array of characters
         * @param headerLength the number of characters in the header
        */
        void setHeader(char * header, unsigned int headerLength);

        /**
        * @brief get a pointer to the header
        * @return a pointer to the header
        */
        char * getHeader();

        /**
         * @brief get the length of the header
         * @return an unsigned int that is the length of the header array
         */
        unsigned int getHeaderLength();

        /**
         * @brief set the initialized flag
         * @param isInitialized the new value for the initialized flag
        */
        void setInitialized(bool isInitialized){
            this->isInitialized = isInitialized;
        };

        /**
         * @brief get the initialized flag
         * @return the initialized flag
        */
        bool getInitialized(){
            return this->isInitialized;
        };

    private:
        unsigned int currentSize = 0;
        ItemType stream[MAX_STREAM_LENGTH];
        char * header = nullptr;
        unsigned int headerLength = 0;
        bool isInitialized = false;

        /**
         * @brief shift all items in the stream to the right by one
         * @param index the index to start shifting at
         * @returns None.
         * @post if the stream is full, the last item will be removed
         */
        void shiftRight(unsigned int index);

        /**
         * @brief shift all items in the stream to the left by one
         * @param index the index to start shifting at
         * @returns None.
         * @post this effectively removes the item at the given index
         */
        void shiftLeft(unsigned int index);

};

//#include "DataStream.h"

template <typename ItemType>
void DataStream<ItemType>::shiftRight(unsigned int index){
    // shift all items to the right by one
    for(unsigned int i = this->currentSize-1; i > index; i--){
        this->stream[i] = this->stream[i - 1];
    }
}

template <typename ItemType>
void DataStream<ItemType>::shiftLeft(unsigned int index){
    // shift all items to the left by one
    for(unsigned int i = index; i < this->currentSize; i++){
        this->stream[i] = this->stream[i + 1];
    }
}

template <typename ItemType>
void DataStream<ItemType>::insert(ItemType item, unsigned int index){
    // don't add the item if the index is out of bounds
    if(index >= MAX_STREAM_LENGTH - 1){
        return;
    }

    // if the index is greater than the current length of the stream, add the item to the end
    if(index >= this->currentSize){
        this->stream[this->currentSize] = item;
    }
    // otherwise, insert the item at the given index
    else{
        shiftRight(index);
        this->stream[index] = item;
    }

    // increment the current length of the stream unless it is already at the maximum length
    if(this->currentSize < MAX_STREAM_LENGTH){
        this->currentSize++;
    }
}

template <typename ItemType>
void DataStream<ItemType>::prepend(ItemType item){
    this->insert(item, 0);
}

template <typename ItemType>
ItemType DataStream<ItemType>::peek(unsigned int index){
    // if the index is out of bounds, return the last item in the stream
    if(index >= this->currentSize-1){
        return this->stream[this->currentSize-1];
    }
    return this->stream[index];
}

template <typename ItemType>
ItemType DataStream<ItemType>::pop(unsigned int index){
    // if the index is out of bounds, return the last item in the stream
    if(index >= this->size()){
        return pop();
    }
    
    ItemType item = this->stream[index];
    shiftLeft(index);
    this->currentSize--;
    return item;
}

template <typename ItemType>
unsigned int DataStream<ItemType>::size(){
    return this->currentSize;
}

template <typename ItemType>
unsigned int DataStream<ItemType>::maxLength(){
    return MAX_STREAM_LENGTH;
}

template <typename ItemType>
void DataStream<ItemType>::setHeader(char * header, unsigned int headerLength){
    this->header = header;
    this->headerLength = headerLength;
}

template <typename ItemType>
char * DataStream<ItemType>::getHeader(){
    return this->header;
}

template <typename ItemType>
unsigned int DataStream<ItemType>::getHeaderLength(){
    return this->headerLength;
}