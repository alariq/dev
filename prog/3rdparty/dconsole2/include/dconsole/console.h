#ifndef __IO_CONSOLE_H__
#define __IO_CONSOLE_H__

#include "dconsole/common.h"

#include <list>
#include <deque>
#include <map>
#include <math.h>
#include <assert.h>

#include "dconsole/base_command.h"

namespace dconsole {


template <typename T, int numEl>
class CycleBuffer {
    std::deque<T> m_data;
    int token;
public:
    enum { eMaxNumEl = numEl };
    enum eDir { eFront = 1, eBack = -1 };
    CycleBuffer():token(0) {
    }

    bool push_front(T el) {
        m_data.push_front(el);
        if(m_data.size() == eMaxNumEl)
        {
            m_data.pop_back();
            return true;
        }
        return false;
    }

    bool push_back(T el) {
        m_data.push_back(el);
        if(m_data.size() == eMaxNumEl)
        {
            m_data.pop_front();
            return true;
        }
        return false;
    }
    
    T& get() {
        assert( !m_data.empty() );
        return m_data[token];
    }
    T& get_raw(int index) {
        assert(index >=0 && index<m_data.size());
        return m_data[index];
    }

    unsigned int wrap(const int i) {
        const int dir = i >= 0 ? 1 : -1;
        const int size = (int)m_data.size();
        int d = abs(i) % size;
        return (size + dir*d) % size;
    }

    T& get(const int i) {
        return m_data[wrap(i)];
    }

    void clear() {
        m_data.clear();
        token = 0;
    }
    
    int move(const unsigned int delta, const bool front) {
        const int dir = front ? 1 : -1;
        const int size = (int)m_data.size();
        int d = delta % size;
        token = (token + size + dir*d) % size;
        return token;
    }

    int     set_token(int i)    { return token = clamp(i, 0, (int)m_data.size()); }
    int     get_token()         { return token; }
    size_t  size()              { return m_data.size(); }
};


class InteractiveDevice;

class Console {
    Console(const Console& c);
    void operator=(const Console& c);
public:
    enum eRecordType    { COL_MSG, COL_BAD_CMD, COL_BAD_ARGS, COL_CMD_RESULT, COL_ECHO, COL_GRAYED_MSG, COL_BG, COL_INFO, COL_PARAMS, LAST_REC };
    static const char* ms_ColorNames[];
protected:
    //typedef void (*EventFP_t)(const KbdEventContext::FullEventInfo*);
    typedef void (*SimpleFP_t)(void);
    typedef std::list<BaseCmd*>                 CmdList;
    typedef std::vector<BaseCmd::Variant>       ParamsList;
    typedef std::pair<const char*, eRecordType> String2EnumPair; // used in correct
    
public:

    BaseCmd* getCommandByName(const MyString& cmdname);

    // processes keyboard input
    static void onKbdExecute();

    static void onKbdPrev(const bool mod);
    static void onKbdNext(const bool mod);
    static void onKbdPrevPage(const bool mod);
    static void onKbdNextPage(const bool mod);

    static void onKbdScrollLogLine(bool forward);
    static void onKbdScrollLogPage(bool forward);
    static void onKbdScrollMatch(bool forward);
    static void onKbdScrollMatchPage(bool forward);

    static void onKbdAutocomplete();
    static void onKbdMoveHome();
    static void onKbdMoveEnd();
    static void onKbdMoveLeft();
    static void onKbdMoveRight();
    static void onKbdDelete();
    static void onKbdBackspace();
    static void onKbdEvent(/*const KbdEventContext::FullEventInfo**/void* pdata);

	void process(); // parses current command line end executes comand
private:

    
    void autocomplete(); 
    void onCmdLineChanged();
    void onMatchListChanged();


    // string handler
    class StringHandler {
    public:
		Console* pConsole;
        static const unsigned int   C_MaxLineLength = 256;

