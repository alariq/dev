#include "dconsole/graphic_console.h"

#ifndef __WIN32__
    #include <stdio.h>
#endif

void GraphicsConsole::Draw()
{
	if(!is_opened) return;

	if(!pTextDriver_) return;

	int w,h;
	if(!pTextDriver_->beginDraw())
		return;

	// get screen params
	pTextDriver_->getScreenParams(w, h);
	const float screenWidth  = (float)w;
    const float screenHeight = (float)h;

	// get font params
	pTextDriver_->getFontParams(w, h);
	const float fontWidth = (float)w;
    const float fontHeight = (float)h;

	// calculate positions
    //const float relHeight = (float)fontHeight;
    const float log_height = (LogLinesPageSize + 1)*fontHeight; // one for info line
    const float matches_height = (MatchCommandsPageSize +1)*fontHeight; // one for info line

	const float top_offset = 2*fontHeight;
    const float log_start = screenHeight - fontHeight - top_offset;//cmd_line_start - log_height;
    const float matches_start = matches_height + fontHeight;
    const float cmd_line_start = fontHeight; /*matches_start - relHeight;*/ // extract one row for cmd line inself
    
    Color backColor = Color(ms_log_colors[COL_BG]);
	const float X_START = 20.0f;

    // draw prompt
	pTextDriver_->printString(X_START, cmd_line_start, ">", logColor(COL_MSG));
	size_t textW = pTextDriver_->getWidth(">");
    if(shadow_command)
    {
        // draw potentian command in grey
		pTextDriver_->printString(X_START+textW, cmd_line_start, shadow_command->getName().c_str(), logColor(COL_GRAYED_MSG));
    }

    // draw current command line
	pTextDriver_->printString(X_START + textW, cmd_line_start, cmd_line.getString(), logColor(COL_MSG)); 
    // draw cursor
	textW += pTextDriver_->getWidth(cmd_line.getString(), token);
    pTextDriver_->printString(X_START + textW, cmd_line_start, ms_cursor, logColor(COL_MSG));

	// draw match commands
    size_t len = m_match_commands.size();
    if(len)
    {
        int idx = 0;
        int num_visible = (int)min( len, MatchCommandsPageSize);
		dconsole::BaseCmd* pcc = 0;
        float currentH = matches_start;

        int token = m_match_commands.get_token();
        for(int i = 0; i< num_visible;)
        {
            pcc = m_match_commands.get(token + idx);
            Color color = ( pcc == shadow_command) ? logColor(COL_MSG) : logColor(COL_GRAYED_MSG);
			MyString t = pcc->getName();
            pTextDriver_->printString(X_START + 0, currentH, t.c_str(), color);
            currentH -= fontHeight;
            i++;

            // draw separator
            if(i < num_visible)
            if(m_match_commands.wrap(token + idx) == m_match_commands.size() - 1 && len > MatchCommandsPageSize)
            {
                pTextDriver_->printString(X_START + 0, currentH, ms_separatorString, logColor(COL_GRAYED_MSG));
                currentH -= fontHeight;
                i++;
            }
			idx++;
        }
        if(len > MatchCommandsPageSize)
        {
			char buf[CmdLineLength+1];
            sprintf(buf, "[ %d more are available ]", len - MatchCommandsPageSize);
            pTextDriver_->printString(X_START + 0, currentH, buf, logColor(COL_INFO));
            currentH += fontHeight;
        }
    }

	// draw log
    len = m_log.size();
    if(len)
    {
        float currentH = log_start - log_height + fontHeight; // extract one line for info
        int num_visible = (int)min( len, LogLinesPageSize);
        int token = m_log.get_token();
        int idx = 0;
        for(int i = 0; i < num_visible ;)
        {
            const Console::LogRecord& r = m_log.get(token + idx);
            Console::LogRecord::const_iterator it = r.begin();
            float dw = .0f;
            for(;it!=r.end();++it)
            {
                const LogPair& log_pair = *it;
                Color color = log_pair.first;
                MyString str = log_pair.second;
                pTextDriver_->printString(X_START + dw, currentH, str.c_str(), color);
				dw += pTextDriver_->getWidth(str.c_str());
            }
            currentH += fontHeight;
            
            i++;

            // draw separator
            if(i < num_visible)
            if(m_log.wrap(token + idx) == len-1 && len > LogLinesPageSize)
            {
                pTextDriver_->printString(X_START + 0, currentH, ms_separatorString, logColor(COL_GRAYED_MSG));
                currentH += fontHeight;
                i++;
            }
			idx++;
        }
        if(len > LogLinesPageSize)
        {
			char buf[CmdLineLength+1];
            sprintf(buf, "[ %d more are available ]", len - LogLinesPageSize);
            pTextDriver_->printString(X_START + 0, log_start, buf, logColor(COL_INFO));
        }
    }

	pTextDriver_->endDraw();
}

namespace dconsole {

#ifdef DLL_BUILD
Console& Instance() {
    return GraphicsConsole::Instance();
}
#endif

} // namespace dconsole