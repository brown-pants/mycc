#ifndef __TOKEN_H__
#define __TOKEN_H__

#define TOKEN_COUNT 34

#include <string>

class Token
{
public:
    enum Type
    {
        /* punctuation */
        Semicolon,  //  ;
        Comma,      //  ,

        /* math */
        Plus,       //  +
        Minus,      //  -
        Mult,       //  *
        Div,        //  /
        Mod,        //  %

        /* address */
        Ampersand,  // &

        /* bracket */
        OpenParen,  //  (
        CloseParen, //  )
        OpenSquare, //  [
        CloseSquare,//  ]
        OpenBrance, //  {
        CloseBrance,//  }

        /* assign */
        Eq,         //  =
        /* compare */
        Eq_Eq,      //  ==

        /* bit */
        Not,        //  !
        /* compare */
        Not_Eq,     //  !=

        /* compare */
        Greater,    //  >
        Greater_Eq, //  >=

        Less,       //  <
        Less_Eq,    //  <=

        /* id & constant */
        Identifier, //  ( _ | {letter} )( _ | {letter} | {digit} )*
        Integer,    //  0 | (1~9){digit}*
        Character,  //  '{char}'
        Float,      //  {digit}.{digit}*
        String,     //  "{letter}*"

        /* keyword */
        If,         //  if
        For,        //  for
        Else,       //  else
        While,      //  while
        Return,     //  return

        /* date type */
        DT_int,     //  int
        DT_char,    //  char
        DT_float,   //  float
        Void,       //  void

        /* special */
        Eof,        //  end token
        Nul         //  ε
    };

    Token(Type type = Nul, const std::string &lexeme = "Nul", int line = -1);

    void setType(Type type);
    void setLexeme(const std::string &lexeme);
    void setLine(int line);

    Type type() const;
    std::string lexeme() const;
    int line() const;

private:
    Type m_type;
    std::string m_lexeme;
    int m_line;
};

#endif