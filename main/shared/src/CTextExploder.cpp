// ===========================================================================
//  CTextExploder.cp                     1994 Metrowerks Inc. All rights reserved.
// ===========================================================================

#include "CTextExploder.h"
//#include "NeoResourceConstants.h"
#include "NeoException.h"
#include "IndexerCodes.h"
#include "EndianSwap.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>


// === Constants ===
/*
 * disabling for now since these are undefined
const RGBColor RGBBlack = {0,0,0};
const RGBColor RGBRed={65535,0,0};
const RGBColor RGBGreen={0,65535,0};
const RGBColor RGBBlue={0,0,65535};
const RGBColor RGBCyan={0,65535,65535};
const RGBColor RGBMagenta={65535,0,65535};
const RGBColor RGBYellow={65535,65535,0};

const RGBColor  ExtAmbColor = RGBGreen;
const RGBColor  IntAmbColor = RGBCyan;
const RGBColor  HitColor    = RGBRed;

const TextStyle HIT_STYLE = {0, bold, '\0', 12, {65535, 0, 0}};
const short HIT_MASK = weDoColor+weDoFace;  // wedoFont, weDoSize
const TextStyle INT_AMB_STYLE = {0, bold, '\0', 12, {65535, 0, 65535}};
const TextStyle EXT_AMB_STYLE = {0, bold, '\0', 12, {0, 0, 65535}};
const TextStyle BOTH_AMB_STYLE = {0, bold, '\0', 12, {65535, 0, 65535}};


//const TextStyle FOOTNOTE_STYLE = {0, outline, '\0', 12, {65535, 0, 0}};
//const short FOOTNOTE_MASK = weDoFont+weDoFace;

const TextStyle PASSAGENUM_STYLE = {0, bold+underline, '\0', 12, {65535, 0, 0}};
const short PASSAGENUM_MASK = weDoFace;

const TextStyle QUOTE_STYLE = {0, italic, '\0', 12, {65535, 0, 0}};
const short QUOTE_MASK = weDoFace;
*/

//==================================================================================

CTextExploder::CTextExploder()
{
    codeText = NULL;
    codeTextLen = 0;
    realTextPtr = NULL;
    realTextLen = 0;
    footnoteCount = numberCount = 0;
}                                    

//==================================================================================

CTextExploder::~CTextExploder()
{
    if (codeText)
        delete[] codeText;
    if (realTextPtr)
        delete[] realTextPtr; // XXX: should this be delete[]?
}

//==================================================================================

void CTextExploder::Setup(CTextFetch *t, CGiant *g, CHitOffsetList *l) 
{
    textFetcher = t;
    giantTable = g;
//  textPane = e;
    ptrToList = l;
}

//==================================================================================

/*void CTextExploder::HighlightHit(short wordNum)
{
}*/

//==================================================================================


void CTextExploder::LoadPassageNumber(hit theHit) {

    if (codeText) {         // destroy old passage if it's in memory
        delete[] codeText; codeText = NULL; }
    if (realTextPtr) {
        delete[] realTextPtr; realTextPtr = NULL; }
            
    codeText = textFetcher->ReadPassage(theHit.volume, theHit.passage, codeTextLen);
    if (!codeText)
        throw CException("Problamo!");
    codeTextLen /= sizeof(int);
    realTextPtr = new char[codeTextLen*15];
    if (!realTextPtr) {
        throw CMemoryFullError("Out of memory");
    }

    
//  quoteCount = 0;
    footnoteCount = 0;
    numberCount = 0;

    realTextLen = 0;
    realTextPtr[0] = '\0';
    long j;
    for (j = 0; j < codeTextLen; j++) {
#ifndef __BIG_ENDIAN__
        endian_swap((unsigned int&)codeText[j]);
#endif
        ExplodeWord(j);
    }
    realTextPtr[realTextLen] = '\0';

    // shrink to actual size
    char *tempTextPtr = new char[realTextLen+1];
    if (!tempTextPtr) {
        throw CMemoryFullError("Out of memory");
    }
    for (j = 0; j < realTextLen+1; j++) {
        tempTextPtr[j] = realTextPtr[j];
    }
    delete[] realTextPtr;
    realTextPtr = tempTextPtr;
}

void CTextExploder::PrintPassageNumber(hit theHit) {
    LoadPassageNumber(theHit);
    if (realTextPtr) {
        printf("%s", realTextPtr);
    }
}

