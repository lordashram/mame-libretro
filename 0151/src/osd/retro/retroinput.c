#define RETROK_TILDE 178

static UINT16 retrokbd_state2[RETROK_LAST];

struct kt_table {
	const char  *   mame_key_name;
	int retro_key_name;
        input_item_id   mame_key;
};

static kt_table ktable[]={
{"A",RETROK_a,ITEM_ID_A},
{"B",RETROK_b,ITEM_ID_B},
{"C",RETROK_c,ITEM_ID_C},
{"D",RETROK_d,ITEM_ID_D},
{"E",RETROK_e,ITEM_ID_E},
{"F",RETROK_f,ITEM_ID_F},
{"G",RETROK_g,ITEM_ID_G},
{"H",RETROK_h,ITEM_ID_H},
{"I",RETROK_i,ITEM_ID_I},
{"J",RETROK_j,ITEM_ID_J},
{"K",RETROK_k,ITEM_ID_K},
{"L",RETROK_l,ITEM_ID_L},
{"M",RETROK_m,ITEM_ID_M},
{"N",RETROK_n,ITEM_ID_N},
{"O",RETROK_o,ITEM_ID_O},
{"P",RETROK_p,ITEM_ID_P},
{"Q",RETROK_q,ITEM_ID_Q},
{"R",RETROK_r,ITEM_ID_R},
{"S",RETROK_s,ITEM_ID_S},
{"T",RETROK_t,ITEM_ID_T},
{"U",RETROK_u,ITEM_ID_U},
{"V",RETROK_v,ITEM_ID_V},
{"W",RETROK_w,ITEM_ID_W},
{"X",RETROK_x,ITEM_ID_X},
{"Y",RETROK_y,ITEM_ID_Y},
{"Z",RETROK_z,ITEM_ID_Z},
{"0",RETROK_0,ITEM_ID_0},
{"1",RETROK_1,ITEM_ID_1},
{"2",RETROK_2,ITEM_ID_2},
{"3",RETROK_3,ITEM_ID_3},
{"4",RETROK_4,ITEM_ID_4},
{"5",RETROK_5,ITEM_ID_5},
{"6",RETROK_6,ITEM_ID_6},
{"7",RETROK_7,ITEM_ID_7},
{"8",RETROK_8,ITEM_ID_8},
{"9",RETROK_9,ITEM_ID_9},
{"F1",RETROK_F1,ITEM_ID_F1},
{"F2",RETROK_F2,ITEM_ID_F2},
{"F3",RETROK_F3,ITEM_ID_F3},
{"F4",RETROK_F4,ITEM_ID_F4},
{"F5",RETROK_F5,ITEM_ID_F5},
{"F6",RETROK_F6,ITEM_ID_F6},
{"F7",RETROK_F7,ITEM_ID_F7},
{"F8",RETROK_F8,ITEM_ID_F8},
{"F9",RETROK_F9,ITEM_ID_F9},
{"F10",RETROK_F10,ITEM_ID_F10},
{"F11",RETROK_F11,ITEM_ID_F11},
{"F12",RETROK_F12,ITEM_ID_F12},
{"F13",RETROK_F13,ITEM_ID_F13},
{"F14",RETROK_F14,ITEM_ID_F14},
{"F15",RETROK_F15,ITEM_ID_F15},
{"Esc",RETROK_ESCAPE,ITEM_ID_ESC},
{"TILDE",RETROK_TILDE,ITEM_ID_TILDE},
{"MINUS",RETROK_MINUS,ITEM_ID_MINUS},
{"EQUALS",RETROK_EQUALS,ITEM_ID_EQUALS},
{"BKCSPACE",RETROK_BACKSPACE,ITEM_ID_BACKSPACE},
{"TAB",RETROK_TAB,ITEM_ID_TAB},
{"(",RETROK_LEFTPAREN,ITEM_ID_OPENBRACE},
{")",RETROK_RIGHTPAREN,ITEM_ID_CLOSEBRACE},
{"ENTER",RETROK_RETURN,ITEM_ID_ENTER},
{"·",RETROK_COLON,ITEM_ID_COLON},
{"\'",RETROK_QUOTE,ITEM_ID_QUOTE},
{"BCKSLASH",RETROK_BACKSLASH,ITEM_ID_BACKSLASH},
///**/BCKSLASH2*/RETROK_,ITEM_ID_BACKSLASH2},
{",",RETROK_COMMA,ITEM_ID_COMMA},
///**/STOP*/RETROK_,ITEM_ID_STOP},
{"/",RETROK_SLASH,ITEM_ID_SLASH},
{"SPACE",RETROK_SPACE,ITEM_ID_SPACE},
{"INS",RETROK_INSERT,ITEM_ID_INSERT},
{"DEL",RETROK_DELETE,ITEM_ID_DEL},
{"HOME",RETROK_HOME,ITEM_ID_HOME},
{"END",RETROK_END,ITEM_ID_END},
{"PGUP",RETROK_PAGEUP,ITEM_ID_PGUP},
{"PGDW",RETROK_PAGEDOWN,ITEM_ID_PGDN},
{"LEFT",RETROK_LEFT,ITEM_ID_LEFT},
{"RIGHT",RETROK_RIGHT,ITEM_ID_RIGHT},
{"UP",RETROK_UP,ITEM_ID_UP},
{"DOWN",RETROK_DOWN,ITEM_ID_DOWN},
{"KO",RETROK_KP0,ITEM_ID_0_PAD},
{"K1",RETROK_KP1,ITEM_ID_1_PAD},
{"K2",RETROK_KP2,ITEM_ID_2_PAD},
{"K3",RETROK_KP3,ITEM_ID_3_PAD},
{"K4",RETROK_KP4,ITEM_ID_4_PAD},
{"K5",RETROK_KP5,ITEM_ID_5_PAD},
{"K6",RETROK_KP6,ITEM_ID_6_PAD},
{"K7",RETROK_KP7,ITEM_ID_7_PAD},
{"K8",RETROK_KP8,ITEM_ID_8_PAD},
{"K9",RETROK_KP9,ITEM_ID_9_PAD},
{"K/",RETROK_KP_DIVIDE,ITEM_ID_SLASH_PAD},
{"K*",RETROK_KP_MULTIPLY,ITEM_ID_ASTERISK},
{"K-",RETROK_KP_MINUS,ITEM_ID_MINUS_PAD},
{"K+",RETROK_KP_PLUS,ITEM_ID_PLUS_PAD},
{"KDEL",RETROK_KP_PERIOD,ITEM_ID_DEL_PAD},
{"KRTRN",RETROK_KP_ENTER,ITEM_ID_ENTER_PAD},
{"PRINT",RETROK_PRINT,ITEM_ID_PRTSCR},
{"PAUSE",RETROK_PAUSE,ITEM_ID_PAUSE},
{"LSHFT",RETROK_LSHIFT,ITEM_ID_LSHIFT},
{"RSHFT",RETROK_RSHIFT,ITEM_ID_RSHIFT},
{"LCTRL",RETROK_LCTRL,ITEM_ID_LCONTROL},
{"RCTRL",RETROK_RCTRL,ITEM_ID_RCONTROL},
{"LALT",RETROK_LALT,ITEM_ID_LALT},
{"RALT",RETROK_RALT,ITEM_ID_RALT},
{"SCRLOCK",RETROK_SCROLLOCK,ITEM_ID_SCRLOCK},
{"NUMLOCK",RETROK_NUMLOCK,ITEM_ID_NUMLOCK},
{"CPSLOCK",RETROK_CAPSLOCK,ITEM_ID_CAPSLOCK},
{"LMETA",RETROK_LMETA,ITEM_ID_LWIN},
{"RMETA",RETROK_RMETA,ITEM_ID_RWIN},
{"MENU",RETROK_MENU,ITEM_ID_MENU},
{"BREAK",RETROK_BREAK,ITEM_ID_CANCEL},
{"-1",-1,ITEM_ID_INVALID},
};

