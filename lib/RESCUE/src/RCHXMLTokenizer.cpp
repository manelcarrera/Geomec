/**************************************************************************

 RCHXMLTokenizer.cpp

 Simple XML Tokenizer.

 Rod Hanks, July, 2007

**************************************************************************/
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "RCHXMLTokenizer.h"

RCHXMLTokenizer::RCHXMLTokenizer(FILE *inputFileIn)
{
  inputFile = inputFileIn;
  needSpecialTerminator = FALSE;
  returnSpecialTerminator = FALSE;
  hitEOF = FALSE;
  inTag = FALSE;
  FillBuffer();
}

void RCHXMLTokenizer::FillBuffer()
{
  readPos = 128;
  writePos = 128;
  while (writePos < 512 && hitEOF == FALSE)
  {
    int nextChar = fgetc(inputFile);
    buffer[writePos++] = nextChar;
    if (nextChar < 0)
    {
      hitEOF = TRUE;
    }
  }
}

RESCUECHAR RCHXMLTokenizer::NextByte()
{
  if (readPos < writePos)
  {
    return buffer[readPos++];
  }
  else if (hitEOF)
  {
    return -1;
  }
  else
  {
    FillBuffer();
    return NextByte();
  }
}

void RCHXMLTokenizer::PushByte(int toPush)
{
  if (readPos > 0)
  {
    buffer[--readPos] = toPush;
  }
/*
  This limits me to 128 push-backs, which is way more than I ever need.
*/
}

void RCHXMLTokenizer::ConsumeTag()
{
  while (inTag == TRUE)
  {
    NextToken();
  }
}

RESCUEBOOL RCHXMLTokenizer::TokenIsTag(const RESCUECHAR *tag)
{
  RESCUEBOOL myReturn = FALSE;
  RESCUECHAR *compare = nextToken.String();
  if (*compare == '<')
  {
    compare++;
    size_t tLen = strlen(tag);
    if (strncmp(tag, compare, tLen) == 0)
    {
      compare += tLen;
      if (*compare == '>' || *compare == 0)
      {
        myReturn = TRUE;
      }
    }
  }
  return myReturn;
}

RESCUECHAR *RCHXMLTokenizer::UnquotedToken()
{
  NextToken();
  RESCUEINT64 tLen = nextToken.length64();
  if (tLen > 0)
  {
    RESCUECHAR *firstChar = nextToken.String();
    if (*firstChar == '"' || *firstChar == '\'')
    {
      memcpy(firstChar, firstChar + 1, (size_t) (tLen - 2));
      firstChar[tLen - 2] = 0;
    }
  }
  return nextToken.String();
}

RESCUECHAR *RCHXMLTokenizer::NextToken()
{
  nextToken = "";
  if (returnSpecialTerminator)
  {
    nextToken = "]]>";
    returnSpecialTerminator = FALSE;
  }
  else
  {
    int tokenLen = 0;
    int firstChar = NextByte();
    if (needSpecialTerminator)
    {
      int tCase = 0;
      int nextChar = firstChar;
      while (tCase < 3 && nextChar >= 0)
      {
        nextToken.AddTo((RESCUECHAR) nextChar);
        tokenLen++;
        if ((tCase == 0 || tCase == 1) && nextChar == ']')
        {
          tCase++;
        }
        else if (tCase == 2 && nextChar == '>')
        {
          tCase++;
        }
        if (tCase < 3)
        {
          nextChar = NextByte();
        }
      }
      nextToken.String()[tokenLen - 3] = 0;
      returnSpecialTerminator = TRUE;
      needSpecialTerminator = FALSE;
    }
    else
    {
      while (firstChar == ' ' || firstChar == '\t' || firstChar == '\r' || firstChar == '\n')
      {
        firstChar = NextByte();
      }
      if (firstChar > 0)
      {
        nextToken.AddTo((RESCUECHAR) firstChar);
        if (firstChar != '=')
        {
          RESCUEBOOL pleaseContinue = TRUE;
          if (firstChar == '<')
          {
            inTag = TRUE;
          }
          else if (inTag && firstChar == '>')
          {
            inTag = FALSE;
            pleaseContinue = FALSE;
          }
          if (pleaseContinue)
          {
            int nextChar = NextByte();
            while (nextChar > 0 && pleaseContinue == TRUE)
            {
              if (firstChar == '"' || firstChar == '\'')
              {
                nextToken.AddTo((RESCUECHAR) nextChar);
                if (nextChar == firstChar)
                {
                  pleaseContinue = FALSE;
                }
                else
                {
                  nextChar = NextByte();
                }
              }
              else if (inTag && nextChar == '>')
              {
                nextToken.AddTo((RESCUECHAR) nextChar);
                pleaseContinue = FALSE;
                inTag = FALSE;
              }
              else if (inTag && (nextChar == ' ' || nextChar == '\t' || nextChar == '\r' || nextChar == '\n' || nextChar == '='))
              {
                PushByte(nextChar);
                pleaseContinue = FALSE;
              }
              else if (inTag == FALSE && nextChar == '<')
              {
                PushByte(nextChar);
                pleaseContinue = FALSE;
              }
              else
              {
                nextToken.AddTo((RESCUECHAR) nextChar);
                nextChar = NextByte();
              }
            }
          }
        }
      }
    }
  }
  if (nextToken.length64() > 0)
  {
    return nextToken.String();
  }
  else
  {
    return 0;
  }
}



