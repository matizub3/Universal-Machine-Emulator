/**************************************************************
 *
 *                     MEMORYLOADER.C
 *
 *     Date:       04/08/25
 *
 *     Purpose:
 *         Implements segmented memory functionality for the Universal Machine.
 *         Functions include  mapping, unmapping, storing to,
 *         and accessing segments. Manages the recycling of segment IDs.
 *
 **************************************************************/

#include "memoryLoader.h"


/********** newMemory **********
 *
 * Creates a new MemoryLoader object, initializes its segment list and
 * unmapped ID stack, and maps segment 0 with all values initialized to 0.
 *
 * Parameters:
 *      size_t num_of_segments:
 *          The initial capacity of the segments sequence.
 *
 *      size_t segmentSize:
 *          The number of words for segment 0.
 *
 * Return:
 *      MemoryLoader
 *          A pointer to the newly allocated MemoryLoader structure, which 
 *          contains a sequence of segments and a stack for unmapped IDs.
 *
 * Expects:
 *      - segmentSize is >= 0.
 *
 * Notes:
 *      - Segment 0 is always created at initialization.
 ************************/
MemoryLoader newMemory(size_t num_of_segments, size_t segmentSize)
{
        MemoryLoader ml = ALLOC(sizeof(*ml));
        ml->segments = Seq_new(num_of_segments);
        ml->unmapped_ids = Stack_new();

        /* Create segment 0 */
        Segment *segment0 = ALLOC(sizeof(*segment0));
        segment0->words = Seq_new(segmentSize);
        
        /* Initialize each word in segment0 to 0 */
        for (size_t index = 0; index < segmentSize; index++) {

                uint32_t *word = ALLOC(sizeof(*word));
                *word = 0;

                Seq_addhi(segment0->words, word);
        }

        segment0->size = Seq_length(segment0->words);
        
        /* Add segment 0 to the segments sequence */
        Seq_addhi(ml->segments, segment0);

        return ml;
}


/********** freeMemory **********
 *
 * Frees all memory allocated in MemoryLoader, including all segments,
 * their words, the unmapped ID stack, and the memory loader.
 *
 * Parameters:
 *      MemoryLoader ml:
 *          Pointer to the MemoryLoader to free.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - ml is non-NULL.
 ************************/
void freeMemory(MemoryLoader ml) 
{
        
        assert(ml != NULL);

        /*Free all segments and their words*/
        for (int i = 0; i < Seq_length(ml->segments); i++) {

                Segment *segment = Seq_get(ml->segments, i);

                if (segment != NULL) {

                        for (int j = 0; j < Seq_length(segment->words); j++) {
                                uint32_t *word = Seq_get(segment->words, j);
                                FREE(word);
                        }

                        Seq_free(&(segment->words));
                        FREE(segment);

                }
        }

        /*Free the segments sequence*/
        Seq_free(&(ml->segments));

        /*Free the unmapped IDs stack*/
        while (!Stack_empty(ml->unmapped_ids)) {
                int *id = Stack_pop(ml->unmapped_ids);
                FREE(id);
        }

        /*Free the stack itself*/
        Stack_free(&(ml->unmapped_ids));

        /*Free the memory loader*/
        FREE(ml);
}

/********** mapSegment **********
 *
 * Maps a new segment with the given size and returns its segment ID.
 * If there are unmapped IDs available, it reuses one from the stack. Otherwise,
 * it adds a new segment to the end of the segment sequence .
 *
 * Parameters:
 *      MemoryLoader ml:
 *          The memory struct to map the segment in.
 *
 *      uint32_t size:
 *          The number of 32-bit words to allocate for the segment.
 *
 * Return:
 *      uint32_t:
 *          The ID of the newly mapped segment.
 *
 * Expects:
 *      - ml is non-NULL.
 ************************/
uint32_t mapSegment(MemoryLoader ml, uint32_t size)
{
        
        assert(ml != NULL);
        Segment *segment = ALLOC(sizeof(*segment));
        segment->words = Seq_new((int) size);
        
        /* Initialize every segment place to 0 */
        for (int index = 0; index < (int) size; index++) {

                uint32_t *new_word = ALLOC(sizeof(*new_word));
                *new_word = 0;

                Seq_addhi(segment->words, new_word);

        }

        /* Set the size of the segment */
        segment->size = Seq_length(segment->words);

        /* Check if there are unmapped IDs available */
        int segId;

        if (!Stack_empty(ml->unmapped_ids)) {

                int *segId_p = (int *) Stack_pop(ml->unmapped_ids);
                segId = *segId_p;
                FREE(segId_p);

                Seq_put(ml->segments, segId, segment);

        } else {
                segId = Seq_length(ml->segments);
                Seq_addhi(ml->segments, segment);
        }
        
        return segId;
}