cstr CTextExploder::RetrievePassageNumber(hit theHit) {
    static cstr thePassage;
    LoadPassageNumber(theHit);
    thePassage.text = realTextPtr;
    thePassage.length = realTextLen;
    return thePassage;
}

// ###########################################################################################

void CTextExploder::FeedPassageToWASTE(long selStart, long selEnd) {

/* XXX: none of this is useful at the moment

//  textPane->Hide();
//  textPane->Disable();
    textPane->InhibitDrawing();
    Try_ {          // memory seems to run out when the system has none left, even if the app does
        textPane->Insert(realTextHandle, realTextLen, NULL, NULL, TRUE);
    } Catch_ (err) {                                    
        SignalPStr_( "\pThe system is out of memory. Please close another application." );
//      throw CMemoryFullError("System Memory Full");
    } EndCatch_;

    if (ptrToList != NULL) {
        short listSize = ptrToList->GetListSize();
        long start, end;
        short ambCode;
        short j;
        for (j = 0; j < listSize; j++) {
            start = ptrToList->GetStartOffsetIdx(j);
            end = ptrToList->GetEndOffsetIdx(j);
            ambCode = ptrToList->GetAmbCodeIdx(j);
    
            textPane->SetSelection(start, end);
            switch (ambCode) {
                case 0 : textPane->SetStyle(HIT_MASK, &HIT_STYLE); break;
                case 1 : textPane->SetStyle(HIT_MASK, &EXT_AMB_STYLE); break;
                case 2 : textPane->SetStyle(HIT_MASK, &INT_AMB_STYLE); break;
                case 3 : textPane->SetStyle(HIT_MASK, &BOTH_AMB_STYLE); break;
            }
        }

    }
*/
/*
 * XXX: may need this later
    TextStyle   style;
    short       fontNum;
    GetFNum("\pcourier", &fontNum);
    style.tsFont = fontNum;
    style.tsFace = outline;
*/

/*
 * TODO: reimplement this in a modular way
    for (short j = 1; j < footnoteCount; j+=2) {
        textPane->SetSelection(footnotePositions[j-1], footnotePositions[j]);
        textPane->SetStyle(weDoFont+weDoFace, &style);
    }       
*/

//  for (short j = 1; j < numberCount; j+=2) {
//      textPane->SetSelection(numberPositions[j-1], numberPositions[j]);
//      textPane->SetStyle(PASSAGENUM_MASK, &PASSAGENUM_STYLE);
//  }       

//  for (j = 1; j < quoteCount; j+=2) {
//      textPane->SetSelection(quotePositions[j-1], quotePositions[j]);
//      textPane->SetStyle(QUOTE_MASK, &QUOTE_STYLE);
//  }
    
/*
 * XXX: may need this later
    textPane->SetSelection(selStart, selEnd);
    textPane->AllowDrawing();
*/

//  textPane->Show();
}

// ###########################################################################################

