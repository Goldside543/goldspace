# SPDX-License-Identifier: GPL-2.0-only

.global gpf_isr_wrapper

gpf_isr_wrapper:
  pushal
  cld              # C code following the sysV ABI requires DF to be clear on function entry
  call gpf_handler
  popal
  iret
