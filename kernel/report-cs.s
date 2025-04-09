# SPDX-License-Identifier: GPL-2.0-only

.global report_cs

report_cs:
  push %cs
  pop %eax
  ret