enum
{
        RETROPAD_B,
        RETROPAD_Y,
        RETROPAD_SELECT,
        RETROPAD_START,
        RETROPAD_PAD_UP,
        RETROPAD_PAD_DOWN,
        RETROPAD_PAD_LEFT,
        RETROPAD_PAD_RIGHT,
        RETROPAD_A,
        RETROPAD_X,
        RETROPAD_L,
        RETROPAD_R,
        RETROPAD_L2,
        RETROPAD_R2,
        RETROPAD_L3,
        RETROPAD_R3,
        RETROPAD_TOTAL
}; 

static INT32 retrokbd_get_state(void *device_internal, void *item_internal)
{
	UINT8 *itemdata = (UINT8 *)item_internal;
	return *itemdata ;
}

static INT32 generic_axis_get_state(void *device_internal, void *item_internal)
{
	INT32 *axisdata = (INT32 *) item_internal;
	return *axisdata;
}

static INT32 generic_button_get_state(void *device_internal, void *item_internal)
{
	INT32 *itemdata = (INT32 *) item_internal;
	return *itemdata >> 7;
}

#define input_device_item_add_j1(a,b,c,d,e) joy0_device->add_item(b,d,e,c)
#define input_device_item_add_j2(a,b,c,d,e) joy1_device->add_item(b,d,e,c)
#define input_device_item_add_mouse(a,b,c,d,e) mouse_device->add_item(b,d,e,c)
#define input_device_item_add_kbd(a,b,c,d,e) retrokbd_device->add_item(b,d,e,c)

