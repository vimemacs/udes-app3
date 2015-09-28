    .def    _enableInterrupts

_enableInterrupts:
	.asmfunc
	; Configuration des interruptions
	; IE4, 15, 11
    MVC     IER,B0
    MVK		0x8812,B1
    OR      B1,B0,B0
    MVC     B0,IER

    MVC     CSR,B0
    OR      1,B0,B0
    MVC     B0,CSR

    B       B3
    NOP     5
	.endasmfunc

	.end
