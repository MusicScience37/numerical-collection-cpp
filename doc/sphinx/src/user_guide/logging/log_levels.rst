Log Levels
================

.. table:: Log levels in the ascending order of severity

    .. csv-table::
        :header-rows: 1

        Log level,          Usage in this library
        trace,              Internal trace logs for developer of this library.
        debug,              None. (Meant for use in user code to write debug logs.)
        iteration,          Log of each iteration.
        iteration_label,    Label of iteration logs.
        summary,            Summary of calculations.
        info,               None. (Meant for use in user code to show some information.)
        warning,            Warnings of undesirable conditions.
        error,              Errors preventing further processing. (Usually used with exceptions.)
        critical,           None. (Meant for use in conditions which may cause death of processes.)