#ifdef RETRO_AND
#define input_device_item_add_p1(a,b,c,d,e) P1_device->add_item(b,d,e,c)
#define input_device_item_add_p2(a,b,c,d,e) P2_device->add_item(b,d,e,c)
#endif

static void initInput(running_machine &machine)
{
	int i,button;
	char defname[20],tempname[512];

   if (mouse_enable)
   {
   	//MOUSE
      	mouse_device = machine.input().device_class(DEVICE_CLASS_MOUSE).add_device("Mice1");
      	// add the axes
      	input_device_item_add_mouse(mouse_device , "X", &mouseLX, ITEM_ID_XAXIS, generic_axis_get_state);
      	input_device_item_add_mouse(mouse_device , "Y", &mouseLY, ITEM_ID_YAXIS, generic_axis_get_state);
	// add the buttons
	for (button = 0; button < 4; button++)
        {
         	input_item_id itemid = (input_item_id) (ITEM_ID_BUTTON1+button);
        	sprintf(defname, "B%d", button + 1);
	        input_device_item_add_mouse(mouse_device, defname, &mouseBUT[button], itemid, generic_button_get_state);
      	}
   }

	//JOY0
	joy0_device=machine.input().device_class(DEVICE_CLASS_JOYSTICK).add_device("Joy0");
	// add the axes
	input_device_item_add_j1 (0, "LX", &joy0_a1[0], ITEM_ID_XAXIS, generic_axis_get_state);
	input_device_item_add_j1 (0, "LY", &joy0_a1[1], ITEM_ID_YAXIS, generic_axis_get_state);
	input_device_item_add_j1 (0, "RX", &joy0_a2[0], (input_item_id)(ITEM_ID_XAXIS+2), generic_axis_get_state);
	input_device_item_add_j1 (0, "RY", &joy0_a2[1], (input_item_id)(ITEM_ID_YAXIS+1), generic_axis_get_state);
 	//add the buttons
	for(i=0;i<MAX_BUTTONS;i++)joy0_bstate[i] = 0;
	input_device_item_add_j1 (joy0_device,"A", &joy0_bstate[RETROPAD_A],(input_item_id)(ITEM_ID_BUTTON1+0),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"B", &joy0_bstate[RETROPAD_B],(input_item_id)(ITEM_ID_BUTTON1+1),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"X", &joy0_bstate[RETROPAD_X],(input_item_id)(ITEM_ID_BUTTON1+2),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"Y", &joy0_bstate[RETROPAD_Y],(input_item_id)(ITEM_ID_BUTTON1+3),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"L", &joy0_bstate[RETROPAD_L],(input_item_id)(ITEM_ID_BUTTON1+4),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"R", &joy0_bstate[RETROPAD_R],(input_item_id)(ITEM_ID_BUTTON1+5),generic_button_get_state );

#ifndef RETRO_AND
	input_device_item_add_j1 (joy0_device,"L2", &joy0_bstate[RETROPAD_L2],(input_item_id)(ITEM_ID_BUTTON1+6),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"R2", &joy0_bstate[RETROPAD_R2],(input_item_id)(ITEM_ID_BUTTON1+7),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"L3", &joy0_bstate[RETROPAD_L3],(input_item_id)(ITEM_ID_BUTTON1+8),generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"R3", &joy0_bstate[RETROPAD_R3],(input_item_id)(ITEM_ID_BUTTON1+9),generic_button_get_state );

	input_device_item_add_j1 (joy0_device,"Pad Up", &joy0_bstate[RETROPAD_PAD_UP],ITEM_ID_HAT1UP,generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"Pad Down", &joy0_bstate[RETROPAD_PAD_DOWN],ITEM_ID_HAT1DOWN,generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"Pad Left", &joy0_bstate[RETROPAD_PAD_LEFT],ITEM_ID_HAT1LEFT,generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"Pad Right", &joy0_bstate[RETROPAD_PAD_RIGHT],ITEM_ID_HAT1RIGHT,generic_button_get_state );

#else
	P1_device = machine.input().device_class(DEVICE_CLASS_KEYBOARD).add_device("Pad1", P1_device);

	input_device_item_add_p1 (joy0_device,"L2", &joy0_bstate[RETROPAD_L2],(input_item_id)(ITEM_ID_ENTER+0),retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"R2", &joy0_bstate[RETROPAD_R2],(input_item_id)(ITEM_ID_TAB+0),retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"L3", &joy0_bstate[RETROPAD_L3],(input_item_id)(ITEM_ID_F3+0),retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"R3", &joy0_bstate[RETROPAD_R3],(input_item_id)(ITEM_ID_F11+0),retrokbd_get_state );

	input_device_item_add_p1 (joy0_device,"Pad Up", &joy0_bstate[RETROPAD_PAD_UP],ITEM_ID_UP,retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"Pad Down", &joy0_bstate[RETROPAD_PAD_DOWN],ITEM_ID_DOWN,retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"Pad Left", &joy0_bstate[RETROPAD_PAD_LEFT],ITEM_ID_LEFT,retrokbd_get_state );
	input_device_item_add_p1 (joy0_device,"Pad Right", &joy0_bstate[RETROPAD_PAD_RIGHT],ITEM_ID_RIGHT,retrokbd_get_state );
#endif
	input_device_item_add_j1 (joy0_device,"START", &joy0_bstate[RETROPAD_START],ITEM_ID_START,generic_button_get_state );
	input_device_item_add_j1 (joy0_device,"SLECT", &joy0_bstate[RETROPAD_SELECT],ITEM_ID_SELECT,generic_button_get_state );
	
	//JOY1
	joy1_device=machine.input().device_class(DEVICE_CLASS_JOYSTICK).add_device("Joy1");
	// add the axes
	input_device_item_add_j2 (0, "LX", &joy1_a1[0], ITEM_ID_XAXIS, generic_axis_get_state);
	input_device_item_add_j2 (0, "LY", &joy1_a1[1], ITEM_ID_YAXIS, generic_axis_get_state);
	input_device_item_add_j2 (0, "RX", &joy1_a2[0], (input_item_id)(ITEM_ID_XAXIS+2), generic_axis_get_state);
	input_device_item_add_j2 (0, "RY", &joy1_a2[1], (input_item_id)(ITEM_ID_YAXIS+1), generic_axis_get_state);
 	//add the buttons
	for(i=0;i<MAX_BUTTONS;i++)joy1_bstate[i] = 0;
	input_device_item_add_j2 (joy1_device,"A", &joy1_bstate[RETROPAD_A],(input_item_id)(ITEM_ID_BUTTON1+0),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"B", &joy1_bstate[RETROPAD_B],(input_item_id)(ITEM_ID_BUTTON1+1),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"X", &joy1_bstate[RETROPAD_X],(input_item_id)(ITEM_ID_BUTTON1+2),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"Y", &joy1_bstate[RETROPAD_Y],(input_item_id)(ITEM_ID_BUTTON1+3),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"L", &joy1_bstate[RETROPAD_L],(input_item_id)(ITEM_ID_BUTTON1+4),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"R", &joy1_bstate[RETROPAD_R],(input_item_id)(ITEM_ID_BUTTON1+5),generic_button_get_state );

#ifndef RETRO_AND
	input_device_item_add_j2 (joy1_device,"L2", &joy1_bstate[RETROPAD_L2],(input_item_id)(ITEM_ID_BUTTON1+6),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"R2", &joy1_bstate[RETROPAD_R2],(input_item_id)(ITEM_ID_BUTTON1+7),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"L3", &joy1_bstate[RETROPAD_L3],(input_item_id)(ITEM_ID_BUTTON1+8),generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"R3", &joy1_bstate[RETROPAD_R3],(input_item_id)(ITEM_ID_BUTTON1+9),generic_button_get_state );

	input_device_item_add_j2 (joy1_device,"Pad Up", &joy1_bstate[RETROPAD_PAD_UP],ITEM_ID_HAT1UP,generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"Pad Down", &joy1_bstate[RETROPAD_PAD_DOWN],ITEM_ID_HAT1DOWN,generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"Pad Left", &joy1_bstate[RETROPAD_PAD_LEFT],ITEM_ID_HAT1LEFT,generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"Pad Right", &joy1_bstate[RETROPAD_PAD_RIGHT],ITEM_ID_HAT1RIGHT,generic_button_get_state );
#else
	P2_device = machine.input().device_class(DEVICE_CLASS_KEYBOARD).add_device("Pad2", P2_device);

	input_device_item_add_p2 (joy1_device,"L2", &joy1_bstate[RETROPAD_L2],(input_item_id)(ITEM_ID_ENTER+0),retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"R2", &joy1_bstate[RETROPAD_R2],(input_item_id)(ITEM_ID_TAB+0),retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"L3", &joy1_bstate[RETROPAD_L3],(input_item_id)(ITEM_ID_F3+0),retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"R3", &joy1_bstate[RETROPAD_R3],(input_item_id)(ITEM_ID_F11+0),retrokbd_get_state );

	input_device_item_add_p2 (joy1_device,"Pad Up", &joy1_bstate[RETROPAD_PAD_UP],ITEM_ID_R,retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"Pad Down", &joy1_bstate[RETROPAD_PAD_DOWN],ITEM_ID_F,retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"Pad Left", &joy1_bstate[RETROPAD_PAD_LEFT],ITEM_ID_D,retrokbd_get_state );
	input_device_item_add_p2 (joy1_device,"Pad Right", &joy1_bstate[RETROPAD_PAD_RIGHT],ITEM_ID_G,retrokbd_get_state);
#endif
	input_device_item_add_j2 (joy1_device,"START", &joy1_bstate[RETROPAD_START],ITEM_ID_START,generic_button_get_state );
	input_device_item_add_j2 (joy1_device,"SLECT", &joy1_bstate[RETROPAD_SELECT],ITEM_ID_SELECT,generic_button_get_state );

	//KEYBOARD
	retrokbd_device = machine.input().device_class(DEVICE_CLASS_KEYBOARD).add_device("Retrokdb");

	if (retrokbd_device == NULL)
			fatalerror("KBD Error creating keyboard device\n");


   	for(i = 0; i < RETROK_LAST; i++){
		retrokbd_state[i]=0;
 		retrokbd_state2[i]=0;
	}

   	i=0;
   	do{
 		input_device_item_add_kbd(retrokbd_device,\
		 ktable[i].mame_key_name, &retrokbd_state[ktable[i].retro_key_name],ktable[i].mame_key,retrokbd_get_state);
 		i++;
   	}while(ktable[i].retro_key_name!=-1);
 
   	fprintf(stderr, "SOURCE FILE: %s\n", machine.system().source_file);
   	fprintf(stderr, "PARENT: %s\n", machine.system().parent);
   	fprintf(stderr, "NAME: %s\n", machine.system().name);
   	fprintf(stderr, "DESCRIPTION: %s\n", machine.system().description);
   	fprintf(stderr, "YEAR: %s\n", machine.system().year);
   	fprintf(stderr, "MANUFACTURER: %s\n", machine.system().manufacturer);

}

