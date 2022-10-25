#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>

using namespace std;

enum TokenType {
  NUM = 0, SYM = 1
};

struct InputChar {
  char ch;
  int coordinate;
  InputChar * next;
};

typedef InputChar * InputCharPtr;

struct Token {
  char symbol;
  double number;
  int coordinate;
  TokenType type;
  Token * next;
};

typedef Token * TokenPtr;

char greturnStr[5];

int gcounter = 0;

int gcoordinate = 1;

bool ifchinese = false;

void GetFile(InputCharPtr & head) {

  char ch = '\0';

  ch = getchar();
  while (ch == ' ' || ch == '\t')
    ch = getchar();

  if ((ch < 0 || ch > 255) && !ifchinese) {
    ifchinese = true ;
  } // if chinese
  else if ( (ifchinese)) {
    ifchinese = false ;
    gcoordinate-- ;
  } // else if

  if (gcounter < 4) {
    greturnStr[gcounter++] = ch;
    greturnStr[gcounter] = '\0';
  } // if

  if (ch != '\n' && ch != '=') {
    head = new InputChar;
    head->next = NULL;
    head->ch = ch;
    head->coordinate = gcoordinate;
    gcoordinate++;
    GetFile(head->next);
  } // if
} // GetFile

void PrintFormula(InputCharPtr input_head) {

  while (input_head != NULL) {
    cout << input_head->ch;
    input_head = input_head->next;
  } // while
} // PrintFormula

bool Error_ExceededDigits(InputCharPtr input_head) {

  InputCharPtr temp = input_head;
  bool ifnum = false;
  int digits = 0;
  while (input_head != NULL) {
    if (input_head->ch >= '0' && input_head->ch <= '9')
      ifnum = true;
    else
      ifnum = false;

    if (ifnum)
      digits++;
    else
      digits = 0;

    if (digits > 16) {
      cout << "輸入超過十六位數! (  ";
      PrintFormula(temp);
      cout << "  位於第 " << input_head->coordinate << " 欄: '" << input_head->ch << "')" << endl;
      return true;
    } // if

    input_head = input_head->next;
  } // while

  return false;
} // Error_ExceededDigits

bool Error_WrongDecimalPoint(InputCharPtr input_head) {

  bool still_same_number = false;
  bool returnError = false;
  InputCharPtr temp = input_head;

  while (input_head != NULL) {
    if (input_head->ch == '.' && still_same_number == true) {
      cout << "輸入錯誤的小數點格式! (  ";
      PrintFormula(temp);
      cout << "  位於第 " << input_head->coordinate << " 欄: '" << input_head->ch << "')" << endl;
      returnError = true;
    } // if

    if (input_head->ch == '+' || input_head->ch == '-' || input_head->ch == '*' || input_head->ch == '/')
      still_same_number = false;
    else if (input_head->ch == '.')
      still_same_number = true;

    input_head = input_head->next;
  } // whlie

  return returnError;
} // Error_WrongDecimalPoint

bool Error_WrongSymbol(InputCharPtr input_head) {

  bool error = false;
  InputCharPtr copyhead = input_head;

  while (input_head != NULL) {
    if (input_head->ch < 0) {
      cout << "輸入錯誤的字元(中文)! (  ";
      PrintFormula(copyhead);
      cout << "  位於第 " << input_head->coordinate << " 欄: '" << input_head->ch << input_head->next->ch << "')" << endl;
      input_head = input_head->next ;
      error = true;
      goto chinese ;
    } // if

    if ((input_head->ch < '0' || input_head->ch > '9') && input_head->ch != '+' &&
            input_head->ch != '-' && input_head->ch != '*' && input_head->ch != '/' && input_head->ch != '.') {
      cout << "輸入錯誤的符號! (  ";
      PrintFormula(copyhead);
      cout << "  位於第 " << input_head->coordinate << " 欄: '" << input_head->ch << "')" << endl;
      error = true;
    } // if
    
    chinese:
    
    input_head = input_head->next;
  } // while

  return error;
} // Error_WrongSymbol

double StrToNum(char temp[]) {

  int i = 0;
  double num1 = 0;
  double num2 = 0;
  double A = 0.1;

  while (temp[i] != '.' && i < strlen(temp)) {
    num1 = num1 * 10 + (temp[i] - '0');
    i++;
  }
  if (i < (strlen(temp) - 1)) {
    i++;
    while (i < strlen(temp)) {
      num2 = num2 + (temp[i] - '0') * A;
      A *= 0.1;
      i++;
    } // while
  } // if

  return (num1 + num2);
} // StrToNum