/********** unmapSegment **********
 *
 * Unmaps the segment with the given ID. Frees all the memory
 * and stores the ID for reuse.
 *
 * Parameters:
 *      MemoryLoader ml:
 *          The memory struct containing the segments.
 *
 *      int segmentId:
 *          The ID of the segment to unmap.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - The segment must be currently mapped and not be NULL.
 ************************/
void unmapSegment(MemoryLoader ml, int segmentId) 
{

        Segment *segment = Seq_get(ml->segments, segmentId);
        
        assert(segment != NULL);

        /*Free the words in the segments*/
        for (int index = 0; index < Seq_length(segment->words); index++) {

                uint32_t *value = (uint32_t *)Seq_get(segment->words, index);
                FREE(value);

        }

        /*free actual segment*/
        Seq_free(&(segment->words));
        FREE(segment);

        Seq_put(ml->segments, segmentId, NULL);

        /*Put segment ID in stack for reuse*/
        int *unmappedId = ALLOC(sizeof(*unmappedId));
        *unmappedId = segmentId;

        Stack_push(ml->unmapped_ids, (void *) unmappedId);
}


/********** getWord **********
 *
 * Returns a pointer to the 32-bit word stored at a specific offset
 * within the given segment.
 *
 * Parameters:
 *      MemoryLoader ml:
 *          The memory struct to access.
 *
 *      int segmentId:
 *          The ID of the segment.
 *
 *      uint32_t offset
 *          The index within the segment to retrieve.
 *
 * Return:
 *      uint32_t*:
 *          A pointer to the word at the given location.
 *
 * Expects:
 *      - segmentId gets a currently mapped segment.
 *      - offset < length of segment.
 ************************/
uint32_t *getWord(MemoryLoader ml, int segmentId, uint32_t offset) 
{
    
        Segment *segment = (Segment *)Seq_get(ml->segments, segmentId);

        assert(segment != NULL);
        assert(offset < (uint32_t)Seq_length(segment->words));

        uint32_t *word = (uint32_t *)Seq_get(segment->words, offset);

        return word;
}

/********** store **********
 *
 * Stores a new value at a specified offset in a segment, replacing the
 * current word and freeing the old one.
 *
 * Parameters:
 *      MemoryLoader ml
 *          The memory manager containing the segment.
 *
 *      int segmentId
 *          The ID of the segment to write to.
 *
 *      uint32_t offset
 *          The index within the segment to write to.
 *
 *      uint32_t value
 *          The value to store.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - segmentId refers to a mapped segment.
 *      - offset is within bounds.
 ************************/
void store(MemoryLoader ml, int segmentId, uint32_t offset, uint32_t value) 
{

        Segment *segment = Seq_get(ml->segments, segmentId);
         /* Check if the segment is valid */
        assert(segment != NULL);
        assert(offset < (uint32_t)Seq_length(segment->words));
        /*Allocate memory for the new value*/
        uint32_t *storedValue = ALLOC(sizeof(*storedValue));

        *storedValue = value;
        /* Store the new value at the specified offset */
        uint32_t *oldValue = (uint32_t*)Seq_put(segment->words, offset, 
                                                                storedValue);
        FREE(oldValue);
}

/********** add **********
 *
 * Adds a value to the end of the specified segment.
 *
 * Parameters:
 *      MemoryLoader ml:
 *          The memory struct containing the segment.
 *
 *      int segmentId:
 *          The ID of the segment.
 *
 *      uint32_t value:
 *          The value to add.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - segmentId refers to a currently mapped segment.
 ************************/
void add(MemoryLoader ml, int segmentId, uint32_t value)
{
        /* Get the segment we want to add to */
        Segment *segment = Seq_get(ml->segments, segmentId);

        assert(segment != NULL);

        /*Allocate memory for the new word*/
        uint32_t *word = ALLOC(sizeof(*word));
        *word = value;

        /* Add the new word to the end of the segment */
        Seq_addhi(segment->words, word);

        segment->size = Seq_length(segment->words);
}

/********** getSegmentLength **********
 *
 * Returns the number of words currently stored in a segment.
 *
 * Parameters:
 *      MemoryLoader ml:
 *          The memory struct containing the segment.
 *
 *      int segmentId:
 *          The ID of the segment.
 *
 * Return:
 *      uint32_t:
 *          The number of words in the segment.
 *
 * Expects:
 *      - segmentId refers to a currently mapped segment.
 ************************/
uint32_t getSegmentLength(MemoryLoader ml, int segmentId) {

        /* Get segment which corresponds to the segment ID */
        Segment *segment = Seq_get(ml->segments, segmentId);

        assert(segment != NULL);

        return Seq_length(segment->words);
}
