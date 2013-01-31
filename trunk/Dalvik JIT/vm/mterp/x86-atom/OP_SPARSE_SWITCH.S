%verify "executed"
%include "x86-atom/OP_PACKED_SWITCH.S" { "func":"dvmInterpHandleSparseSwitch" }
