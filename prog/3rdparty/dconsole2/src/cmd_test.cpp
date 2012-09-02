#include "dconsole/common.h"
#include "dconsole/console.h"
#include "dconsole/command.h"

namespace dconsole {

    int testVar;

void setTestVar(int v)
{
    testVar = v;
}
int getTestVar(void)
{
    return testVar;
}

MyString f_hw()
{
    return "Hello, World!";
}

void f_voidretval()
{
	return ;
}

int f_intretval()
{
	return 42;
}

MyString f_string_p1Intp2Int(int , int )
{
	return "std::string: you've added two numbers :-)";
}

const char* f_constcharptr_p1Intp2Int(int , int )
{
	return "const char*: you've added two numbers :-)";
}

char* f_charptr_p1Intp2Int(int , int )
{
	return "char*: you've added two numbers :-)";
}

void f_void_intint(int, int)
{
	int a=10;
	(void)a;
	return;
}

MyString f_intfloatUD(int i1, float f2, int i3)
{
    MyString str1;
	MyString str2;
	MyString ud;

	std::stringstream inout;
	std::stringstream inout2;
	inout<<i1;
	inout>>str1;
	inout.clear();

	inout<<f2;
	inout>>str2;
	inout.clear();
	
	inout<<i3;
	inout>>ud;

	str1.append(" + ");
	str1.append(ud);
	str1.append("(user data)");

    if(i1 + i3 < f2)
        str1.append(" < ");
    else
        str1.append(" > ");
    str1.append(str2);

    return str1;
}

bool load_world(unsigned int key, bool async)
{
    return true;
}

int f_intintint(int i1, int i2, int i3)
{
    return (i1+i2)*i3;
}

float f_intfloat(int i, float f)
{
    return i+f;
}

int f_intptr(int* pi)
{
    return *pi;
}

int f(float f)
{
    return (int)(f+0.5f);
}

void f1(const char* const p)
{
    return;
}

int f1_ud(float f, int ud)
{
    return (int)(f + ud);
}

int functor1_ud(float f, void* pdata)
{
    return (int)(f + *((float*)pdata));
}

float f2(float f, int i)
{
    float asdfsa = f + i;
    return asdfsa;
}

bool f_and(bool b1, bool b2)
{
    return b1 && b2;
}

bool f_or(bool b1, bool b2)
{
    return b1 || b2;
}

class IO_CmdMy: public dconsole::BaseCmd {
public:
    IO_CmdMy(const MyString& name, const MyString desc):
        BaseCmd(name, desc) {  }

    bool runCommand(const std::vector<dconsole::BaseCmd::Variant>& params, dconsole::Console& console)
    {
        MyString str;
        for(size_t i=0; i<params.size();++i)
        {
            str = "CustomCommand: ";
            MyString pi = params[i];
            str.append(pi);
            console.addLogRecord(dconsole::Console::COL_MSG, str);
        }
        return true;
    }
protected:
    const MyString getUsage() const { return getName(); }
private:
    bool checkArguments(const std::vector<dconsole::BaseCmd::Variant>& params)
    {
        return true;
    }
};

MyString f2_str(const char* c1, const char* c2, int ud)
{
    char buf[128] = {0};
    if(strlen(c1) + strlen(c2) < 128-1 ) // one for #
    {
        if(ud)
        {
            strcpy(buf, c1);
            strcat(buf, "#");
            strcat(buf, c2);
        }
        else
        {
            strcpy(buf, c2);
            strcat(buf, "#");
            strcat(buf, c1);
        }
    }

    return MyString(&buf[0]);
}

void test() {

//    bool v1 = TypeTraits<int>::isFundamental;
//    bool v2 = TypeTraits<unsigned int>::isFundamental;
    //---------------------------------------------------------------------------
    std::vector<dconsole::BaseCmd::Variant> params;
    //params.push_back(dconsole::BaseCmd::Variant("1.86"));
    //params.push_back(dconsole::BaseCmd::Variant("5"));


    { // 
    dconsole::BaseCmd* pcmd = createCmd("helloWorld", "Hello, World!", f_hw);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);
    }

	{ // 
    dconsole::BaseCmd* pcmd = createCmd("do_somethig", "void ret val test", f_voidretval);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);
    }

	{ // >
    dconsole::BaseCmd* pcmd = createCmd("ret_int", "int ret val test", f_intretval);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);
    }

    { // 
    dconsole::BaseCmd* pcmd = createCmd("fasimple", "adder", f);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);

    pcmd = createCmd("fachar", "adder", f1);
    pcmd ->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd );
    }

	{ // 
	params.push_back(dconsole::BaseCmd::Variant("1.86"));
    params.push_back(dconsole::BaseCmd::Variant("5"));
	// rv: MyString; p1: int ; p2:int
	dconsole::BaseCmd* pcmd = createCmd("mystring_intint", "returns MyString", f_string_p1Intp2Int);
	pcmd ->runCommand(params, dconsole::Instance());
	dconsole::Instance().registerCommand(pcmd );

	pcmd = createCmd("constcharptr_intint", "returns const char*", f_constcharptr_p1Intp2Int);
	pcmd ->runCommand(params, dconsole::Instance());
	dconsole::Instance().registerCommand(pcmd );

	pcmd = createCmd("charptr_intint", "returns chasr*", f_charptr_p1Intp2Int);
	pcmd ->runCommand(params, dconsole::Instance());
	dconsole::Instance().registerCommand(pcmd );
	// void (*fp)(int,int)
	pcmd = createCmd("void_intint", "returns void", f_void_intint);
	pcmd ->runCommand(params, dconsole::Instance());
	dconsole::Instance().registerCommand(pcmd );
	}

    { // 
    dconsole::BaseCmd* pcmd = createCmd("AND", "logical AND", f_and);
    dconsole::Instance().registerCommand(pcmd);
        
    pcmd = createCmd("OR", "logical OR", f_or);
    dconsole::Instance().registerCommand(pcmd);
    
    pcmd = createCmd("int_float", "desc1", f_intfloat);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);
     
    pcmd = createCmd("func_add", "adder", f2);
    pcmd ->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);

    pcmd = createCmd("load_world", "Loads world: first parameter - key, second - asyns/sync load", load_world);
    dconsole::Instance().registerCommand(pcmd );
    }

    { // 
    typedef int (*f_intintint_t)(int i1, int i2, int i3);
    dconsole::BaseCmd* pcmd = createCmd("int3", "desc1", f_intintint);
    pcmd->runCommand(params, dconsole::Instance());
    dconsole::Instance().registerCommand(pcmd);
    }
}

}

