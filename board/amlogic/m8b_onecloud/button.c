#include <common.h>

#include <asm/errno.h>
#include <amlogic/gpio.h>
#include <command.h>

typedef enum BUTTON_ID {
	BUTTON_ID_RESET = 0,

	BUTTON_ID_INVALID,
} BUTTON_ID;

static const char *button_id_lable[] = {
	[BUTTON_ID_RESET]	 = "reset",
};

typedef enum BUTTON_STATE {
	BUTTON_STATE_RELEASED,
	BUTTON_STATE_PRESSED,

	BUTTON_STATE_INVALID,
} BUTTON_STATE;

static const char *button_state_label[] = {
	[BUTTON_STATE_RELEASED]	 = "released",
	[BUTTON_STATE_PRESSED]	 = "pressed",
};

static BUTTON_ID onecloud_button_getId(const char *label)
{
	BUTTON_ID id;
	for(id = 0; id < BUTTON_ID_INVALID; id++)
		if(strcmp(label, button_id_lable[id]) == 0)
			return id;

	return BUTTON_ID_INVALID;
}

static int onecloud_button_getPin(BUTTON_ID id)
{
	if(id == BUTTON_ID_RESET)
		return GPIOAO_5;
	else
		return -EINVAL;
}

static BUTTON_STATE onecloud_button_get(BUTTON_ID id)
{
	int pin = onecloud_button_getPin(id);
	if(pin < 0)
		return BUTTON_STATE_INVALID;

	amlogic_gpio_direction_input(pin);
	return ((amlogic_get_value(pin) == 0) ? BUTTON_STATE_PRESSED : BUTTON_STATE_RELEASED);
}

void onecloud_button_init(void)
{
	onecloud_button_get(BUTTON_ID_RESET);
}

int do_button(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 2)
		return cmd_usage(cmdtp);

	if(strcmp(argv[1], "list") == 0) {
		// List buttons
		BUTTON_ID id;
		for(id = 0; id < BUTTON_ID_INVALID; id++) {
			BUTTON_STATE state = onecloud_button_get(id);
			printf("%s\t%s\n", button_id_lable[id], button_state_label[state]);
		}
		return 0;
	}

	BUTTON_ID id = onecloud_button_getId(argv[1]);
	if(id >= BUTTON_ID_INVALID) {
		printf("Button '%s' not found\n", argv[1]);
		return 1;
	}

	// Show button state
	BUTTON_STATE state = onecloud_button_get(id);
	printf("%s\n", button_state_label[state]);
	return ((state == BUTTON_STATE_PRESSED) ? 0 : 1);
}

U_BOOT_CMD(
	button, 2, 1, do_button,
	"manage buttons",
	"list            Show a list of buttons\n"
	"button <button_label>  Get button state"
);
