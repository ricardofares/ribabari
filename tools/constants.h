#ifndef OS_PROJECT_CONSTANTS_H
#define OS_PROJECT_CONSTANTS_H

/**
 * Debug Options
 */

/* General Debug Options */

/**
 * It enables all subsystems from the
 * operating system simulator debugging
 * system.
 */
#define OS_DEBUG 0

/* Kernel Debug Options */

/**
 * It enables the kernel debugging system,
 * as well as cited before, if OS_DEBUG is
 * activated then this debugging system is
 * activated as well.
 */
#define OS_KERNEL_DEBUG 0

/**
 * It enables the evaluation debug part
 * from the kernel debugging system.
 */
#define OS_KERNEL_EVAL_DEBUG 0

/* Process Debug Options */

/**
 * It enables the process debugging system,
 * as well as cited before, if OS_DEBUG is
 * activated then this debugging system is
 * activated as well.
 */
#define OS_PROCESS_DEBUG 0

/* File System Debug Options */

/**
 * It enables the file system debugging system,
 * as well as cited before, if OS_DEBUG is
 * activated then this debugging system is
 * activated as well.
 */
#define OS_FS_DEBUG 0

/* Semaphore Debug Options */

/**
 * It enables the semaphore debugging system,
 * as well as cited before, if OS_DEBUG is
 * activated then this debugging system is
 * activated as well.
 */
#define OS_SEM_DEBUG 0

#endif // OS_PROJECT_CONSTANTS_H
