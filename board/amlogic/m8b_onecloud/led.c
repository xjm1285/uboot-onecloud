#include <common.h>

#include <asm/errno.h>
#include <amlogic/gpio.h>
#include <command.h>

typedef enum LED_ID {
	LED_ID_RED = 0,
	LED_ID_GREEN,
	LED_ID_BLUE,

	LED_ID_INVALID,
} LED_ID;

static const char *led_id_lable[] = {
	[LED_ID_RED]	 = "red",
	[LED_ID_GREEN]	 = "green",
	[LED_ID_BLUE]	 = "blue",
};

typedef enum LED_STATE {
	LED_STATE_OFF,
	LED_STATE_ON,
	LED_STATE_TOGGLE,

	LED_STATE_INVALID,
} LED_STATE;

static const char *led_state_label[] = {
	[LED_STATE_OFF]		 = "off",
	[LED_STATE_ON]		 = "on",
	[LED_STATE_TOGGLE]	 = "toggle",
};

static LED_ID onecloud_led_getId(const char *label)
{
	LED_ID id;
	for(id = 0; id < LED_ID_INVALID; id++)
		if(strcmp(label, led_id_lable[id]) == 0)
			return id;

	return LED_ID_INVALID;
}

static int onecloud_led_getPin(LED_ID id)
{
	if(id == LED_ID_RED)
		return GPIOAO_2;
	else if(id == LED_ID_GREEN)
		return GPIOAO_3;
	else if(id == LED_ID_BLUE)
		return GPIOAO_4;
	else
		return -EINVAL;
}

static LED_STATE onecloud_led_get(LED_ID id)
{
	int pin = onecloud_led_getPin(id);
	if(pin < 0)
		return LED_STATE_INVALID;

	return ((amlogic_get_value(pin) == 1) ? LED_STATE_ON : LED_STATE_OFF);
}

static int onecloud_led_set(LED_ID id, LED_STATE state)
{
	int pin = onecloud_led_getPin(id);
	if(pin < 0)
		return pin;

	if(state == LED_STATE_TOGGLE)
		state = (onecloud_led_get(id) == LED_STATE_ON) ? LED_STATE_OFF : LED_STATE_ON;

	int value = 0;
	if(state == LED_STATE_OFF)
		value = 0;
	else if(state == LED_STATE_ON)
		value = 1;
	else
		return -EINVAL;

	amlogic_gpio_direction_output(pin, value);
	return 0;
}

void onecloud_led_init(void)
{
	onecloud_led_set(LED_ID_RED, LED_STATE_ON);
	onecloud_led_set(LED_ID_GREEN, LED_STATE_OFF);
	onecloud_led_set(LED_ID_BLUE, LED_STATE_OFF);
}

int do_led(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc < 2)
		return cmd_usage(cmdtp);

	if(strcmp(argv[1], "list") == 0) {
		// List LEDs
		LED_ID id;
		for(id = 0; id < LED_ID_INVALID; id++) {
			LED_STATE state = onecloud_led_get(id);
			printf("%s\t%s\n", led_id_lable[id], led_state_label[state]);
		}
		return 0;
	}

	LED_ID id = onecloud_led_getId(argv[1]);
	if(id >= LED_ID_INVALID) {
		printf("LED '%s' not found\n", argv[1]);
		return 1;
	}

	if(argc == 2) {
		// Show LED state
		LED_STATE state = onecloud_led_get(id);
		printf("%s\n", led_state_label[state]);
		return 0;
	}

	// Change LED state
	LED_STATE state;
	for(state = 0; state < LED_STATE_INVALID; state++)
		if(strcmp(argv[2], led_state_label[state]) == 0)
			return ((onecloud_led_set(id, state) >= 0) ? 0 : 1);

	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	led, 4, 1, do_led,
	"manage LEDs",
	"list                       Show a list of LEDs\n"
	"led <led_label>                Get LED state\n"
	"led <led_label> on|off|toggle  Change LED state"
);
