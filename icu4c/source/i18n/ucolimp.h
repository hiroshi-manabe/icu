/*
*******************************************************************************
*
*   Copyright (C) 1998-2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*
* Private implementation header for C collation
*   file name:  ucolimp.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000dec11
*   created by: Vladimir Weinstein
* 
* Modification history
* Date        Name      Comments
* 02/16/2001  synwee    Added UCOL_GETPREVCE for the use in ucoleitr
*                       
*/

#ifndef UCOL_IMP_H
#define UCOL_IMP_H

#include "unicode/ucol.h"
#include "ucmp32.h"
#include "unicode/ures.h"
#include "unicode/udata.h"

/* This is the size of the stack allocated buffer for sortkey generation and similar operations */
/* if it is too small, heap allocation will occur.*/
/* You can expect stack usage in following multiples */
/* getSortkey 6*UCOL_MAX_BUFFER */
/* strcoll 26 - 32 */
/* strcollInc 14 - 46 */
/* you can change this value if you need memory - it will affect the performance, though, since we're going to malloc */
#define UCOL_MAX_BUFFER 256

#define UCOL_NORMALIZATION_GROWTH 2

/* This writable buffer is used if we encounter Thai and need to reorder the string on the fly */
/* Sometimes we already have a writable buffer (like in case of normalized strings). */
/* you can change this value if you need memory - it will affect the performance, though, since we're going to malloc */
#define UCOL_WRITABLE_BUFFER_SIZE 256

/* This is the size of the buffer for expansion CE's */
/* In reality we should not have to deal with expm sequences longer then 16 */
/* you can change this value if you need memory */
/* WARNING THIS BUFFER DOES NOT HAVE MALLOC FALLBACK. If you make it too small, you'll get in trouble */
/* Reasonable small value is around 10, if you don't do Arabic or other funky collations that have long expansion sequence */
/* This is the longest expansion sequence we can handle without bombing out */
#define UCOL_EXPAND_CE_BUFFER_SIZE 64


#define UCOL_RUNTIME_VERSION 1
#define UCOL_BUILDER_VERSION 1

struct collIterate {
  UChar *string; /* Original string */
  UChar *len;   /* Original string length */
  UChar *pos; /* This is position in the string */
  uint32_t *toReturn; /* This is the CE from CEs buffer that should be returned */
  uint32_t *CEpos; /* This is the position to which we have stored processed CEs */
  uint32_t CEs[UCOL_EXPAND_CE_BUFFER_SIZE]; /* This is where we store CEs */
  UBool isThai; /* Have we already encountered a Thai prevowel */
  UBool isWritable; /* is the source buffer writable? */
  UBool JamoSpecial;
  UChar stackWritableBuffer[UCOL_WRITABLE_BUFFER_SIZE]; /* A writable buffer. */
  UChar *writableBuffer;
};

struct UCollationElements
{
  /**
  * Locale specific collator for generating the collation elements
  */
  const UCollator          *collator_;
  /**
  * Normalization mode, not exactly the same as the data in collator_.
  * If collation strength requested is UCOL_IDENTICAL, this modes will be 
  * UNORM_NONE other it follows collator_.
  */
        UNormalizationMode normalization_;
  /**
  * Struct wrapper for source data
  */
        collIterate        iteratordata_;
  /**
  * Source text length
  */
        int32_t            length_;
  /**
  * Indicates if this data has been reset.
  */
        UBool              reset_;
};

struct incrementalContext {
    UCharForwardIterator *source; 
    void *sourceContext;
    UChar currentChar;
    UChar lastChar;
    UChar stackString[UCOL_MAX_BUFFER]; /* Original string */
    UChar *stringP; /* This is position in the string */
    UChar *len;   /* Original string length */
    UChar *capacity; /* This is capacity */
    uint32_t *toReturn; /* This is the CE from CEs buffer that should be returned */
    uint32_t *CEpos; /* This is the position to which we have stored processed CEs */
    uint32_t CEs[UCOL_EXPAND_CE_BUFFER_SIZE]; /* This is where we store CEs */
    UBool JamoSpecial;
    UBool panic; /* can't handle it any more - we have to call the cavalry */
};

/* from coleiterator */
#define UCOL_UNMAPPEDCHARVALUE 0x7fff0000     

