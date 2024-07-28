# Log Levels

```{table} Log levels in the ascending order of severity

| Log level       | Usage in this library                                                   |
| :-------------- | :---------------------------------------------------------------------- |
| trace           | Internal trace logs for developer of this library.                      |
| debug           | Log for debugging.                                                      |
| iteration       | Log of each iteration.                                                  |
| iteration_label | Label of iteration logs. (Not usable for output log level.)             |
| summary         | Summary of calculations.                                                |
| info            | None. (Meant for use in user code to show some information.)            |
| warning         | Warnings of undesirable conditions.                                     |
| error           | Errors preventing further processing. (Usually used with exceptions.)   |
| critical        | None. (Meant for use in conditions which may cause death of processes.) |
| off             | Turn off output. (Only for output log level.)                           |
```
