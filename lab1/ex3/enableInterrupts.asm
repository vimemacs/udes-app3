    .def    _enableInterrupts

_enableInterrupts:
    ; NME - activation des interruptions 'nonreset'
    MVC     IER,B0
    OR      2,B0,B0
    MVC     B0,IER

    ; IE4 - Interrupt Enable
    MVC     IER,B0
    OR      0x10,B0,B0       ; cf. "Interrupt Service Table" (INT_15)
    MVC     B0,IER

    ; IE15 - Interrupt Enable
    MVC     IER,B0
    OR      0x8000,B0,B0     ; cf. "Interrupt Service Table" (INT_15)
    MVC     B0,IER

    ; CSR configuration
    MVC     CSR,B0
    OR      1,B0,B0             ; GIE = 1: enable all interrupts
    MVC     B0,CSR

    B       B3
    NOP     5