/*

////////////////////////////////////////////////////////////////////
// Debug Console
////////////////////////////////////////////////////////////////////

// Users manual
// ------------
// Intro 
// -----
// Console consists of three main parts:
// 1) command line itself
// 2) log lines
// 3) command match list
// Command Line - where you type the commands
// Log Lines - where you can see result of the command beeing executed
// Command Match List - list of commands that match your current input
// You may start entering your command from '*' then all possible commands that have typed substing in it will be added into command match list
// e.g. >*dmin will match: admin, dminimum.
//
// Command entering
// ----------------
// Same as in ordinary command line: enter command and its parameters (if any) separated by SPACEs, followed by ENTER
// If command is wrong, corresponding log record will be added along with correct usage of the command
// to get the command description use "help cmdname" command, where "cmdname" - is the name of command which description you want to get.
// You may also do "help help" :-)
// If you start typing with astesisk '*' character then all commands that contain typed substring will be in command match list; 
//	
// Parameters passing 
// ------------------
// parameters are separated by SPACE sybmols. Numbers are entered like: 1, 2.35, 0.38. Bool values as: true, false, TRuE, False
//
// Return values
// -------------
// command return value printed in the log immediately after the command itself.
//
// Variables
// ---------
// To set variable type variable name and pass its value as parameter. To ge value of a variable - just print its name, following return key.
//
// Echo
// ----
// You may turn echo on/off by setting corressponding boolean variable, e.g. "echo true/false".

// Keys
// ----
// Open/Close console - TAB
// Ctrl + SPACE BAR  - to complete the word. If nothing to complete, then current line from log will be copied to command line.
// UP/DOWN - scroll command match list by line. If no commands in command match list, then scroll among entered commands wil be performed.  
// Ctrl + UP/DOWN - scroll the log by line
// LEFT/RIGHT/BACKSPACE - navigation/edition in the command line
// PAGE_UP/PAGE_DOWN - scroll in the command match list by page
// Ctrl + PAGE_UP/PAGE_DOWN - scroll in the log by page

// Colouring
// ---------
//
// To set your own colour add next lines to "default.cfg" file:
// Console_col_msg=0xffFFFFFF
// Console_col_grayed_msg=0xff555555
// Console_col_bad_cmd=0xffFF0000
// Console_col_bad_args=0xff800080
// Console_col_cmd_result=0xff0000FF
// Console_col_echo=0xff00FF00
// Console_col_bg=0xff000000
// Console_col_info=0xffAA5500
// Console_col_params=0xffffffff
// If line is not found default colour will be assigned. You can see default colours just above



// Programmers Manual
// ------------------

// 1) dconsole
// -------------
// It is actually a console class itself. Singleton.
// Access: 
	dconsole& console = donsole::Instance();
//
// 2) BaseCmd
// ------------------
// Base class for command creation. You may derive your own class from this one to create your speciall command.
// But.. this is not always needed. Why? For this see the next section.
// To be able to construct commands include "command.h" header, you may also need "dconsole.h" header 
// 
// 3) Command construction using predefined classes
// ------------------------------------------------
//
// Using predefined classes you benefit from automatic arguments checking and converting, errors handling, usage string construction, etc..
// All that you have to do is provide c++ function for your command.
//
// 3.1)Possible function parameters
// --------------------------------
//
// int, float, bool, const char* , MyString - actually you cannot enter anything else from command line, can you? :-)
// This also applies to return value, for return value you may use void also. It will be printed to console
// But user data can have any possible tye you wish (except the ones which are prohibited by  c++ like void)


// 4) Custom commands construction.
// --------------------------------
// 
// Sometimes there might be a situation when you need to make some custom processing. For this you may derive a class from IO_Base_Command.
// In this case of course you will need to check (and convert) your arguments by yourself.
// Here is an example of how you can do it:
//
class IO_CmdMy: public dconsole::BaseCmd {
public:
    IO_CmdMy(const MyString& name, const MyString desc):
        BaseCmd(name, desc) {  }

    bool runCommand(const std::vector<dconsole::BaseCmd::Variant>& params, dconsole::Console& console)
    {
        MyString str;
        for(size_t i=0; i<params.size();++i)
        {
            str = "CustomCommand: ";
            MyString pi = params[i];
            str.append(pi);
            console.addLogRecord(dconsole::Console::COL_MSG, str);
        }
        return true;
    }
protected:
    const MyString getUsage() const { return getName(); }
private:
    bool checkArguments(const std::vector<dconsole::BaseCmd::Variant>& params)
    {
        return true;
    }
};

*/