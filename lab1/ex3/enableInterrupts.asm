    .def    _enableInterrupts

_enableInterrupts:
	.asmfunc
    MVC     IER,B0		; IE4
    MVK		0x12,B1
    OR      B1,B0,B0
    MVC     B0,IER

    MVC     CSR,B0
    OR      1,B0,B0
    MVC     B0,CSR

    B       B3
    NOP     5
	.endasmfunc

	.end
