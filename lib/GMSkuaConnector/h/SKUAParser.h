#pragma once

// Fast scanner + parser for SKUA file formats
//
// 700Mb+ file is scanned and parsed in 5.5 seconds
// if you change anything, make sure it is faster, not slower (benchmark first for reference)
// post processing takes another 1.5 seconds -- turn it off to measure

#include "SKUADefs.h"

#include <unordered_map>
#include <cstring>
#include <vector>

namespace gm_skua
{

class IProgressHandler;
struct SKUAParseData;
class SKUAIPostProcessor;

namespace internal
{

#define DEFAULT_SYMBOLTABLE_SIZE  2048


// we use one symbol type for keywords and identifiers
struct Symbol
{
  int token;
  int attribute;
  char data[MAX_IDENTIFIER_SIZE];

  Symbol();
  Symbol(int token, int attribute);
  Symbol(int token, int attribute, char *token_data);
  Symbol(const Symbol& rhs);
};


class SymbolTable
{
  // Standard hashing function; not particular fast or optimized for the SKUA files (where a lot of identifiers differ in the last characters)
  // but... it suffices; more complex ones need length, which means either walking over the string again, or keeping track of the string size (a la Pascal)
  struct HashKey
  {
  int operator()(const char *str) const
  {
      int hash = 5381;
      for (int i = 0; *str; ++str, ++i)
    hash = ((hash << 5) + hash) + *str;
      return hash;
  }
  };

  // Assume case sensitive checking
  struct HashEqual
  {
  bool operator()(const char *lhs, const char *rhs) const
  {
      return strncmp(lhs, rhs, MAX_IDENTIFIER_SIZE) == 0;
  }
  };

  // we use an array of identifiers, and a hash to look them up (the int is an index into SymbolStorage, as is the key)
  // for a new attempt, we reserve a new entry in the storage,
  // if it already exists we keep the reserved area, if it is added, we simply mark the reserved spot as part of the table
  //
  // careful: as the hash table points into the vector for its keys, we need to re-hash when we resize the vector!
  //
  // std::unordered_map is not fast; can be replaced by something faster in the future
  typedef std::unordered_map<char *, int, HashKey, HashEqual> SymbolLookup;
  typedef std::vector<Symbol> SymbolStorage;

  SymbolLookup m_symbolLookup;
  SymbolStorage m_symbolStorage;

  // first keywords, then we set m_identifiers to the current index, and from then we add identifiers (names, etc)
  int m_identifiers;

  // actual size (including the reserved spot)
  int m_size;

  // whether we have a reserved spot
  bool m_have_reserve;

  // resize the vector and re-hash
  void resize();

public:
  SymbolTable();
  ~SymbolTable();

  // fix the identifier index
  void start_identifiers_at(int size);

  // insert keywords (follow with start_identifiers_at)
  int insert(const Symbol& symbol);
  // insert reserved spot
  int insert();

  // reserve a new spot (this is the default one used by the scanner)
  int reserve(int token);
  // turn on or off reserved spot (used by resize)
  void reserve(bool toggle);
  // do we have a reserved spit
  bool have_reserve() const;
  // invalidate the reserve spot's parsed string (we might parse it double at eof otherwise)
  void invalidate_reserve();

  // size of actual identifiers (never the reserved spot)
  int size() const;

  // index into the vector
  Symbol& operator[](int index);
  const Symbol& operator[](int index) const;
};




// we put both keywords and identifiers in the same symbol table
// m_identifier_start is set after the keywords

class SKUAParser
{
  FILE *m_fp;
  IProgressHandler& m_progress;
  SymbolTable m_symbols;
  int m_identifier_start;

  SKUAIPostProcessor *m_postProcessor;


  int m_lineNr;

  // data for maintaining a double buffer
  // 16Mb (per buffer) gives experimentally a good performance, while keeping the buffer small (32Mb gives same results, 64Mb close)
  // we define the two buffers with one malloc, and add a sentinel value after them, with some padding for alignment
  // sentinel 0 gives good performance in tests
  // m_lexeme lags behind m_forward (which tries to find the extend of the lexeme), and new buffer will be loaded when *m_forward = sentinel
  // this is standard stuff from the Dragon book, except that we never decrease m_forward
  //
  // extracting this to a separate class severely slows down the scanner
  const size_t m_buffer_size = 16 * 1024 * 1024;
  const size_t m_sentinel_size = 16;
  const char m_sentinel = 0;

  char *m_buffer;

  char *m_buffer_0_end;
  char *m_buffer_1_end;