void Trans(InputCharPtr input_head, TokenPtr & token_head) {

  if (input_head != NULL) {
    if (input_head->ch == '+' || input_head->ch == '-' || input_head->ch == '*' || input_head->ch == '/') {
      token_head = new Token;
      token_head->next = NULL;
      token_head->symbol = input_head->ch;
      token_head->coordinate = input_head->coordinate;
      token_head->type = SYM;
    }// if
    else {
      int i = 0;
      char temp[100] = {0};
      char temp2[100] = {0};
      InputCharPtr a = NULL;

      token_head = new Token;
      token_head->next = NULL;
      token_head->coordinate = input_head->coordinate;
      while ((input_head->ch >= '0' && input_head->ch <= '9') || input_head->ch == '.') {
        temp[i++] = input_head->ch;
        if (input_head->next == NULL)
          goto here;
        a = input_head;
        input_head = input_head->next;
      } // while

      input_head = a;

here:
      if (temp[0] == '.') {
        temp2[0] = '0';
        strcat(temp2, temp); // if
      } else
        strcpy(temp2, temp); // else

      token_head->number = StrToNum(temp2);
      token_head->type = NUM;
    } // else

    Trans(input_head->next, token_head->next);
  } // if
} // Trans

bool Error_WrongFormat(InputCharPtr input_head, TokenPtr token_head) {

  if (token_head->symbol == '*' || token_head->symbol == '/') {
    cout << "輸入錯誤的格式! (  ";
    PrintFormula(input_head);
    cout << "  位於第 " << token_head->coordinate << " 欄: '" << token_head->symbol << "')" << endl;
    return true;
  }// if
  else {
    TokenType type;
    if (token_head->type == NUM) {
      int i = 2;
      TokenPtr temp = token_head;

      while (token_head != NULL) {
        if (i % 2 == 0)
          type = NUM;
        else
          type = SYM;
        if (type != token_head->type) {
          cout << "輸入錯誤的格式! (  ";
          PrintFormula(input_head);
          cout << "  位於第 " << token_head->coordinate << " 欄: '" << token_head->symbol << "')" << endl;
          return true;
        } // if
        i++;
        temp = token_head;
        token_head = token_head->next;
      } // while

      if (i % 2 == 0) {
        cout << "結尾須為數字! (  ";
        PrintFormula(input_head);
        cout << "  位於第 " << temp->coordinate << " 欄: '" << temp->symbol << "')" << endl;
        return true;
      } // if
    }// if
    else {
      int i = 1;
      TokenPtr temp = token_head;

      while (token_head != NULL) {
        if (i % 2 == 0)
          type = NUM;
        else
          type = SYM;
        if (type != token_head->type) {
          cout << "輸入錯誤的格式! (  ";
          PrintFormula(input_head);
          cout << "  位於第 " << token_head->coordinate << " 欄: '" << token_head->symbol << "')" << endl;
          return true;
        }
        i++;

        temp = token_head;
        token_head = token_head->next;
      } // while

      if (i % 2 == 0) {
        cout << "結尾須為數字! (  ";
        PrintFormula(input_head);
        cout << "  位於第 " << temp->coordinate << " 欄: '" << temp->symbol << "')" << endl;
        return true;
      } // if
    } // else
  } // else

  return false;
} // bool Error_WrongFormat(InputCharPtr input_head, TokenPtr token_head) {

void AddNode(TokenPtr & token_head) {

  if (token_head->type == SYM) {
    TokenPtr temp = new Token;
    temp->next = NULL;
    temp->number = 0;
    temp->type = NUM;
    temp->next = token_head;
    token_head = temp;
  } // if
} // AddNode

bool CheckIfFinished_1(TokenPtr token_head) {

  while (token_head != NULL) {
    if (token_head->symbol == '*' || token_head->symbol == '/')
      return true;

    token_head = token_head->next;
  } // while
  return false;
} // CheckIfFinished_1

bool CheckIfFinished_2(TokenPtr token_head) {

  while (token_head != NULL) {
    if (token_head->symbol == '+' || token_head->symbol == '-')
      return true;

    token_head = token_head->next;
  } // while

  return false;
} // CheckIfFinished_2

