#ifndef WINDOW_LIB
#define WINDOW_LIB

/************************************************************************
  Window
 ************************************************************************/
 
typedef struct {
    	int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

#endif