#define UCOL_LEVELTERMINATOR 1
/* need look up in .commit() */
#define UCOL_CHARINDEX 0x70000000             
/* Expand index follows */
#define UCOL_EXPANDCHARINDEX 0x7E000000       
/* contract indexes follows */
#define UCOL_CONTRACTCHARINDEX 0x7F000000     
/* unmapped character values */
#define UCOL_UNMAPPED 0xFFFFFFFF              
/* primary strength increment */
#define UCOL_PRIMARYORDERINCREMENT 0x00010000 
/* secondary strength increment */
#define UCOL_SECONDARYORDERINCREMENT 0x00000100 
/* tertiary strength increment */
#define UCOL_TERTIARYORDERINCREMENT 0x00000001 
/* maximum ignorable char order value */
#define UCOL_MAXIGNORABLE 0x00010000          
/* mask off secondary and tertiary order */
#define UCOL_SECONDARYRESETMASK 0x0000ffff    
/* mask off ignorable char order */
#define UCOL_IGNORABLEMASK 0x0000ffff         
/* use only the primary difference */
#define UCOL_PRIMARYDIFFERENCEONLY 0xffff0000 
/* use only the primary and secondary difference */
#define UCOL_SECONDARYDIFFERENCEONLY 0xffffff00  

/* mask off anything but primary order */
#define UCOL_PRIMARYORDERMASK 0xffff0000      
/* mask off anything but long primary order */
#define UCOL_LONGPRIMARYORDERMASK 0xffffff00      
/* mask off anything but secondary order */
#define UCOL_SECONDARYORDERMASK 0x0000ff00    
/* mask off anything but tertiary order */
#define UCOL_TERTIARYORDERMASK 0x000000ff     
/* primary order shift */
#define UCOL_PRIMARYORDERSHIFT 16             
/* long primary order shift */
#define UCOL_LONGPRIMARYORDERSHIFT 8             
/* secondary order shift */
#define UCOL_SECONDARYORDERSHIFT 8            
/* minimum sort key offset */
#define UCOL_SORTKEYOFFSET 2                  
/* Indicates the char is a contract char */
#define UCOL_CONTRACTCHAROVERFLOW 0x7FFFFFFF  


/* starting value for collation elements */
#define UCOL_COLELEMENTSTART 0x02020202       
/* testing mask for primary low element */
#define UCOL_PRIMARYLOWZEROMASK 0x00FF0000    
/* reseting value for secondaries and tertiaries */
#define UCOL_RESETSECONDARYTERTIARY 0x00000202
/* reseting value for tertiaries */
#define UCOL_RESETTERTIARY 0x00000002         

#define UCOL_IGNORABLE 0
#define UCOL_PRIMIGNORABLE 0
#define UCOL_SECIGNORABLE 0
#define UCOL_TERIGNORABLE 0

/* get weights from a CE */
#define UCOL_PRIMARYORDER(order) (isLongPrimary((order))?(((order) & UCOL_LONGPRIMARYORDERMASK)>> UCOL_LONGPRIMARYORDERSHIFT):(((order) & UCOL_PRIMARYORDERMASK)>> UCOL_PRIMARYORDERSHIFT))
#define UCOL_SECONDARYORDER(order) (isLongPrimary((order))?UCOL_UNMARKED:((order) & UCOL_SECONDARYORDERMASK)>> UCOL_SECONDARYORDERSHIFT)
/*#define UCOL_TERTIARYORDER(order) ((order) & UCOL_TERTIARYORDERMASK)*/
#define UCOL_TERTIARYORDER(order) ((order) & 0x3f)

/**
 * Determine if a character is a Thai vowel (which sorts after
 * its base consonant).
 */
#define UCOL_ISTHAIPREVOWEL(ch) ((uint32_t)(ch) - 0xe40) <= (0xe44 - 0xe40)

/**
 * Determine if a character is a Thai base consonant
 */
#define UCOL_ISTHAIBASECONSONANT(ch) ((uint32_t)(ch) - 0xe01) <= (0xe2e - 0xe01)

/* initializes collIterate structure */
/* made as macro to speed up things */
#define init_collIterate(sourceString, sourceLen, s, isSourceWritable) { \
    (s)->string = (s)->pos = (UChar *)(sourceString); \
    (s)->len = (UChar *)(sourceString)+(sourceLen); \
    (s)->CEpos = (s)->toReturn = (s)->CEs; \
	(s)->isThai = TRUE; \
	(s)->isWritable = (isSourceWritable); \
	(s)->writableBuffer = (s)->stackWritableBuffer; \
    (s)->JamoSpecial = FALSE; \
}