void Calcu1ate_TimesAndDividedBy(TokenPtr token_head) {

  TokenPtr temp = token_head;
  if (token_head->next != NULL) {
    while (token_head->next->next != NULL) {
      double result = 0;
      if (token_head->next->symbol == '*' || token_head->next->symbol == '/') {
        if (token_head->next->symbol == '*')
          result = token_head->number * token_head->next->next->number;
        else
          result = token_head->number / token_head->next->next->number;

        token_head->number = result;

        if (token_head->next->next->next != NULL)
          token_head->next = token_head->next->next->next;
        else {
          token_head->next = NULL;
          break;
        } // else

      } // if
      token_head = token_head->next;
    } // while

    if (CheckIfFinished_1(temp))
      Calcu1ate_TimesAndDividedBy(temp);

  } // if
} // Calcu1ate_TimesAndDividedBy

void Calculate_PlusAndMinus(TokenPtr token_head) {

  TokenPtr temp = token_head;
  if (token_head->next != NULL) {


    while (token_head->next->next != NULL) {
      double result = 0;
      if (token_head->next->symbol == '+' || token_head->next->symbol == '-') {
        if (token_head->next->symbol == '+')
          result = token_head->number + token_head->next->next->number;
        else
          result = token_head->number - token_head->next->next->number;

        token_head->number = result;

        if (token_head->next->next->next != NULL)
          token_head->next = token_head->next->next->next;
        else {
          token_head->next = NULL;
          break;
        } // else

      } // if
      token_head = token_head->next;
    } // while

    if (CheckIfFinished_2(temp))
      Calculate_PlusAndMinus(temp);
  } // if

} // Calculate_PlusAndMinus

void End() {

  for (int i = 0; i < strlen(greturnStr); i++)
    greturnStr[i] = tolower(greturnStr[i]);

  if (strcmp(greturnStr, "exit") == 0)
    exit(0);
} // END

void PrintAnswer(TokenPtr token_head) {

  char temp[500];
  bool ifFloat = false;
  sprintf(temp, "%.10f", token_head->number);
  for (int i = 0; i < strlen(temp); i++) {
    if (temp[i] == '.')
      ifFloat = true;
  } // for

  int i = strlen(temp) - 1;
  if (ifFloat) {
    for (; temp[i] != '.' && temp[i] == '0'; i--)
      temp[i] = '\0';
  } // if 

  if (temp[i] == '.')
    temp[i] = '\0';

  ifFloat = false;
  for (int i = 0; i < strlen(temp); i++) {
    if (temp[i] == '.')
      ifFloat = true;
  } // for

  bool iftoolarge = false;
  if (ifFloat) {
    int times = 0;
    i = strlen(temp) - 1;
    for (int j = 0; temp[i] != '.' && temp[j] != '.'; j++, i--) {
      times++;
      if (times > 20)
        iftoolarge = true;
    } // for
  }// if
  else {
    if (strlen(temp) > 20)
      iftoolarge = true;
  } // else

  if (iftoolarge)
    cout << "答案數值超過二十位數，無法計算!" << endl;
  else
    cout << temp << endl;

} // PrintAnswer

int main() {

  while (true) {
    InputCharPtr input_head = NULL;
    TokenPtr token_head = NULL;

    printf("本計算機提供加減乘除 輸入 exit 以退出 : \n");

redo:
    input_head = NULL;
    token_head = NULL;
    gcounter = 0;
    gcoordinate = 1;
    GetFile(input_head);
    End();

    if (input_head == NULL) {
      goto redo;
    } // if

    if (Error_ExceededDigits(input_head)) {
      cout << "請重新輸入 : " << endl;
      goto redo;
    } // if

    if (Error_WrongSymbol(input_head)) {
      cout << "請重新輸入 : " << endl;
      goto redo;
    } // if

    if (Error_WrongDecimalPoint(input_head)) {
      cout << "請重新輸入 : " << endl;
      goto redo;
    } // if

    Trans(input_head, token_head);

    if (Error_WrongFormat(input_head, token_head)) {
      cout << "請重新輸入 : " << endl;
      goto redo;
    } // if

    AddNode(token_head);

    Calcu1ate_TimesAndDividedBy(token_head);
    Calculate_PlusAndMinus(token_head);
    PrintAnswer(token_head);
  } // while
} // main


