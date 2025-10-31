#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "joystick.h"

static int joystick_fd = -1;

int open_joystick()
{
	joystick_fd = open(JOYSTICK_DEVNAME, O_RDONLY | O_NONBLOCK); /* read write for force feedback? */
	if (joystick_fd < 0)
		return joystick_fd;

	/* maybe ioctls to interrogate features here? */

	return joystick_fd;
}

int read_joystick_event(struct js_event *jse)
{
	int bytes;

	bytes = read(joystick_fd, jse, sizeof(*jse)); 

	if (bytes == -1)
		return 0;

	if (bytes == sizeof(*jse))
		return 1;

	printf("Unexpected bytes from joystick:%d\n", bytes);

	return -1;
}

void close_joystick()
{
	close(joystick_fd);
}


/* a little test program */
int processaJoystick(int *buttons, struct js_event* jse)
{
	int rc;
	rc = read_joystick_event(jse);
	//usleep(1000);
	if (rc == 1)
	{
		if(jse->type == JS_EVENT_BUTTON)
		{
			if(jse->value == 1)
			{
				//printf("Button pressed\n");
				//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
				buttons[jse->number] = 1;
				
	
			}
			else if(jse->value == 0)
			{
				//printf("Button released\n");
				//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
				buttons[jse->number] = 0;
			}
		}
		else if(jse->type == JS_EVENT_AXIS)
		{
				
			if(jse->number == 5)
			{
				//printf("3st Axis X\n");
				
				
				if(jse->value != 0)
				{
					//printf("Directional pressed\n");
					//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
					if(jse->value == 32767)
					{
						buttons[9] = 1;
					}
					else if(jse->value == -32767)
					{
						buttons[11] = 1;
					}

				}
				else if(jse->value == 0)
				{
					//printf("Directional released\n");
					//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
					if(jse->number == 5)
					{
						buttons[9] = buttons[11] = 0;
					}
				}
			}
			else if(jse->number == 6)
			{
				if(jse->value != 0)
				{
					
					//printf("Directional pressed\n");
					//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
					if(jse->value == -32767)
					{
						buttons[8] = 1;
					}

					else if(jse->value == 32767)
					{
						buttons[10] = 1;
					}
				}
				else
				{
					//printf("Directional released\n");
					//printf("Event: Value %hd, type: %u, axis/button: %u\n", jse->value, jse->type, jse->number);
					if(jse->number == 6)
					{
						buttons[8] = buttons[10] = 0;
					}
				}
			}
		}	/*	
		else if(jse->type == JS_EVENT_INIT)
		{
			//printf("Initial state\n");
		}
		else
		{
			//printf("UNDEFINED STATE %d\n",jse->type);
		}
		*/
	}
	
	return 0;
}