/* a macro that gets a simple CE */
/* for more complicated CEs it resorts to getComplicatedCE (what else) */
#define UCOL_GETNEXTCE(order, coll, collationSource, status) {                        \
    if ((collationSource).CEpos > (collationSource).toReturn) {                       \
      (order) = *((collationSource).toReturn++);                                      \
      if((collationSource).CEpos == (collationSource).toReturn) {                     \
        (collationSource).CEpos = (collationSource).toReturn = (collationSource).CEs; \
      }                                                                               \
    } else if((collationSource).pos < (collationSource).len) {                        \
      UChar ch = *(collationSource).pos++;                                              \
      if(ch <= 0xFF) {                                                                \
      (order) = (coll)->latinOneMapping[ch];                                          \
      } else {                                                                        \
      (order) = ucmp32_get((coll)->mapping, ch);                                      \
      }                                                                               \
      if((order) >= UCOL_NOT_FOUND) {                                                 \
        (order) = getSpecialCE((coll), (order), &(collationSource), (status));        \
        if((order) == UCOL_NOT_FOUND) {                                               \
          (order) = ucol_getNextUCA(ch, &(collationSource), (status));                \
        }                                                                             \
      }                                                                               \
    } else {                                                                          \
      (order) = UCOL_NO_MORE_CES;                                                     \
    }                                                                                 \
}

/**
* Macro that gets a simple CE.
* So what it does is that it will first check the expansion buffer. If the 
* expansion buffer is not empty, ie the end pointer to the expansion buffer 
* is different from the start pointer, we return the collation element at the 
* return pointer and decrement it.
* For more complicated CEs it resorts to getComplicatedCE.
*/
#define UCOL_GETPREVCE(order, coll, data, length, status) {                  \
  if ((data).CEpos > (data).CEs) {                                           \
    (data).toReturn --;                                                      \
    (order) = *((data).toReturn);                                            \
    if ((data).CEs == (data).toReturn) {                                     \
      (data).CEpos = (data).toReturn = (data).CEs;                           \
    }                                                                        \
  }                                                                          \
  else {                                                                     \
    if ((data).pos == (data).string || (data).pos == (data).writableBuffer) {\
      (order) = UCOL_NO_MORE_CES;                                            \
    }                                                                        \
    else {                                                                   \
      UChar ch;                                                              \
      (data).pos --;                                                         \
      ch = *((data).pos);                                                    \
      if (ch <= 0xFF) {                                                      \
        (order) = (coll)->latinOneMapping[ch];                               \
      }                                                                      \
      else {                                                                 \
        (order) = ucmp32_get((coll)->mapping, ch);                           \
      }                                                                      \
      if ((order) >= UCOL_NOT_FOUND) {                                       \
        (order) = getSpecialPrevCE((coll), (order), &(data), (length),       \
                                                             (status));      \
        if ((order) == UCOL_NOT_FOUND) {                                     \
          (order) = ucol_getPrevUCA(ch, &(data), (length), (status));        \
        }                                                                    \
      }                                                                      \
    }                                                                        \
  }                                                                          \
}

uint32_t getSpecialCE(const UCollator *coll, uint32_t CE, collIterate *source, UErrorCode *status);
uint32_t getSpecialPrevCE(const UCollator *coll, uint32_t CE, 
                          collIterate *source, uint32_t length, 
                          UErrorCode *status);
U_CFUNC uint32_t ucol_getNextCE(const UCollator *coll, collIterate *collationSource, UErrorCode *status);
uint32_t ucol_getNextUCA(UChar ch, collIterate *collationSource, UErrorCode *status);
uint32_t ucol_getPrevUCA(UChar ch, collIterate *collationSource, 
                         uint32_t length, UErrorCode *status);
void incctx_cleanUpContext(incrementalContext *ctx);
UChar incctx_appendChar(incrementalContext *ctx, UChar c);

/* function used by C++ getCollationKey to prevent restarting the calculation */
U_CFUNC uint8_t *ucol_getSortKeyWithAllocation(const UCollator *coll, 
        const    UChar        *source,
        int32_t            sourceLength, int32_t *resultLen);

/* get some memory */
void *ucol_getABuffer(const UCollator *coll, uint32_t size);

/* worker function for generating sortkeys */
int32_t
ucol_calcSortKey(const    UCollator    *coll,
        const    UChar        *source,
        int32_t        sourceLength,
        uint8_t        **result,
        int32_t        resultLength,
        UBool allocatePrimary,
        UErrorCode *status);

