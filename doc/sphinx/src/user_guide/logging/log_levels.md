# Log Levels

```{table} Log levels in the ascending order of severity

| Log level       | Meaning                                                      | Usage in this library                                         |
| :-------------- | :----------------------------------------------------------- | :------------------------------------------------------------ |
| trace           | Logs for debugging.                                          | Internal trace logs for developer of this library.            |
| debug           | Logs for debugging.                                          | Logs for debugging.                                           |
| iteration       | Logs of each iteration.                                      | Logs of each iteration.                                       |
| iteration_label | Labels of iteration logs. Not usable as output log level.    | Labels of iteration logs.                                     |
| summary         | Summary of calculations.                                     | Summary of calculations.                                      |
| info            | Some information.                                            | None.                                                         |
| warning         | Warnings of undesirable conditions.                          | Warnings of undesirable conditions.                           |
| error           | Errors preventing further processing.                        | Errors preventing further processing. Used with exceptions.   |
| critical        | Errors which may cause death of processes.                   | None.                                                         |
| off             | Turn off output. Used in output log level.                   | None.                                                         |
```