void CTextExploder::ExplodeWord(long wordNum) {
    
        static Boolean  numberIsNext = FALSE;   // tells that the next thing is a straight #
        Boolean         caps, allcaps;
        char            *word, c;
        short           j;
        Boolean isAmb = FALSE;
        int wordCode = codeText[wordNum];
        wordNum++; // modify since hits start at word #1, but code passage at position 0
        Boolean thisOneIsAHit = (ptrToList == NULL? FALSE : ptrToList->CheckFor(wordNum));
        short           ambValue;
    
        unsigned int    strippedCode, code;
        code = wordCode;                        // bit shifting is messed up when signed longs are used
    
        if (numberIsNext) {
            char buffer[10];
            sprintf(buffer, "%u", code);
            int j;
            for (j = 0; buffer[j] != '\0'; j++)
                realTextPtr[realTextLen++] = buffer[j];
            numberIsNext = FALSE;
    //      realTextPtr[realTextLen] = '\0';
            if (footnoteCount%2 == 1) {                 // finish storing footnote # offset
                footnotePositions[footnoteCount++] = realTextLen;
                if (footnoteCount >= MAX_FOOTNOTES_PER_PASSAGE*2)
                    throw CFatalError("All footnotes full!");
            }
//          if (numberCount%2 == 1) {                   // finish storing passage number # offset
//              realTextPtr[realTextLen++] = ']';
//              numberPositions[numberCount++] = realTextLen;
//              if (numberCount >= MAX_PASSAGENUMS_PER_PASSAGE*2)
//                  throw CFatalError("All passage nums full!");
//          }
            
            return;
        }
    
        if ((code >= I_BASE) && (code <= I_ASTERIX)) {
            switch (code) {
                case I_PASSAGE_NUM: c = '#';    numberIsNext = TRUE;
                        numberPositions[numberCount++] = realTextLen; break;
                case I_SUB_PASSAGE: c = '[';    numberIsNext = TRUE;    
                        numberPositions[numberCount++] = realTextLen; break;
                case I_FOOTNOTE_REF:c = ' ';    numberIsNext = TRUE;    
                        footnotePositions[footnoteCount++] = realTextLen; break;
                case I_FOOT_START:  c = '@';    break;
                case I_FOOT_END:    c = '$';    break;
                case I_NEW_LINE:    c = '\n';   break;
                case I_TAB:         c = '\t';   break;
                case I_L_SQUAREB:   c = '[';    break;
                case I_R_SQUAREB:   c = ']';    break;
                case I_DASH:        c = '-';    break;
                case I_ASTERIX:     c = '*';    break;
            }
            if (code != I_FOOTNOTE_REF)
                realTextPtr[realTextLen++] = c;
    //      realTextPtr[realTextLen] = '\0';
            return;
        }       
        if (code & M_L_QUOTE)   realTextPtr[realTextLen++] = '"';
    //                           quotePositions[quoteCount++] = realTextLen-1; }
        if (code & M_L_PAREN)   realTextPtr[realTextLen++] = '(';
    
    
        strippedCode = (code << 15) >> 15;  // kill the leftmost 15 bits
        if (strippedCode == I_MISSINGWORD)
            word = "{?}";
        else if (strippedCode == I_BLANKWORD)
            word = "";
        else {
            word = giantTable->GetString(strippedCode);
            if (thisOneIsAHit) {
                isAmb = giantTable->GetRoseCode(strippedCode).IsExtAmb();
                ambValue = (isAmb? 1 : 0);
                if (giantTable->GetRoseCode(strippedCode).IsIntAmb())
                    ambValue += 2;
                ptrToList->SetAmbCode(wordNum, ambValue);
                ptrToList->SetStartOffset(wordNum, (int)realTextLen);
            }

/*          else {      // erase this whole section
                static long lastPlace = 0;
                if (wordNum != lastPlace+1)
                    realTextPtr[realTextLen++] = '*';
                lastPlace = wordNum;
            }       // end section
*/

        }
        
    //  if (isAmb)
    //      styleSequence[styleCount].Set(&AMB_STYLE, AMB_MASK, realTextLen, 0);
        
        realTextPtr[realTextLen] = '\0';
        caps = (code & M_CAPITAL) > 0;
        allcaps = (code & M_ALL_CAPS) > 0;
        if (!(caps) && !(allcaps)) {
            for (j = 0; word[j] != '\0'; j++)
                realTextPtr[realTextLen++] = word[j];
        }
        else {
            realTextPtr[realTextLen++] = (char)(toupper(word[0]));
            for (j = 1; word[j] != '\0'; j++) {
                if (allcaps)
                    realTextPtr[realTextLen++] = (char)(toupper(word[j]));
                else
                    realTextPtr[realTextLen++] = word[j];
            }
        }
    
    if (thisOneIsAHit)
        ptrToList->SetEndOffset(wordNum, (int)realTextLen);  
    //  if (isAmb)
    //      styleSequence[styleCount++].end = realTextLen;
        
        if (code & M_COMMA)             realTextPtr[realTextLen++] = ',';
        else if (code & M_PERIOD)       realTextPtr[realTextLen++] = '.';
        else if (code & M_COLON)        realTextPtr[realTextLen++] = ':';
        else if (code & M_SEMI_COLON)   realTextPtr[realTextLen++] = ';';
        else if (code & M_QUESTIONM)    realTextPtr[realTextLen++] = '?';
        
        if (code & M_R_QUOTE)           realTextPtr[realTextLen++] = '"';
    //                           quotePositions[quoteCount++] = realTextLen-1;
    //                           if (quoteCount >= MAX_PUNCS) quoteCount-=2; }
        else if (code & M_R_PAREN)      realTextPtr[realTextLen++] = ')';
        
        if (code & M_SPACE)             realTextPtr[realTextLen++] = ' ';
        
        if (code & M_NEWLINE)           realTextPtr[realTextLen++] = '\r';
    //  realTextPtr[realTextLen] = '\0';

//  }
}


// ###########################################################################################



