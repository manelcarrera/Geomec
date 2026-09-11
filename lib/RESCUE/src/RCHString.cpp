/**************************************************************************

    RCHString.h

    A simple string class.

 Rod Hanks        April 1996

**************************************************************************/
#include "RCHString.h"
#include "myHeaders.h"
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(WIN32) && !defined(WIN64) && !defined(_WINDOWS)
#define _strdup strdup
#endif // !defined(WIN32) && !defined(WIN64)

RESCUECHAR *RCHString::NonNullString() {
  if (stringValue == 0) {
    allocated = 10;
    stringValue = (RESCUECHAR *)malloc((size_t)allocated);
    stringValue[0] = 0;
  }
  return stringValue;
}

void RCHString::cgiDecode() {
  if (stringValue != 0) {
    unsigned RESCUEINT64 i = 0, j = 0;
    RESCUECHAR buf[3];
    buf[2] = '\0';

    while (i < strlen(stringValue)) {
      if (stringValue[i] == '+') {
        stringValue[j] = ' ';
      } else if (stringValue[i] == '%') {
        buf[0] = stringValue[++i];
        buf[1] = stringValue[++i];
        stringValue[j] = (RESCUECHAR)strtol(buf, 0, 16);
      } else {
        stringValue[j] = stringValue[i];
      }
      i++;
      j++;
    }
    stringValue[j] = '\0';
  }
}

void RCHString::cgiEncode() {
  if (stringValue != 0) {
    RESCUECHAR *buf = stringValue;
    stringValue = 0;
    allocated = 0;
    RESCUECHAR *cPtr = buf;
    while (*cPtr != 0) {
      RESCUECHAR next = *cPtr++;
      if (next == ' ') {
        AddTo('+');
      } else if ((next >= 'A' && next <= 'Z') || (next >= 'a' && next <= 'z') || (next >= '0' && next <= '9') ||
                 next == '.') {
        AddTo(next);
      } else {
        RESCUECHAR toAdd[10];
        sprintf(toAdd, "%%%02x", next);
        AddTo(toAdd);
      }
    }
    free(buf);
    if (stringValue == 0) {
      stringValue = (RESCUECHAR *)malloc((size_t)1);
      allocated = 1;
    }
  }
}

