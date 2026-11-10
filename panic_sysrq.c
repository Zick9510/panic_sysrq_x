// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) "ZPanic: " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/umh.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zick");
MODULE_DESCRIPTION("Panic mode via SysRq");
MODULE_VERSION("1.1");

#define ZPANIC_KEY 'x'

static char *panic_argv[] = { "/usr/local/sbin/panic", NULL };

static char *panic_envp[] = {
	"HOME=/",
	"PATH=/sbin:/bin:/usr/sbin:/usr/bin",
	NULL
};

/* Corre en contexto atómico (filtro de input): no puede dormir. */
static void zpanic_handler(u8 key) {
	int ret;

	pr_info("SysRq+%c detected\n", key);

	/*
	 * UMH_NO_WAIT es el modo adecuado aquí (usa GFP_ATOMIC y no espera).
	 * ret solo indica si el helper se pudo encolar, no cómo termina el script.
	 */
	ret = call_usermodehelper(panic_argv[0], panic_argv, panic_envp, UMH_NO_WAIT);
	if (ret) {
		pr_err("failed to queue %s: %d\n", panic_argv[0], ret);
  } else {
		pr_info("helper queued\n");
  }
}

static const struct sysrq_key_op zpanic_op = {
	.handler	= zpanic_handler,
	.help_msg	= "panic-mode(x)",
	.action_msg	= "Starting panic mode",
	.enable_mask	= SYSRQ_ENABLE_KEYBOARD,
};

static int __init zpanic_init(void) {
	int ret;

	ret = register_sysrq_key(ZPANIC_KEY, &zpanic_op);
	if (ret) {
		pr_err("cannot register SysRq+%c (key already in use?): %d\n",
		       ZPANIC_KEY, ret);
		return -EBUSY;
	}

	pr_info("SysRq+%c registered\n", ZPANIC_KEY);
	return 0;
}

static void __exit zpanic_exit(void) {
	unregister_sysrq_key(ZPANIC_KEY, &zpanic_op);
	pr_info("unloaded\n");
}

module_init(zpanic_init);
module_exit(zpanic_exit);