void retro_poll_mame_input()
{
	int i=0;

	input_poll_cb();

   if (mouse_enable)
   {
	//MOUSE
      	static int mbL=0,mbR=0;
      	int mouse_l;
      	int mouse_r;
      	int16_t mouse_x;
      	int16_t mouse_y;

      	mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
      	mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
      	mouse_l = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
      	mouse_r = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
      	mouseLX=mouse_x*INPUT_RELATIVE_PER_PIXEL;;
      	mouseLY=mouse_y*INPUT_RELATIVE_PER_PIXEL;;

      	if(mbL==0 && mouse_l)
      	{
      	   mbL=1;		
      	   mouseBUT[0]=0x80;
      	}
      	else if(mbL==1 && !mouse_l)
      	{	
      	   mouseBUT[0]=0;
      	   mbL=0;
      	}

      	if(mbR==0 && mouse_r)
      	{
      	   mbR=1;
      	   mouseBUT[1]=1;
      	}
      	else if(mbR==1 && !mouse_r)
      	{
      	   mouseBUT[1]=0;
      	   mbR=0;
      	}
   }

	//JOY0
	for(i=0;i<MAX_BUTTONS;i++)
		joy0_bstate[i] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,i)?0x80:0;

	joy0_a1[0] = 2*(input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
      	joy0_a1[1] = 2*(input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));
      	joy0_a2[0] = 2*(input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      	joy0_a2[1] = 2*(input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

	//JOY1
	for(i=0;i<MAX_BUTTONS;i++)
		joy1_bstate[i] = input_state_cb(1, RETRO_DEVICE_JOYPAD, 0,i)?0x80:0;

      	joy1_a1[0] = 2*(input_state_cb(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
      	joy1_a1[1] = 2*(input_state_cb(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));
      	joy1_a2[0] = 2*(input_state_cb(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      	joy1_a2[1] = 2*(input_state_cb(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

	//KBD
	//TODO: handle mods:SHIFT/CTRL/ALT/META/NUMLOCK/CAPSLOCK/SCROLLOCK
	i=0;
	do{
 		retrokbd_state[ktable[i].retro_key_name]=input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,ktable[i].retro_key_name)?0x80:0;
 		if(retrokbd_state[ktable[i].retro_key_name] && retrokbd_state2[ktable[i].retro_key_name]==0){
			ui_ipt_pushchar=ktable[i].retro_key_name;
			retrokbd_state2[ktable[i].retro_key_name]=1;
 		}
		else if(!retrokbd_state[ktable[i].retro_key_name] && retrokbd_state2[ktable[i].retro_key_name]==1)
			retrokbd_state2[ktable[i].retro_key_name]=0;

 		i++;

	}while(ktable[i].retro_key_name!=-1);

}