int32_t
ucol_calcSortKeySimpleTertiary(const    UCollator    *coll,
        const    UChar        *source,
        int32_t        sourceLength,
        uint8_t        **result,
        int32_t        resultLength,
        UBool allocatePrimary,
        UErrorCode *status);

/**
 * Makes a copy of the Collator's rule data. The format is
 * that of .col files.
 * 
 * @param length returns the length of the data, in bytes.
 * @param status the error status
 * @return memory, owned by the caller, of size 'length' bytes.
 * @internal INTERNAL USE ONLY
 */
U_CAPI uint8_t *
ucol_cloneRuleData(UCollator *coll, int32_t *length, UErrorCode *status);

#define UCOL_SPECIAL_FLAG 0xF0000000
#define UCOL_TAG_SHIFT 24
#define UCOL_TAG_MASK 0x0F000000
#define INIT_EXP_TABLE_SIZE 1024
#define UCOL_NOT_FOUND 0xF0000000
#define UCOL_EXPANSION 0xF1000000
#define UCOL_CONTRACTION 0xF2000000
#define UCOL_THAI 0xF3000000
#define UCOL_UNMARKED 0x03
#define UCOL_NEW_TERTIARYORDERMASK 0x0000003f     

/* Bit mask for primary collation strength. */
#define UCOL_PRIMARYMASK    0xFFFF0000

/* Bit mask for secondary collation strength. */
#define UCOL_SECONDARYMASK  0x0000FF00

/* Bit mask for tertiary collation strength. */
#define UCOL_TERTIARYMASK   0x000000FF

/** 
 * Internal.
 * This indicates the last element in a UCollationElements has been consumed. 
 * Compare with the UCOL_NULLORDER, UCOL_NULLORDER is returned if error occurs.
 */
#define UCOL_NO_MORE_CES        0x00010101

#define isSpecial(CE) ((((CE)&UCOL_SPECIAL_FLAG)>>28)==0xF)

#define isContinuation(CE) (((CE) & 0xC0) == 0x80)
#define isFlagged(CE) (((CE) & 0x80) == 0x80)
#define isLongPrimary(CE) (((CE) & 0xC0) == 0xC0)

#define getCETag(CE) (((CE)&UCOL_TAG_MASK)>>UCOL_TAG_SHIFT)
#define isContraction(CE) (isSpecial((CE)) && (getCETag((CE)) == CONTRACTION_TAG))
#define constructContractCE(CE) (UCOL_SPECIAL_FLAG | (CONTRACTION_TAG<<UCOL_TAG_SHIFT) | ((CE))&0xFFFFFF)
#define getContractOffset(CE) ((CE)&0xFFFFFF)
#define getExpansionOffset(CE) (((CE)&0x00FFFFF0)>>4)
#define getExpansionCount(CE) ((CE)&0xF)

#define UCA_DATA_TYPE "dat"
#define UCA_DATA_NAME "ucadata"
#define UCOL_CASE_BIT_MASK 0x40
#define UCOL_FLAG_BIT_MASK 0x80
#define INVC_DATA_TYPE "dat"
#define INVC_DATA_NAME "invuca"
#define UCOL_COMMON_TOP2 0x81  
#define UCOL_COMMON_BOT2 0x03  
#define UCOL_TOP_COUNT2  0x40 
#define UCOL_BOT_COUNT2  0x3D

#define UCOL_COMMON_TOP3 0x84 
#define UCOL_COMMON_BOT3 0x03 
#define UCOL_TOP_COUNT3  0x40 
#define UCOL_BOT_COUNT3  0x40

#define UCOL_COMMON2 0x03
#define UCOL_COMMON3 0x03
#define UCOL_COMMON4 0xFF

typedef enum {
    NOT_FOUND_TAG = 0,
    EXPANSION_TAG = 1,
    CONTRACTION_TAG = 2,
    THAI_TAG = 3,
    CHARSET_TAG = 4,
    SURROGATE_TAG = 5,
    CE_TAGS_COUNT
} UColCETags;


