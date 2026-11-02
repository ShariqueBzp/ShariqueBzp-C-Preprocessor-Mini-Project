#ifndef TOOLKIT_H
#define TOOLKIT_H

#define TOOL_NAME      "mypreprocessor"
#define TOOL_VERSION   "2.0"
#define MAX_ITEMS      10
#define SCALE(x)       ((x) * 2)
#define ARRAY_LEN(a)   (sizeof(a) / sizeof((a)[0]))

int toolkit_ready(void);

#endif
