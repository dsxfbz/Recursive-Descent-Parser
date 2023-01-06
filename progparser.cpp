/*
NAME: Daniel Xiang
DATE: 2020-12-17
ASSIGNMENT: CS3500 HW3 Recursive Descent Parser

TOKENS
    +Integers
        -Non-empty sequences of digits optionally preceded with either a ▒
<80><98>+▒<80><99> or ▒<80><98>-▒<80><99> sign.
    +Decimal numbers
        -Integers followed by a ▒<80><98>.▒<80><99>, followed by a non-empty sequence of digits.
    +Strings
        -Any non-space sequences of characters enclosed in ▒<80><9C>▒<80><9D>.
        -e.g. ▒<80><9C>hello▒<80><9D> ▒<80><9C>abc123▒<80><9D>.
    +Keywords
        -Following strings:
            !
            :
            :=
            +
            -
            *
            /
            OR
            AND
            ~
            (
            )
            <
            >
            =
            #
            ;
            FUNC
            IF
            ELSE
            WHILE
            PRINT
            RETURN
            END
        -(Notice: keywords are uppercase)
    +Identifiers
        -Sequences of digits or letters.
        -The first character must be a letter
        -Cannot be a Keyword.

*/
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cctype>

#include <stdlib.h>

using namespace std;

bool isDecDigit(const char & symbol)
{
    //cout<<"    isDecDigit("<<symbol<<") ";
    if('0'<=symbol && symbol<='9')
    {
        //cout<<"true"<<endl;
        return true;
    }
    //cout<<"false"<<endl;
    return false;
}

bool isHexDigit(const char & symbol)
{
    //cout<<"    isHexDigit("<<symbol<<") ";
    if(('0'<=symbol && symbol<='9')||('A'<=symbol && symbol<='F'))
    {
        //cout<<"true"<<endl;
        return true;
    }
    //cout<<"false"<<endl;
    return false;
}

bool isChar(const char & symbol)
{
    //cout<<"    isChar("<<symbol<<") ";
    if(('a'<=symbol && symbol<='z')||('A'<=symbol && symbol<='Z'))
    {
        //cout<<"true"<<endl;
        return true;
    }
    //cout<<"false"<<endl;
    return false;
}

bool isAlphaNum(const char & symbol)
{
    //cout<<"    isAlphaNum("<<symbol<<") ";
    if(('0'<=symbol && symbol<='9')||('a'<=symbol && symbol<='z')||('A'<=symbol && symbol<='Z'))
    {
        //cout<<"true"<<endl;
        return true;
    }
    //cout<<"false"<<endl;
    return false;
}

bool check(string state, vector<string> currStates)
{
    for(int i=0; i<currStates.size(); i++)
    {
        if(currStates[i] == state)
        {
            return true;
        }
    }
    return false;
}

string slice(const string & source, const int & startInd, const int & endInd)
{
    string result = "";
    for(int i=startInd; i<=endInd; i++)
    {
        result += source[i];
    }
    return result;
}


deque<string> original;
deque<string> data;
string token;

/*
Fills data with input simplified to tokens
    +Integers
        -Non-empty sequences of digits optionally preceded with either a ▒
<80><98>+▒<80><99> or ▒<80><98>-▒<80><99> sign.
    +Decimal numbers
        -Integers followed by a ▒<80><98>.▒<80><99>, followed by a non-empty sequence of digits.
    +Strings
        -Any non-space sequences of characters enclosed in ▒<80><9C>▒<80><9D>.
        -e.g. ▒<80><9C>hello▒<80><9D> ▒<80><9C>abc123▒<80><9D>.
    +Keywords
*/
void buildData();

int tokenCount = 0;
void getToken();
void error_statement(string expected, string actual);

void parse_Relation();
void parse_AddOperator();
void parse_MulOperator();
void parse_Expression();
void parse_SimpleExpression();
void parse_Term();
void parse_Factor();
void parse_Assignment();
void parse_PrintStatement();
void parse_IfStatement();
void parse_WhileStatement();
void parse_Statement();
void parse_StatementSequence();
void parse_ParamSequence();
void parse_RetStatement();
void parse_FunctionDeclaration();
void parse_FunctionSequence(); //Start