  size_t m_count;

  char *m_forward;
  char *m_lexeme;


  // everything in SKUA is 7-bits ASCII; we define our character classes to avoid locale handling in the std routines for isdigit, isalpha, etc
  static int m_char_class[256];


  // the current character and its character class
  char m_char_read;
  int  m_char_class_read;


  // we keep track of state transitions ourselves; they're fairly simple
  int m_lexstate;


  // define a temporary buffer for strtod here
  // if the double crosses buffers, we can't feed it to strtod directly
  // (there is no speed difference for checking for buffer crossing and only using the buffer then)
  // (there is also no speed difference for trying our own double parsing, and since it's fairly involved to do it right, we use the std)
  char m_tmp_buffer_for_double[256];

  // there is an enormous speedup for parsing integers ourselves; main reason is that we don't check for different bases 0x, 0b, etc


  // our pointer to where we can write our current lexeme (in the reserved spot in the symbol table)
  char *m_identifier_data;


  // variables that keep track of the info we need to pass on from the scanner to the parser itself
  // for identifiers, ATTRIB (+ m_identifier_start) points into the symbol table
  int TOKEN;
  int ATTRIB;

  char   CVAL;
  int    IVAL;
  double DVAL;

  int SIGN;

public:
  SKUAParser(FILE *fp, IProgressHandler& progress, SKUAIPostProcessor *postProcessor = nullptr);
  ~SKUAParser();

  int LineNr() const;

  int ErrorNr() const;
  const char *Error(int index) const;

  int ObjectNr() const;
  const SKUAParseData *Object(int index) const;

  bool Parse();

private:

  // PARSER INTERNAL routines

  SKUAParseData *m_data;

  std::vector<const char *> m_errors;
  std::vector<const SKUAParseData *> m_objects;

  bool ParseError(const char *error);

  bool ParseGOCAD();
  bool ParseLightTSolid();
  bool ParseTSolid();
  bool ParseTSurf();
  bool ParseVSet();
  bool ParseHeader();
  bool ParseOriginalCoordinateSystem();
  bool ParseAxisUnit();
  bool ParseProps();
  bool ParseTetraRegions();
  bool ParseTVolume();
  bool ParseVertices();
  bool ParseSharedVertices();
  bool ParseTetras();
  bool ParseTetraRegionIndicators();
  bool ParseTriangles();
  bool ParseSurfaces();
  bool ParseTFace();
  bool ParseModelRegions();


  // BUFFER INTERFACE

  // load the next buffer
  bool load();
  // decide which buffer to load and call load
  bool handle_forward_is_null();


  // SCANNER INTERFACE

  // keep track of we are reading " or ' quoted identifier
  char m_scan_quote_char;

  // scan initialization
  bool scan_init();

  // scan until token, error, or eof is found -- THIS IS THE MAIN ROUTINE FOR THE PARSER TO CALL
  void scan(int next_lexstate = 0);

  // skips whitespace (no newline) without altering state
  void scan_skip_whitespace();

  // skips to end of line
  void scan_skip_to_eol();


  // SCANNER INTERNAL routines

  // set up reading an identifier
  void start_identifier();
  // process the read identifier and finds it in or adds it to the symbol table
  void end_identifier();
  void end_identifier_forced();

  // default state
  void handle_lexstate_new();
  // handle + and - (can be part of a number or surface)
  void handle_lexstate_pm();
  // make sure we don't interpret the next one as an int (+13x is a valid surface, or 13region a valid model region)
  // Not happy with this; should be replaced by backing up if non-whitespace encountered after int
  void handle_lexstate_noint();
  // handle integer
  void handle_lexstate_integer();
  // handle double (if we find a . after handling integer)
  void handle_lexstate_double();
  // handle a normal identifier
  void handle_lexstate_identifier();
  // handle surface identifier
  void handle_lexstate_identifier_plus_number();
  // set up reading a "quoted" identifier
  void handle_lexstate_quoted_identifier_start();
  // handle rest of "quoted" identifier
  void handle_lexstate_quoted_identifier();
  // set up reading (surface_geological_type)
  void handle_lexstate_parenthesis_identifier_start();
  // handle rest of (surface_geolical_type)
  void handle_lexstate_parenthesis_identifier();
  // set up reading the rest of the line as an identifier
  void handle_lexstate_identifier_to_eol_start();
  // handle rest of reading the rest of the line
  void handle_lexstate_identifier_to_eol();

  void debug_print_token(const char *filename) const;
};


}

}
