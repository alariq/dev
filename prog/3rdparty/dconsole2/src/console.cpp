#include "dconsole/common.h"

#include <algorithm>
#include <assert.h>
#include <math.h>

#include "dconsole/console.h"
#include "dconsole/command.h"



namespace dconsole {

// Initialization on static variables --------------------------------------------------------------------------------
//Console* Console::pRef =  0;
const char*  Console::ms_separatorString = ".................";
const char*  Console::ms_cursor = "_";
unsigned int Console::ms_log_colors[] = {   0xffFFFFFF, //COL_MSG
                                      0xffFF0000, //COL_BAD_CMD
                                      0xffFF00F0, //0xff800080, //COL_BAD_ARGS
                                      0xff00DDFF,//0xff0000FF, //COL_CMD_RESULT
                                      0xff00FF00, //COL_ECHO 
                                      0xffA7A7A7, //0xff555555,  //COL_GRAYED_MSG
                                      0x55000000,  //COL_BG
                                      0xffFFA800,//0xffAA5500,  //COL_INFO
                                      0xffffffff  //COL_PARAMS
};

const char* Console::ms_ColorNames[] = {    "Console_col_msg", 
                                                "Console_col_bad_cmd",
                                                "Console_col_bad_args", 
                                                "Console_col_cmd_result",
                                                "Console_col_echo",
                                                "Console_col_greyed_msg",
                                                "Console_col_bg",
                                                "Console_col_info",
                                                "Console_col_params",
                                                0
};

std::map<const char*, Console::eRecordType> Console::m_String2EnumMap;
// END_OF Initialization on static variables --------------------------------------------------------------------------

// prints command description 
MyString Console::describe(MyString cmdname)
{
    Console& console = dconsole::Instance();
    BaseCmd* pcmd = console.getCommandByName(cmdname);
    if(pcmd)
        return pcmd->getDescription();
    return MyString("No such command");
}

void Console::onKbdExecute()
{
    dconsole::Instance().process();
}
void Console::onKbdPrev(const bool mod)
{
    if( mod )
        Console::onKbdScrollLogLine(false);
    else
        Console::onKbdScrollMatch(false);
}
void Console::onKbdNext(const bool mod)
{
    if( mod )
        Console::onKbdScrollLogLine(true);
    else
        Console::onKbdScrollMatch(true);
}
void Console::onKbdPrevPage(const bool mod)
{
    if( mod )
        Console::onKbdScrollLogPage(false);
    else
        Console::onKbdScrollMatchPage(false);
}
void Console::onKbdNextPage(const bool mod)
{
    if( mod )
        Console::onKbdScrollLogPage(true);
    else
        Console::onKbdScrollMatchPage(true);
}

void Console::onKbdScrollLogLine(bool forward)
{
    Console& console = dconsole::Instance();
    size_t len = console.m_log.size();
    if(!len) return;
    console.m_log.move(1, !forward);// log is scrolled in opposite direction
}
void Console::onKbdScrollLogPage(bool forward)
{
    Console& console = dconsole::Instance();
    size_t len = console.m_log.size();
    if(!len /*|| len < LogLinesPageSize*/ ) return;
    console.m_log.move(LogLinesPageSize, !forward);// log is scrolled in opposite direction
}

void Console::onKbdScrollMatch(bool forward)
{
    Console& console = dconsole::Instance();
    if(console.m_match_commands.size() && console.mb_enable_match_commands_scroll)
    {
        size_t len = console.m_match_commands.size();
        if(!len) return;
        console.m_match_commands.move(1, forward);
        dconsole::Instance().onMatchListChanged();
    }
    // scroll in commands buffer
    else
    {
        size_t len = console.m_commands_buffer.size();
        if(!len) return;
        if(console.m_bCommandsBufferFlushed)
            console.m_bCommandsBufferFlushed = false;
        else
            console.m_commands_buffer.move(1, !forward);
        console.setCmdLine( console.m_commands_buffer.get().c_str() );
        console.mb_enable_match_commands_scroll = false;
    }
}

void Console::onKbdScrollMatchPage(bool forward)
{
    Console& console = dconsole::Instance();
    size_t len = console.m_match_commands.size();
    if(!len /*|| len <MatchCommandsPageSize*/ ) return;
    console.m_match_commands.move(MatchCommandsPageSize, forward);
    dconsole::Instance().onMatchListChanged();
}
void Console::onKbdAutocomplete()
{
    dconsole::Instance().autocomplete();
}
void Console::onKbdMoveHome()
{
    Console& console = dconsole::Instance();
    console.token = 0;
}
void Console::onKbdMoveEnd()
{
    Console& console = dconsole::Instance();
    size_t cmd_line_length = console.cmd_line.getLength();
    console.token = cmd_line_length;
}
void Console::onKbdMoveLeft()
{
	Console& console = dconsole::Instance();
    if(console.token)
        console.token--;
}
void Console::onKbdMoveRight()
{
    Console& console = dconsole::Instance();
    size_t cmd_line_length = console.cmd_line.getLength();
    if(console.token<cmd_line_length)
        console.token++;
}
void Console::onKbdDelete()
{
    Console& console = dconsole::Instance();
    console.cmd_line.removeAt(console.token);
}

void Console::onKbdBackspace()
{
    Console& console = dconsole::Instance();
    if( console.token > 0 )
    {
        if(console.cmd_line.removeAt(console.token-1) )
            console.token--;
    }
}


// string handler ---------------------------------------------------------------
// ------------------------------------------------------------------------------
void Console::StringHandler::setString(const char* pstr)
{
    assert(pstr);
    size_t len = strlen(pstr);
    assert(len < C_MaxLineLength );
    memset(&m_pstr[0], 0, C_MaxLineLength);
    if(len)
    {
        strncpy(&m_pstr[0], pstr, min(len, size_t(C_MaxLineLength-1)));
    }
    if(pConsole) pConsole->onCmdLineChanged();
}

Console::StringHandler::StringHandler()
:pConsole(0)
{
    memset(m_pstr, 0, C_MaxLineLength);
}

Console::StringHandler::~StringHandler()
{
    //MEM_M_Delete pCallback;
}

bool Console::StringHandler::insertAt(const char c, const unsigned int pos)
{
    size_t len = strlen(m_pstr);

    if(pos > len) return false;

    if(len >= C_MaxLineLength - 1) // -1 because we need 1char space to insert a MEM_M_New char
        return false;
    if(len == pos)
    {
        m_pstr[len] = c;
        if(pConsole) pConsole->onCmdLineChanged();
        return true;
    }

    char tmp[C_MaxLineLength] = {0};
    strncpy(tmp, m_pstr, pos);
    tmp[pos] = c;
    strcat(&tmp[pos+1], &m_pstr[pos]);
    strcpy(m_pstr, tmp);
    if(pConsole) pConsole->onCmdLineChanged();
    return true;
}
bool Console::StringHandler::removeAt(const size_t pos)
{
    const size_t len = strlen(m_pstr);

    assert(pos >= 0 && pos<=len);

    if(pos == C_MaxLineLength)
        return false;

    if(len == pos) // end of the line
        return false;

    if(len - pos == 1)
    {
        m_pstr[pos] = '\0'; // just kill last char
        if(pConsole) pConsole->onCmdLineChanged();
        return true;
    }
    
    char tmp[C_MaxLineLength] = {0};
    strncpy(tmp, m_pstr, pos);
    strcat(&tmp[pos], &m_pstr[pos+1]);
    strcpy(m_pstr, tmp);
    if(pConsole) pConsole->onCmdLineChanged();
    return true;
}
void Console::StringHandler::setStrChangedCallback(dconsole::Console* pC)
{
    //MEM_M_Delete pCallback;
    //pCallback = MEM_M_New StrChangedEventFunctor_t();
    //*pCallback = cb;
	pConsole = pC;
}
// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------


//void Console::onKbdEvent(const KbdEventContext::FullEventInfo* pei)
//{
//    Console& console = dconsole::Instance();
//
//    KbdButtonEvent* pkev = (KbdButtonEvent*)pei->m_Event;
//    if(pkev->GetButton() == Keyboard::Key_Tab)
//    {
//        console.is_opened = ! console.is_opened;
//        return;
//    }
//    if(!console.is_opened)
//        return;
//
//    const MyString str = pkev->GetStringValue();
//    if(!str.Length()) // shift, caps, ...
//        return;
//
//
//    char vkey[2] = {0};
//    str.ToAnsi(vkey,1);
//
////#ifndef _FINAL_
//    //TRACE_LogInfoF(LOG_CAT_Engine, "Button [ %c ] was pressed\n", vkey[0]);
////#endif
//    
//
//    // allow only allowed characters (which are present in font)
//    // TODO: add this in font class : font->isPrintable(vkey[0]);
//    if(vkey[0] >= 127 || vkey[0] <= 31)
//        return;
//
//    if(console.cmd_line.insertAt(vkey[0], console.token))
//        console.token++;
//}

static void set_echo(bool tf) { Instance().setEcho(tf); }
static bool get_echo() { return !!Instance().getEcho(); }

Console::Console():
token(0)  
,is_opened( false )
,shadow_command( 0 )
,m_bCommandsBufferFlushed( true )
,m_bEcho( false )
,mb_enable_match_commands_scroll( false ) 
{
    // init member variables


    m_String2EnumMap.insert(String2EnumPair("Console_col_msg", COL_MSG));
    m_String2EnumMap.insert(String2EnumPair("Console_col_bad_cmd", COL_BAD_CMD));
    m_String2EnumMap.insert(String2EnumPair("Console_col_bad_args", COL_BAD_ARGS)); 
    m_String2EnumMap.insert(String2EnumPair("Console_col_cmd_result", COL_CMD_RESULT));
    m_String2EnumMap.insert(String2EnumPair("Console_col_echo", COL_ECHO));
    m_String2EnumMap.insert(String2EnumPair("Console_col_grayed_msg", COL_GRAYED_MSG)); 
    m_String2EnumMap.insert(String2EnumPair("Console_col_bg", COL_BG));
    m_String2EnumMap.insert(String2EnumPair("Console_col_info", COL_INFO));
    m_String2EnumMap.insert(String2EnumPair("Console_col_params", COL_PARAMS));
     

    BaseCmd* pcmd = dconsole::createCmd("help", "Describes a command", Console::describe);
    registerCommand(pcmd);

	cmd_line.setStrChangedCallback(this);

}

Console::~Console()
{
    CmdList::iterator it = m_commands.begin(); 
    for(;it!=m_commands.end();++it)
    {
        MEM_M_Delete *it;
    }
}

bool Console::registerCommand(BaseCmd* pcmd)
{
    if(m_commands.size() == MaxRegisteredCommands)
    {
        MyString str = "Cannot register command: ";
        str.append(pcmd->getName());
        str.append(", max number of commands acheived");
        addLogRecord(COL_BAD_CMD, str);
    }
    CmdList::iterator it = m_commands.begin();
    // check if we have command with the same name
    bool ok = true;
    for(;it!=m_commands.end();++it)
    {
        if((*it)->getName() == pcmd->getName() )
        {
            ok = false;
            MyString str = "Cannot register command: ";
            str.append(pcmd->getName());
            str.append(" command with the same name was already registered");
            addLogRecord(COL_BAD_CMD, str);
        }
    }
    if(ok)
    {
        m_commands.push_back(pcmd);
        m_commands.sort();
        onCmdLineChanged();// most suitable command to current cmd_line may be registered
        return true;
    }

    return false;
}
bool Console::unregisterCommand(BaseCmd* pcmd)
{
    CmdList::iterator it = std::find_if(m_commands.begin(), m_commands.end(), CommandFinder(pcmd->getName().c_str()));
    if(it!=m_commands.end())
    {
        m_commands.erase(it);
        onCmdLineChanged();// current shadow may be deregistered
    }
    return true;
}

BaseCmd* Console::getCommandByName(const MyString& cmdname)
{
    CmdList::iterator it = std::find_if(m_commands.begin(), m_commands.end(), CommandFinder(cmdname.c_str()));
    if(it!=m_commands.end())
    {
        return *it;
    }
    return pnull;
}

void Console::process()
{
    MyString command;
    ParamsList params;

    std::vector<MyString> strings;
    split_string(cmd_line.getString(), strings, ' ');
    if(strings.size())
    {
        std::vector<MyString>::iterator sit = strings.begin();
        command = *sit;
        sit++;
        for(;sit!=strings.end();++sit)
            params.push_back( BaseCmd::Variant( *sit ) );
    }
    else
    {
        //reset cmd line
        setCmdLine("");
        return;
    }

    // kinda echo:
    LogRecord echo;
    echo.push_back( LogPair( logColor(COL_ECHO), command ) );
    ParamsList::iterator it = params.begin();
    for(;it!=params.end(); ++it)
    {
        MyString str = " ";
        MyString p = *it;
        str.append( p );

        echo.push_back( LogPair( logColor(COL_MSG), str ) );
    }
    //addLogRecord(Console::COL_ECHO, cmd_line.getString(), true );
    if(m_bEcho)
        addLogRecord(echo, true );

    m_commands_buffer.push_front( logRecordToString(echo) );
        
    CmdList::iterator com_it = std::find_if( m_commands.begin(),m_commands.end(), CommandFinder(command.c_str()));
    if(com_it!=m_commands.end())
    {
        if(! (*com_it)->runCommand(params, *this) )
        {
            MyString s = command;
            s.append(": bad arguments, usage: ");
            s.append((*com_it)->getUsage());
            addLogRecord(Console::COL_BAD_ARGS, s );
        }
    }
    else
    {
        MyString s = "bad command: ";
        echo.push_front( LogPair( logColor(COL_BAD_CMD), s ) );
        addLogRecord(echo);
    }
    
    // reset log to its beginning
    m_log.set_token(0);
    // to not skip first line in commands buffer
    m_bCommandsBufferFlushed = true;
    m_commands_buffer.set_token(0);
    //reset cmd line
    setCmdLine("");
}

void Console::clearLog()
{
    m_log.clear();
}
void Console::addLogRecord(const Console::eRecordType rt, const MyString& str, bool addToCommandsBuffer/* = false*/)
{
    assert((unsigned int)rt < sizeof(ms_log_colors)/sizeof(ms_log_colors[0]) );
    LogRecord r;
    r.push_back( LogPair( logColor(rt), str) );
    addLogRecord(r, addToCommandsBuffer);
}

void Console::addLogRecord(const Console::LogRecord r, bool addToCommandsBuffer/* = false*/)
{
    m_log.push_front( r );
    /*if(addToCommandsBuffer)
    {
        m_commands_buffer.push_front( logRecordToString(r) );
    }*/
}


void Console::autocomplete()
{
    if(shadow_command)
    {
        setCmdLine(shadow_command->getName().c_str());
    }
    else //if(0 == cmd_line.getLength())
        if(m_log.size()) // if we have something in log add it to the entered command line
    {
        // add log string
        MyString str = cmd_line.getString();
        str.append(" ");
        str.append( logRecordToString(m_log.get()) );
        setCmdLine(str.c_str());
    }
}

void Console::onCmdLineChanged()
{
    // find suitable shadow 
    shadow_command = pnull;
    m_match_commands.clear();

    std::vector<MyString> strings;
    MyString cmd_line_command;
    split_string(cmd_line.getString(), strings, ' ');
    if(0 == strings.size())
        return;

    cmd_line_command = *strings.begin();
    MyString cmd_line_command_to_check = cmd_line_command;
    // because users want match while typing and also match by patterns
    // and without this strings like: 'cmd' will not match anything but 'cmd'
    // but users want it to be like 'cmd*'
    cmd_line_command_to_check.append("*"); 

    // for case insensitive check
    //cmd_line_command_to_check.ToLower();

    const char* pcmd_line_lowered = cmd_line_command_to_check.c_str();
    MyString tmp;
    MyString tmp2;

    CmdList::iterator it = m_commands.begin();
    for(;it!=m_commands.end(); ++it)
    {
        tmp = (*it)->getName();
        //tmp.ToLower();
        const char* cmd_name_lowered = tmp.c_str();
        if(strmatch(cmd_name_lowered, pcmd_line_lowered))
        {
            m_match_commands.push_back(*it);
        }
    } 
    //throw away match if it is one and same as entered string 
    if(1 == m_match_commands.size())
    {
        tmp = cmd_line_command;
        //tmp.ToLower();
        tmp2 = m_match_commands.get(0)->getName();
        //tmp2.ToLower();
        if(tmp == tmp2)
            m_match_commands.clear();
        else
            select_match();// select current match
    }
    else
        select_match();// select current match

    mb_enable_match_commands_scroll = true;
}


void Console::onMatchListChanged()
{
    select_match(); 
}
void Console::select_match()
{
    size_t len = m_match_commands.size();
    size_t mid = min( len/2, MatchCommandsPageSize/2);
    if(len)
    {
        shadow_command = m_match_commands.get( (int)( m_match_commands.get_token() + mid) );
    }
}

void Console::setColor(const char* const name, unsigned int color)
{
    assert(m_String2EnumMap.count(name));
    ms_log_colors[ m_String2EnumMap[name] ] = color;
}

void Console::setCmdLine(const char* const pstr)
{
    if(!pstr) return;
    cmd_line.setString(pstr);
    token = strlen(pstr);
}

bool Console::split_string(MyString str, std::vector<MyString>& params, char splitter)
{

    if(str.empty()) return false;
    params.clear();

    size_t start = 0;
    MyString tmp;
    size_t pos = 0;
    bool started_from_splitter = false;

    // start always from non splitter
    if(str[pos] == splitter)
    {
        started_from_splitter = true;
        pos++;
        while(pos<str.size() && str[pos]==splitter) pos++;
    }
    start = pos;    

    while(pos<str.size())
    {
        pos = str.find(splitter, start);
        if(pos!=(unsigned int)-1)
            tmp = str.substr(start, pos - start);
        else
            tmp = str.substr(start);

        params.push_back(BaseCmd::Variant(tmp));

        //skip all spaces
        while(pos<str.size() && str[pos]==splitter) pos++;
        start = pos;
    }

    return started_from_splitter;
}

MyString Console::logRecordToString(const Console::LogRecord& r)
{
    MyString str;
    Console::LogRecord::const_iterator it = r.begin();
    for(;it!=r.end();++it)
    {
        str.append(it->second);
    }
    return str;
}

Color Console::logColor(const Console::eRecordType rt) const
{
    unsigned int c = ms_log_colors[rt];
    return Color(c);
}

} // namespace dconsole