#include "dconsole/console.h"
#include "dconsole/command.h"


class TextConsole: public dconsole::Console {
public:
    virtual void Draw();
    static TextConsole& Instance(void);

	virtual void addLogRecord(const dconsole::Console::LogRecord r, bool addToCommandsBuffer = false);
};