int main()
{
    /* - Convert input strings to simpler forms (tokens) - */
    buildData();
    /*
    for(int i=0; i<data.size(); i++)
    {
        cout<<i<<": \""<<data[i]<<"\""<<endl;
    }
    */

    //Parse
    getToken();
    parse_FunctionSequence();

    //Output
    if(token == "End of Input")
    {
        cout<<"CORRECT"<<endl;
    }
    else
    {
        error_statement("Function Sequence", token);
    }

    return 0;
}

/*
Fills data with input simplified to tokens
    +Integers
        -Non-empty sequences of digits optionally preceded with either a ▒
<80><98>+▒<80><99> or ▒<80><98>-▒<80><99> sign.
    +Decimal numbers
        -Integers followed by a ▒<80><98>.▒<80><99>, followed by a non-empty sequence of digits.
    +Strings
        -Any non-space sequences of characters enclosed in ▒<80><9C>▒<80><9D>.
        -e.g. ▒<80><9C>hello▒<80><9D> ▒<80><9C>abc123▒<80><9D>.
    +Keywords
*/
void buildData()
{
    //Using parser made in hw1
    /*
TOKENS
    +Integers
        -Non-empty sequences of digits optionally preceded with either a ▒
<80><98>+▒<80><99> or ▒<80><98>-▒<80><99> sign.
    +Decimal numbers
        -Integers followed by a ▒<80><98>.▒<80><99>, followed by a non-empty sequence of digits.
    +Strings
        -Any non-space sequences of characters enclosed in ▒<80><9C>▒<80><9D>.
        -e.g. ▒<80><9C>hello▒<80><9D> ▒<80><9C>abc123▒<80><9D>.
    +Keywords
        -Following strings:
            !
            :
            :=
            +
            -
            *
            /
            OR
            AND
            ~
            (
            )
            <
            >
            =
            #
            ;
            FUNC
            IF
            ELSE
            WHILE
            PRINT
            RETURN
            END.
        -(Notice: keywords are uppercase)
    +Identifiers
        -Sequences of digits or letters.
        -The first character must be a letter
        -Cannot be a Keyword.
    */


    /*---Variables{---*/
    //int numLexemes;
    string currLexeme;
    //string stringType = "ERROR{ Type not set";

    bool valid = false;

    //string currState = "S";
    vector<string> currStates = {"S"};
    vector<string> nextStates = {};

    /*---Input---*/
    /*
    cin>>numLexemes;
    cout<<numLexemes<<endl;
    for(int x=0; x<numLexemes; x++)
    */
    int x=0;
    while(cin>>currLexeme)
    {
        original.push_back(currLexeme);
        //Get string
        currStates = {"S"};//Set start state

        //cout<<currLexeme<<endl;

        //Analyze string
        //  +ndFSA
        if((currLexeme == "!")
            || (currLexeme == "&")
            || (currLexeme == ",")
            || (currLexeme == ":")
            || (currLexeme == ":=")
            || (currLexeme == "+")
            || (currLexeme == "-")
            || (currLexeme == "*")
            || (currLexeme == "/")
            || (currLexeme == "OR")
            || (currLexeme == "AND")
            || (currLexeme == "~")
            || (currLexeme == "(")
            || (currLexeme == ")")
            || (currLexeme == "<")
            || (currLexeme == ">")
            || (currLexeme == "=")
            || (currLexeme == "#")
            || (currLexeme == ";")
            || (currLexeme == "FUNC")
            || (currLexeme == "RETURN")
            || (currLexeme == "END.")
            || (currLexeme == "IF")
            || (currLexeme == "ELSE")
            || (currLexeme == "WHILE")
            || (currLexeme == "PRINT"))
        {
            valid = true;
            data.push_back(currLexeme);
        }
        else
        {
            for(int i=0; i<currLexeme.length(); i++)
            {
                for(int j=0; j<currStates.size(); j++)
                {
                    /*
                    for(int y=0; y<currStates.size(); y++)
                    {
                        cout<<"  "<<currStates[y]<<" ";
                    }
                    cout<<endl;
                    */
                    //switch(currStates[j])
                    //{
                    if (currStates[j] == "S")
                    {
                        /*
                        if(isHexDigit(currLexeme[i]))
                        {
                            nextStates.push_back("hexDig");
                            nextStates.push_back("charLit1");
                        }
                        */
                        if((currLexeme[i] == '+')||(currLexeme[i] == '-'))
                        {
                            nextStates.push_back("int+-");
                        }
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("int");
                        }
                        if(isChar(currLexeme[i]))
                        {
                            nextStates.push_back("ident");

                            /*
                            //Keywords
                            if(currLexeme[i] == 'W')
                            {
                                nextStates.push_back("while1");
                            }
                            if(currLexeme[i] == 'E')
                            {
                                nextStates.push_back("else1");
                            }
                            if(currLexeme[i] == 'I')
                            {
                                nextStates.push_back("if1");
                            }
                            if(currLexeme[i] == 'P')
                            {
                                nextStates.push_back("print1");
                            }
                            */

                        }

                        //String start
                        if(currLexeme[i] == '\"')
                        {
                            nextStates.push_back("quot1");
                        }
                    }
                    else if (currStates[j] == "int+-")
                    {
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("int");
                        }
                    }
                    else if (currStates[j] == "int"){
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("int");
                        }
                        else if(currLexeme[i] == '.')
                        {
                            nextStates.push_back("period");
                        }
                    }
                    else if (currStates[j] == "period"){
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("dec");
                        }
                    }
                    else if (currStates[j] == "dec"){
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("dec");
                        }
                        /*
                        if(currLexeme[i] == 'E')
                        {
                            nextStates.push_back("E");
                        }
                        */
                    }
                    /*
                    else if (currStates[j] == "E"){
                        if((currLexeme[i] == '+')||(currLexeme[i] == '-'))
                        {
                            nextStates.push_back("sci+-");
                        }

                        if(currLexeme[i] == '0')
                        {
                            nextStates.push_back("sci0");
                        }
                        else if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("sci");
                        }
                    }
                    else if (currStates[j] == "sci+-")
                    {
                        if(currLexeme[i] == '0')
                        {
                            nextStates.push_back("sci0");
                        }
                        else if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("sci");
                        }
                    }
                    else if (currStates[j] == "sci+-")
                    {
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("sci");
                        }
                    }
                    else if (currStates[j] == "sci")
                    {
                        if(isDecDigit(currLexeme[i]))
                        {
                            nextStates.push_back("sci");
                        }
                    }
                    */
                    /*
                    else if (currStates[j] == "hexDig"){
                        if(isHexDigit(currLexeme[i]))
                        {
                            nextStates.push_back("hexDig");
                        }
                        if(currLexeme[i] == 'H')
                        {
                            nextStates.push_back("hex");
                        }
                    }
                    */
                    /*
                    if (currStates[j] == "hex"){
                    }
                    */
                    /*
                    else if (currStates[j] == "charLit1"){
                        if(isHexDigit(currLexeme[i]))
                        {
                            nextStates.push_back("charLit2");
                        }
                    }
                    else if (currStates[j] == "charLit2"){
                        if(currLexeme[i] == 'X')
                        {
                            nextStates.push_back("charLit");
                        }
                    }
                    */
                    else if (currStates[j] == "ident"){
                        if(isAlphaNum(currLexeme[i]))
                        {
                            nextStates.push_back("ident");
                        }
                    }
                    else if (currStates[j] == "quot1"){
                        if (currLexeme[i] == '\"')
                        {
                            nextStates.push_back("string");
                        }
                        else if(!isspace(currLexeme[i]))
                        {
                            nextStates.push_back("strMid");
                        }
                    }
                    else if (currStates[j] == "strMid")
                    {
                        if((currLexeme[i]!=' ') &&
                        (currLexeme[i]!='\f') &&
                        (currLexeme[i]!='\n') &&
                        (currLexeme[i]!='\r') &&
                        (currLexeme[i]!='\t') &&
                        (currLexeme[i]!='\v'))
                        {
                            nextStates.push_back("strMid");
                        }
                        if(currLexeme[i] == '\"')
                        {
                            nextStates.push_back("string");
                        }
                    }

                    //Keyword States
                    /*
                    else if (currStates[j] == "while1"){
                        if(currLexeme[i] == 'H')
                        {
                            nextStates.push_back("while2");
                        }
                    }
                    else if (currStates[j] == "while2"){
                        if(currLexeme[i] == 'I')
                        {
                            nextStates.push_back("while3");
                        }
                    }
                    else if (currStates[j] == "while3"){
                        if(currLexeme[i] == 'L')
                        {
                            nextStates.push_back("while4");
                        }
                    }
                    else if (currStates[j] == "while4"){
                        if(currLexeme[i] == 'E')
                        {
                            nextStates.push_back("while5");
                        }
                    }
                    else if (currStates[j] == "else1"){
                        if(currLexeme[i] == 'L')
                        {
                            nextStates.push_back("else2");
                        }
                    }
                    else if (currStates[j] == "else2"){
                        if(currLexeme[i] == 'S')
                        {
                            nextStates.push_back("else3");
                        }
                    }
                    else if (currStates[j] == "else3"){
                        if(currLexeme[i] == 'E')
                        {
                            nextStates.push_back("else4");
                        }
                    }
                    else if (currStates[j] == "if1"){
                        if(currLexeme[i] == 'F')
                        {
                            nextStates.push_back("if2");
                        }
                    }
                    else if (currStates[j] == "print1"){
                        if(currLexeme[i] == 'R')
                        {
                            nextStates.push_back("print2");
                        }
                    }
                    else if (currStates[j] == "print2"){
                        if(currLexeme[i] == 'I')
                        {
                            nextStates.push_back("print3");
                        }
                    }
                    else if (currStates[j] == "print3"){
                        if(currLexeme[i] == 'N')
                        {
                            nextStates.push_back("print4");
                        }
                    }
                    else if (currStates[j] == "print4"){
                        if(currLexeme[i] == 'T')
                        {
                            nextStates.push_back("print5");
                        }
                    }
                    */
                }
                currStates = nextStates;
                nextStates.clear();
            }
        }
        /*
        for(int y=0; y<currStates.size(); y++)
        {
            cout<<currStates[y]<<" ";
        }
        cout<<endl;
        */

        //Output string type
            for(int j=0; j<currStates.size(); j++)
            {
            //switch(currStates[j])
            //{
                if (currStates[j] == "int"){
                    cout<<x+1<<": "<<"Integer"<<endl;
                    valid = true;
                    data.push_back("int");
                }
                else if (currStates[j] == "dec"){
                    cout<<x+1<<": "<<"Decimal"<<endl;
                    valid = true;
                    data.push_back("dec");
                }
                /*
                else if (currStates[j] == "sci" || currStates[j] == "sci2"){
                    cout<<x+1<<": "<<"Scientific"<<endl;
                    valid = true;
                }
                else if (currStates[j] == "while5"){
                    cout<<x+1<<": "<<"Keyword"<<endl;
                    valid = true;
                }
                else if (currStates[j] == "if2"){
                    cout<<x+1<<": "<<"Keyword"<<endl;
                    valid = true;
                }
                else if (currStates[j] == "else4"){
                    cout<<x+1<<": "<<"Keyword"<<endl;
                    valid = true;
                }
                else if (currStates[j] == "print5"){
                    cout<<x+1<<": "<<"Keyword"<<endl;
                    valid = true;
                }
                else if (currStates[j] == "hex"){
                    cout<<x+1<<": "<<"Hexadecimal"<<endl;
                    valid = true;
                }
                */
                else if (currStates[j] == "ident"){
                    //Assuming that Keywords aren't identifiers
                    /*
                    if(check("hex", currStates)==false
                    && check("charLit", currStates)==false
                    && check("while5", currStates)==false
                    && check("if2", currStates)==false
                    && check("else4", currStates)==false
                    && check("print5", currStates)==false)
                    {
                        cout<<x+1<<": "<<"Identifier"<<endl;
                        valid = true;
                    }
                    */
                    valid = true;
                    data.push_back("ident");
                }
                else if (currStates[j] == "string"){
                    //cout<<x+1<<": "<<"String"<<endl;
                    valid = true;
                    data.push_back("string");
                }
                /*
                else if (currStates[j] == "charLit"){
                    cout<<x+1<<": "<<"Character"<<endl;
                    valid = true;
                }
                */
            //}
            }

            if(valid == false)
            {
                cout<<currLexeme<<"("<<x+1<<"): INVALID!"<<endl;
            }
            valid = false;
            x++;
    }

    data.push_back("End of Input");

    return;
}