                            StringHandler();
                            ~StringHandler();
        void                setString(const char* pstr);
        const char* const   getString() { return m_pstr; }
        size_t		        getLength() { return m_pstr ? strlen(m_pstr) : 0; }
        bool	            isEmpty() { return !m_pstr || strlen(m_pstr)==0 ; }
        bool                insertAt(const char c, unsigned int pos);
        bool                removeAt(size_t pos);
		void                setStrChangedCallback(Console* pC);
    private:
        
        char                m_pstr[C_MaxLineLength];
    };

    // command camparator
    class CommandFinder : std::unary_function<BaseCmd*, bool> {
        public:
            CommandFinder(const char* name) : m_name(name) { }
            bool operator()(BaseCmd* p) const 
            {
                return 0 == /*stricmp*/strcmp(m_name, p->getName().c_str());
            }
        private:
            const char* m_name;
    };

public:
    typedef std::pair<Color, MyString>   LogPair;
    typedef std::list< LogPair >              LogRecord;

    void addLogRecord(const eRecordType rt, const MyString& str, bool addToCommandsBuffer = false);
    void addLogRecord(const LogRecord r, bool addToCommandsBuffer = false);
    void clearLog();

    // registers command in console
    bool registerCommand(BaseCmd* pcmd);
    // unregisters command in console
    bool unregisterCommand(BaseCmd* pcmd);

    // prints command description 
    static MyString describe(MyString cmdname);

    virtual void Draw() = 0;

    bool isOpened() { return is_opened; }
    void setOpen(bool tf) { is_opened = tf; }

    void setColor(const char* const name, unsigned int color);
    void setCmdLine(const char* const str);

    void setEcho(bool tf) { m_bEcho = tf; }
    bool getEcho() { return m_bEcho; }
    static const size_t CmdLineLength = 256;
    static const size_t MatchCommandsPageSize = 5;
    static const size_t LogLinesPageSize = 8;
    static const size_t MaxLogLines = 128;
    static const size_t MaxCommandsBufferLines = 50;
    static const size_t MaxRegisteredCommands = USHRT_MAX;
    static const char*  ms_separatorString;
    static const char*  ms_cursor;
    Color          logColor(const Console::eRecordType rt) const;
protected:
    Console();
    virtual ~Console();

    void select_match(); // selects best match for entered command line and assigns shadow command to it
    bool split_string(MyString str, std::vector<MyString>& params, char splitter);

    MyString   logRecordToString(const LogRecord& r);

   
    static unsigned int          ms_log_colors[]; // to make console coloured :-)
    // string to enum map
    static std::map<const char*, eRecordType> m_String2EnumMap;
    
    CmdList             m_commands;     // list of all registered commands
    StringHandler       cmd_line;       // current command line
    BaseCmd*            shadow_command; // current shadowed command (which current command line will be completed to by CTRL+ENTER )
    size_t					token;          // current cursor position in command line

    // list of all commands that match currently entered string, changes dynamically as user is editing cmd_line
    // USHRT_MAX and hope we will not have more commands than this
    CycleBuffer<BaseCmd*, MaxRegisteredCommands>   m_match_commands;

    // keeps all entered command lines
    CycleBuffer<MyString, MaxCommandsBufferLines>  m_commands_buffer;
    bool                m_bCommandsBufferFlushed;

    CycleBuffer<LogRecord, MaxLogLines>   m_log;

    // is console opened
    bool               is_opened;
    // duplicate entered command in log
    bool               m_bEcho;
    //
    bool               mb_enable_match_commands_scroll;
    // kbd events stuff
    //KbdEventContext*    m_pkbdevcontext;
    // kbd evens (added into m_pkbdevcontext)
    //std::list<KbdEvent*> m_events;
};


/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif
// get instance of console singleton
extern DECLSPEC Console& DCONSOLECALL Instance();

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif 

// add some test commands
void test();

}

#endif // __IO_CONSOLE_H__