void RCHString::Archive(RescueContext *context, FILE *archiveFile) {
  if (stringValue == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else if (context->BinaryFlag()) {
    RESCUEINT64 length = (RESCUEINT64)strlen(stringValue);
    myfprintf(context, archiveFile, length);
    myfprintf(context, archiveFile, stringValue);
  } else {
    RCHString other = stringValue;
    other.cgiEncode();
    RESCUEINT64 length = other.length64();
    myfprintf(context, archiveFile, length);
    myfprintf(context, archiveFile, other.String());
  }
}

RCHString::RCHString(RescueContext *context, FILE *archiveFile) {
  delimiters = 0;
  stringValue = 0;
  allocated = 0;
  RESCUEINT64 length;
  myfscanf(context, archiveFile, &length);
  if (length > 0) {
    allocated = length + 1;
    stringValue = (RESCUECHAR *)malloc((size_t)allocated);
    myfgets(context, stringValue, allocated, archiveFile);

    if (context->BinaryFlag() == FALSE) {
      cgiDecode();
    }
  }
}

RCHString::~RCHString() {
  if (stringValue != 0) {
    free(stringValue);
  }
  if (delimiters != 0) {
    free(delimiters);
  }
}

RCHString::RCHString(RESCUEINT64 length) {
  allocated = length;
  delimiters = 0;
  stringValue = (RESCUECHAR *)malloc((size_t)length);
  *stringValue = 0;
}

RCHString::RCHString(RESCUEINT32 length) {
  allocated = (RESCUEINT64)length;
  delimiters = 0;
  stringValue = (RESCUECHAR *)malloc((size_t)length);
  *stringValue = 0;
}

RCHString::RCHString() {
  stringValue = 0;
  allocated = 0;
  delimiters = 0;
}

RESCUEINT64 RCHString::length64() {
  if (stringValue == 0) {
    return 0;
  } else {
    return (RESCUEINT64)strlen(stringValue);
  }
}

RESCUEINT32 RCHString::length(RESCUEBOOL throwIfTooBig) {
  if (stringValue == 0) {
    return 0;
  } else {
    RESCUEINT64 count = strlen(stringValue);
    if (count > 2147483647) {
      if (throwIfTooBig) {
        throw "Model is too large to be accessed in 32 bit mode.";
      }
      return 0;
    } else {
      return (RESCUEINT32)count;
    }
  }
}

RCHString::RCHString(const RESCUECHAR *begin) {
  if (begin == 0) {
    stringValue = 0;
    allocated = 0;
  } else {
    stringValue = _strdup(begin);
    allocated = (RESCUEINT64)strlen(begin) + 1;
  }
  delimiters = 0;
}

void RCHString::Accept(RESCUECHAR *value) {
  if (stringValue != 0) {
    free(stringValue);
  }
  stringValue = value;
  allocated = (RESCUEINT64)strlen(value) + 1;
  /*
   We replace our buffer with a new one handed to us.
   The buffer must be a block allocated on the heap.
  */
}

void RCHString::AddTo(RESCUECHAR more) {
  RESCUECHAR add[2];
  add[0] = more;
  add[1] = 0;
  AddTo(add);
}

void RCHString::AddTo(const RESCUECHAR *more) {
  if (more != 0) {
    size_t utilization = 0;
    size_t moreNeeded = strlen(more);
    if (stringValue != 0) {
      utilization = strlen(stringValue) + 1;
    }
    if (utilization + moreNeeded > allocated) {
      allocated += (moreNeeded + 255);
      if (stringValue == 0) {
        stringValue = (RESCUECHAR *)malloc((size_t)allocated);
        stringValue[0] = 0;
      } else {
        stringValue = (RESCUECHAR *)realloc(stringValue, (size_t)allocated);
      }
    }
    if (moreNeeded > 0) {
      strcat(stringValue, more);
    } else if (stringValue == 0) {
      allocated = 20;
      stringValue = (RESCUECHAR *)malloc((size_t)allocated);
      stringValue[0] = 0;
    }
  }
}

void RCHString::AddTo(RCHString &more) {
  if (more.length64() > 0) {
    AddTo(more.String());
  }
}

void RCHString::AddTo(RESCUEINT64 more) {
  RESCUECHAR myString[20];

  sprintf(myString, "%lld", more);
  AddTo(myString);
}

void RCHString::AddTo(RESCUEINT32 more) {
  RESCUECHAR myString[20];

  sprintf(myString, "%d", more);
  AddTo(myString);
}

void RCHString::AddTo(RESCUEDOUBLE more) {
  RESCUECHAR myString[255];

  sprintf(myString, "%.15lf", more);
  AddTo(myString);
}

RESCUEBOOL RCHString::operator!=(const RESCUECHAR *other) {
  RESCUEBOOL myReturn = TRUE;
  if (stringValue != 0) {
    if (strcmp(stringValue, other) == 0) {
      myReturn = FALSE;
    }
  }
  return myReturn;
}

RESCUEBOOL RCHString::operator==(const RESCUECHAR *other) {
  RESCUEBOOL myReturn = FALSE;
  if (stringValue != 0) {
    if (strcmp(stringValue, other) == 0) {
      myReturn = TRUE;
    }
  }
  return myReturn;
}

RESCUEBOOL RCHString::operator!=(RCHString &other) { return (*this) != other.String(); }

RESCUEBOOL RCHString::operator==(RCHString &other) { return (*this) == other.String(); }

RCHString &RCHString::operator<<(RESCUECHAR more) {
  RESCUECHAR localString[2];

  localString[0] = more;
  localString[1] = 0;
  AddTo(localString);
  return *this;
}

RCHString &RCHString::operator<<(const RESCUECHAR *more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator<<(RESCUEINT64 more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator<<(RESCUEINT32 more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator<<(RESCUEDOUBLE more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator<<(RCHString &more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator+=(const RESCUECHAR *more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator+=(RCHString &more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator+=(RESCUEINT64 more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator+=(RESCUEINT32 more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator+=(RESCUEDOUBLE more) {
  AddTo(more);
  return *this;
}

RCHString &RCHString::operator=(const RESCUECHAR *replace) {
  Replace(replace);
  return *this;
}

RCHString &RCHString::operator=(RCHString &replace) {
  Replace(replace);
  return *this;
}

void RCHString::Replace(const RESCUECHAR *replace) {
  if (stringValue != 0) {
    *stringValue = 0;
  }
  AddTo(replace);
}

RCHString &RCHString::operator=(RESCUEINT64 replace) {
  Replace(replace);
  return *this;
}

RCHString &RCHString::operator=(RESCUEINT32 replace) {
  Replace(replace);
  return *this;
}

void RCHString::Replace(RESCUEINT64 replace) {
  if (stringValue != 0) {
    *stringValue = 0;
  }
  AddTo(replace);
}

void RCHString::Replace(RESCUEINT32 replace) {
  if (stringValue != 0) {
    *stringValue = 0;
  }
  AddTo(replace);
}

RCHString &RCHString::operator=(RESCUEDOUBLE replace) {
  Replace(replace);
  return *this;
}

void RCHString::Replace(RESCUEDOUBLE replace) {
  if (stringValue != 0) {
    *stringValue = 0;
  }
  AddTo(replace);
}

void RCHString::Replace(RCHString &replace) {
  if (stringValue != 0) {
    *stringValue = 0;
  }
  AddTo(replace);
}

void RCHString::tokenize(const RESCUECHAR *delimitersIn, RESCUEBOOL eachIn, RESCUEBOOL respectQuotesIn) {
  if (delimiters != 0) {
    free(delimiters);
  }
  if (delimitersIn == 0) {
    delimiters = 0;
  } else {
    delimiters = _strdup(delimitersIn);
  }
  each = eachIn;
  respectQuotes = respectQuotesIn;
  pos = stringValue;
}

RESCUEBOOL RCHString::operator>>(RESCUECHAR *buffer) {
  RESCUECHAR *out = buffer;
  *out = 0;
  if (*pos == 0) {
    return FALSE;
  } else {
    if (stringValue != 0) {
      if (each == FALSE) {
        if (delimiters != 0) {
          while (*pos != 0 && strchr(delimiters, *pos) != 0) {
            pos++;
          }
        }
      }
      /*
       Move along until we're at the end of the string or the
          delimiters.
      */
      while (*pos != 0 && (delimiters == 0) ? TRUE : strchr(delimiters, *pos) == 0) {
        if (*pos == '\'' && respectQuotes) {
          *out++ = *pos++;
          while (*pos != 0 && *pos != '\'') {
            *out++ = *pos++;
          }
          if (*pos == '\'') {
            *out++ = *pos++;
          }
        } else {
          *out++ = *pos++;
        }
      }
      if (*pos != 0) {
        pos++;
      }
      *out = 0;
    }
    return TRUE;
  }
}

RESCUEBOOL RCHString::operator>>(RCHString &buffer) {
  buffer = "";
  if (*pos == 0) {
    return FALSE;
  } else {
    if (stringValue != 0) {
      if (each == FALSE) {
        if (delimiters != 0) {
          while (*pos != 0 && strchr(delimiters, *pos) != 0) {
            pos++;
          }
        }
      }
      /*
          Move along until we're at the end of the string or the
          delimiters.
      */
      while (*pos != 0 && (delimiters == 0) ? TRUE : strchr(delimiters, *pos) == 0) {
        if (*pos == '\'' && respectQuotes) {
          buffer << *pos++;
          while (*pos != 0 && *pos != '\'') {
            buffer << *pos++;
          }
          if (*pos == '\'') {
            buffer << *pos++;
          }
        } else {
          buffer << *pos++;
        }
      }
      if (*pos != 0) {
        pos++;
      }
    }
    return TRUE;
  }
}

RESCUEBOOL RCHString::StartsWith(RCHString &other) { return StartsWith(other.String()); }

RESCUEBOOL RCHString::StartsWith(const RESCUECHAR *other) {
  if (stringValue == 0) {
    return FALSE;
  } else {
    size_t length = strlen(other);
    if (strncmp(stringValue, other, length) == 0) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
}

RESCUEBOOL RCHString::EndsWith(RCHString &other) { return EndsWith(other.String()); }

RESCUEBOOL RCHString::EndsWith(const RESCUECHAR *other) {
  if (stringValue == 0) {
    return FALSE;
  } else {
    RESCUEBOOL myReturn = TRUE;
    RESCUECHAR *pos2 = (RESCUECHAR *)(((size_t)other + strlen(other)) - 1);
    RESCUECHAR *pos1 = (RESCUECHAR *)(((size_t)stringValue + strlen(stringValue)) - 1);

    while ((size_t)pos2 >= (size_t)other && myReturn == TRUE) {
      if (tolower(*pos2--) != tolower(*pos1--)) {
        myReturn = FALSE;
      }
    }
    return myReturn;
  }
}

void RCHString::doubleApostrophe() {
  if (stringValue != 0) {
    RESCUEINT64 available = allocated - (1 + (RESCUEINT64)strlen(stringValue));
    RESCUEINT64 needed = 0;
    RESCUECHAR *cPtr = stringValue;
    while (*cPtr != 0) {
      if (*cPtr == '\'') {
        needed++;
      }
      cPtr++;
    }
    if (needed >= available) {
      allocated += needed;
      stringValue = (RESCUECHAR *)realloc(stringValue, (size_t)allocated);
    }

    cPtr = stringValue;
    while (*cPtr != 0) {
      if (*cPtr == '\'') {
        RESCUECHAR *pos2 = cPtr;
        RESCUECHAR *pos3;
        while (*pos2 != 0) {
          pos2++;
        }
        pos3 = pos2++;
        while (pos2 > cPtr) {
          *pos2-- = *pos3--;
        }
        cPtr++;
      }
      cPtr++;
    }
  }
}