typedef struct {
  uint32_t firstHigh; uint32_t lastHigh; uint32_t highCount; uint32_t highByteCount;
  uint32_t firstMid; uint32_t lastMid;   uint32_t midCount; uint32_t midByteCount;
  uint32_t firstLow; uint32_t lastLow;   uint32_t lowCount; uint32_t lowByteCount;
  uint32_t byteSize; uint32_t start; uint32_t limit;
  int32_t count;
  uint32_t current;

} ucolCEGenerator;
typedef struct {
      int32_t size;
      /* all the offsets are in bytes */
      /* to get the address add to the header address and cast properly */
      uint32_t CEindex;          /* uint16_t *CEindex;              */
      uint32_t CEvalues;         /* int32_t *CEvalues;              */
      uint32_t mappingPosition;  /* const uint8_t *mappingPosition; */
      uint32_t expansion;        /* uint32_t *expansion;            */
      uint32_t contractionIndex; /* UChar *contractionIndex;        */
      uint32_t contractionCEs;   /* uint32_t *contractionCEs;       */
      uint32_t contractionSize;  /* needed for various closures */
      uint32_t latinOneMapping;  /* fast track to latin1 chars      */
      CompactIntArray *mapping;
      int32_t CEcount;
      UChar variableTopValue;
      UVersionInfo version;
      UColAttributeValue frenchCollation;
      UColAttributeValue alternateHandling; /* attribute for handling variable elements*/
      UColAttributeValue caseFirst;         /* who goes first, lower case or uppercase */
      UColAttributeValue caseLevel;         /* do we have an extra case level */
      UColAttributeValue normalizationMode; /* attribute for normalization */
      UColAttributeValue strength;          /* attribute for strength */
} UCATableHeader;

typedef struct {
  uint32_t byteSize;
  uint32_t tableSize;
  uint32_t contsSize;
  uint32_t table;
  uint32_t conts;
} InverseTableHeader;

struct UCollator {
    UBool freeOnClose;
    UResourceBundle *rb;
    const UCATableHeader *image;
    CompactIntArray *mapping; 
    const uint32_t *latinOneMapping;
    const uint32_t *expansion;            
    const UChar *contractionIndex;        
    const uint32_t *contractionCEs;       
    const uint8_t *scriptOrder;
    uint8_t variableMax1;
    uint8_t variableMax2;
    UChar variableTopValue;
    UColAttributeValue frenchCollation;
    UColAttributeValue alternateHandling; /* attribute for handling variable elements*/
    UColAttributeValue caseFirst;         /* who goes first, lower case or uppercase */
    UColAttributeValue caseLevel;         /* do we have an extra case level */
    UColAttributeValue normalizationMode; /* attribute for normalization */
    UColAttributeValue strength;          /* attribute for strength */
    UBool variableTopValueisDefault;
    UBool frenchCollationisDefault;
    UBool alternateHandlingisDefault; /* attribute for handling variable elements*/
    UBool caseFirstisDefault;         /* who goes first, lower case or uppercase */
    UBool caseLevelisDefault;         /* do we have an extra case level */
    UBool normalizationModeisDefault; /* attribute for normalization */
    UBool strengthisDefault;          /* attribute for strength */
    UBool hasRealData;                /* some collators have only options, like French, no rules */
                                      /* to speed up things, we use the UCA image, but we don't want it */
                                      /* to run around */
    UChar *rules;
    UChar zero;
    UDataInfo dataInfo;               /* Data info of UCA table */
};

/* various internal functions */
void init_incrementalContext(UCharForwardIterator *source, void *sourceContext, incrementalContext *s);
int32_t ucol_getIncrementalCE(const UCollator *coll, incrementalContext *ctx, UErrorCode *status);
void incctx_cleanUpContext(incrementalContext *ctx);
UChar incctx_appendChar(incrementalContext *ctx, UChar c);
UCollationResult alternateIncrementalProcessing(const UCollator *coll, incrementalContext *srcCtx, incrementalContext *trgCtx);
void ucol_initUCA(UErrorCode *status);
void ucol_initInverseUCA(UErrorCode *status);

UCollator* ucol_initCollator(const UCATableHeader *image, UCollator *fillIn, UErrorCode *status);
void ucol_setOptionsFromHeader(UCollator* result, const UCATableHeader * image, UErrorCode *status);
void ucol_putOptionsToHeader(UCollator* result, UCATableHeader * image, UErrorCode *status);

uint32_t ucol_getIncrementalUCA(UChar ch, incrementalContext *collationSource, UErrorCode *status);
int32_t ucol_getIncrementalSpecialCE(const UCollator *coll, incrementalContext *ctx, UErrorCode *status);

#endif