void getToken()
{
    token = data.front();
    data.pop_front();
    cout<<">"<<tokenCount<<": "<<token<<endl;
    tokenCount++;
    return;
}

void error_statement(string expected, string actual)
{
    cout<<"INVALID!"<<endl;
    //cout<<"Error: "<<expected<<" expected, got \""<<actual<<"\""<<endl;
    cout<<"Error: "<<expected<<" expected, got \""<<original[tokenCount-1]<<"\""<<endl;
    //cout<<"Token count: "<<tokenCount-1<<endl;
    exit(0);
    return;
}

void parse_Relation()
{
    //Relation  ▒<86><92>  < | > |  = | #
    //cout<<"void parse_Relation()"<<endl;
    if(token == "<")
    {
        getToken();
    }
    else if(token == ">")
    {
        getToken();
    }
    else if(token == "=")
    {
        getToken();
    }
    else if(token == "#")
    {
        getToken();
    }
    else
    {
        error_statement("Relation", token);
    }
    return;
}
void parse_AddOperator()
{
    //AddOperator  ▒<86><92>   + | - | OR | &
    //cout<<"void parse_AddOperator()"<<endl;
    if(token == "+")
    {
        getToken();
    }
    else if(token == "-")
    {
        getToken();
    }
    else if(token == "OR")
    {
        getToken();
    }
    else if(token == "&")
    {
        getToken();
    }
    else
    {
        error_statement("AddOperator", token);
    }
    return;
}
void parse_MulOperator()
{
    //MulOperator  ▒<86><92>   * | / | AND
    //cout<<"void parse_MulOperator()"<<endl;
    if(token == "*")
    {
        getToken();
    }
    else if(token == "/")
    {
        getToken();
    }
    else if(token == "AND")
    {
        getToken();
    }
    else
    {
        error_statement("MulOperator", token);
    }
    return;
}
void parse_Expression()
{
    //Expression ▒<86><92> SimpleExpression [ Relation SimpleExpression ]
    //cout<<"void parse_Expression()"<<endl;
    parse_SimpleExpression();
    if((token == "<")||(token == ">")||(token == "=")||(token == "#"))
    {
        parse_Relation();
        parse_SimpleExpression();
    }
}
void parse_SimpleExpression()
{
    //SimpleExpression ▒<86><92> Term { AddOperator Term }
    //cout<<"void parse_SimpleExpression()"<<endl;
    parse_Term();
    while((token == "+")||(token == "-")||(token == "OR")||(token == "&"))
    {
        parse_AddOperator();
        parse_Term();
    }
}
void parse_Term()
{
    //Term ▒<86><92> Factor { MulOperator Factor }
    //cout<<"void parse_Term()"<<endl;
    parse_Factor();
    while((token == "*")||(token == "/")||(token == "AND"))
    {
        parse_MulOperator();
        parse_Factor();
    }
    return;
}
void parse_Factor()
{
    //Factor ▒<86><92>  integer | decimal | string | identifier | ( Expression ) | ~ Factor
    //cout<<"void parse_Factor()"<<endl;
    if(token == "int")
    {
        getToken();
    }
    else if(token == "dec")
    {
        getToken();
    }
    else if(token == "string")
    {
        getToken();
    }
    else if(token == "ident")
    {
        getToken();
    }
    else if(token == "(")
    {
        getToken();
        parse_Expression();
        if(token == ")")
        {
            getToken();
        }
        else
        {
            error_statement(")", token);
        }
    }
    else if(token == "~")
    {
        getToken();
        parse_Factor();
    }
    else
    {
        error_statement("Factor", token);
    }
}
void parse_Assignment()
{
    //Assignment ▒<86><92> identifier := Expression
    //cout<<"void parse_Assignment()"<<endl;
    if(token == "ident")
    {
        getToken();
        if(token == ":=")
        {
            getToken();
            parse_Expression();
        }
        else
        {
            error_statement(":=", token);
        }
    }
    else
    {
        error_statement("Assignment", token);
    }
    return;
}
void parse_PrintStatement()
{
    //PrintStatement ▒<86><92> PRINT ( Expression )
    //cout<<"void parse_PrintStatement()"<<endl;
    if(token == "PRINT")
    {
        getToken();
        if(token == "(")
        {
            getToken();
            parse_Expression();
            if(token == ")")
            {
                getToken();
            }
            else
            {
                error_statement(")", token);
            }
        }
        else
        {
            error_statement("(", token);
        }
    }
    else
    {
        error_statement("Print statement", token);
    }
    return;
}
void parse_IfStatement()
{
    //IfStatement ▒<86><92> IF  Expression : StatementSequence [ ELSE StatementSequence ] ;
    //cout<<"void parse_IfStatement()"<<endl;
    if(token == "IF")
    {
        getToken();
        parse_Expression();
        if(token == ":")
        {
            getToken();
        }
        else
        {
            error_statement(":", token);
        }
        parse_StatementSequence();
        while(token == "ELSE")
        {
            getToken();
            parse_StatementSequence();
        }
        if(token == ";")
        {
            getToken();
        }
        else
        {
            error_statement(";", token);
        }
    }
    else
    {
        error_statement("If statement", token);
    }
    return;
}
void parse_WhileStatement()
{
    //WhileStatement ▒<86><92> WHILE Expression : StatementSequence ;
    //cout<<"void parse_WhileStatement()"<<endl;
    if(token == "WHILE")
    {
        getToken();
        parse_Expression();
        if(token == ":")
        {
            getToken();
        }
        else
        {
            error_statement(":", token);
        }
        parse_StatementSequence();
        if(token == ";")
        {
            getToken();
        }
        else
        {
            error_statement(";", token);
        }
    }
    else
    {
        error_statement("While statement", token);
    }
}
void parse_Statement()
{
    //Statement ▒<86><92> Assignment | PrintStatement | IfStatement | WhileStatement
    //cout<<"void parse_Statement()"<<endl;
    if(token == "ident")
    {
        parse_Assignment();
    }
    else if(token == "PRINT")
    {
        parse_PrintStatement();
    }
    else if(token == "IF")
    {
        parse_IfStatement();
    }
    else if(token == "WHILE")
    {
        parse_WhileStatement();
    }
    else
    {
        error_statement("Statement", token);
    }
    return;
}
void parse_StatementSequence()
{
    //StatementSequence ▒<86><92> Statement { ! Statement }
    //cout<<"void parse_StatementSequence()"<<endl;
    parse_Statement();
    while(token == "!")
    {
        getToken();
        parse_Statement();
    }
}
void parse_ParamSequence()
{
    //ParamSequence ▒<86><92>  identifier  { , identifier }
    //  {...} -> 0+ repititions
    //cout<<"void parse_ParamSequence()"<<endl;
    if(token == "ident")
    {
        getToken();

        //0+ repititions of R"(, ident)"
        while(token == ",")
        {
            getToken();
            if(token == "ident")
            {
                getToken();
            }
            else
            {
                error_statement("Identifier", token);
            }
        }
    }
    else
    {
        error_statement("Identifier", token);
    }
    return;
}
void parse_RetStatement()
{
    //RetStatement ▒<86><92> RETURN identifier
    //cout<<"void parse_RetStatement()"<<endl;
    if(token == "RETURN")
    {
        getToken();
        if(token == "ident")
        {
            getToken();
        }
        else
        {
            error_statement("Identifier", token);
        }
    }
    else
    {
        error_statement("Return statement", token);
    }
    return;
}
void parse_FunctionDeclaration()
{
    //FunctionDeclaration ▒<86><92> FUNC identifier ( [ ParamSequence ] ) StatementSequence [ RetStatement ] END.
    cout<<"void parse_FunctionDeclaration()"<<endl;
    cout<<"  token: "<<token<<"\n";
    if(token == "FUNC")
    {
        getToken();
        if(token == "ident")
        {
            getToken();
            if(token == "(")
            {
                getToken();

                if(token == ")")
                {
                    getToken();
                }
                else//Optional paramsequence
                {
                    parse_ParamSequence();

                    if(token == ")")
                    {
                        getToken();
                    }
                    else
                    {
                        error_statement(")", token);
                    }
                }

                parse_StatementSequence();
                if(token == "END.")
                {
                    getToken();
                }
                else
                {
                    parse_RetStatement();
                    if(token == "END.")
                    {
                        getToken();
                    }
                    else
                    {
                        error_statement("END.", token);
                    }
                }
            }
            else
            {
                error_statement("(", token);
            }
        }
        else
        {
            error_statement("Identifier", token);
        }
    }
    else
    {
        error_statement("Function declaration", token);
    }

    return;
}
void parse_FunctionSequence() //Start
{
    //FunctionDeclaration ▒<86><92> FUNC identifier ( [ ParamSequence ] )
    //FunctionSequence ▒<86><92> FunctionDeclaration { FunctionDeclaration }
    //cout<<"void parse_FunctionSequence()"<<endl;

  do {
    parse_FunctionDeclaration();
  } while(token == "FUNC");
    return;
}









