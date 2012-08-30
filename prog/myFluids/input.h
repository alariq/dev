#ifndef __INPUT_H__
#define __INPUT_H__

struct mouse_data {
	int x;
	int y;
	int rel_x;
	int rel_y;
	bool buttons[3];
};

#endif // __INPUT_H__
