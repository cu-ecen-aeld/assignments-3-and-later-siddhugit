### OOPS console output

```
echo “hello_world” > /dev/faulty
[   33.722815] Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
[   33.723096] Mem abort info:
[   33.723156]   ESR = 0x0000000096000045
[   33.723234]   EC = 0x25: DABT (current EL), IL = 32 bits
[   33.723346]   SET = 0, FnV = 0
[   33.723581]   EA = 0, S1PTW = 0
[   33.723650]   FSC = 0x05: level 1 translation fault
[   33.723757] Data abort info:
[   33.723826]   ISV = 0, ISS = 0x00000045
[   33.723917]   CM = 0, WnR = 1
[   33.724035] user pgtable: 4k pages, 39-bit VAs, pgdp=0000000043e23000
[   33.724183] [0000000000000000] pgd=0000000000000000, p4d=0000000000000000, pud=0000000000000000
[   33.724496] Internal error: Oops: 0000000096000045 [#1] PREEMPT SMP
[   33.724733] Modules linked in: scull(O) faulty(O) hello(O)
[   33.725012] CPU: 1 PID: 352 Comm: sh Tainted: G           O      5.15.175-yocto-standard #1
[   33.725204] Hardware name: linux,dummy-virt (DT)
[   33.725393] pstate: 80000005 (Nzcv daif -PAN -UAO -TCO -DIT -SSBS BTYPE=--)
[   33.725578] pc : faulty_write+0x18/0x20 [faulty]
[   33.725856] lr : vfs_write+0xf8/0x2a0
[   33.725929] sp : ffffffc00a15bd80
[   33.725987] x29: ffffffc00a15bd80 x28: ffffff800236e040 x27: 0000000000000000
[   33.726139] x26: 0000000000000000 x25: 0000000000000000 x24: 0000000000000000
[   33.726259] x23: 0000000000000000 x22: ffffffc00a15bdc0 x21: 0000005584527030
[   33.726383] x20: ffffff8001ab5600 x19: 0000000000000012 x18: 0000000000000000
[   33.726511] x17: 0000000000000000 x16: 0000000000000000 x15: 0000000000000000
[   33.726637] x14: 0000000000000000 x13: 0000000000000000 x12: 0000000000000000
[   33.726761] x11: 0000000000000000 x10: 0000000000000000 x9 : ffffffc008270a18
[   33.726894] x8 : 0000000000000000 x7 : 0000000000000000 x6 : 0000000000000000
[   33.727013] x5 : 0000000000000001 x4 : ffffffc000b95000 x3 : ffffffc00a15bdc0
[   33.727129] x2 : 0000000000000012 x1 : 0000000000000000 x0 : 0000000000000000
[   33.727309] Call trace:
[   33.727453]  faulty_write+0x18/0x20 [faulty]
[   33.727546]  ksys_write+0x74/0x110
[   33.727612]  __arm64_sys_write+0x24/0x30
[   33.727681]  invoke_syscall+0x5c/0x130
[   33.727749]  el0_svc_common.constprop.0+0x4c/0x100
[   33.727828]  do_el0_svc+0x4c/0xc0
[   33.727888]  el0_svc+0x28/0x80
[   33.727943]  el0t_64_sync_handler+0xa4/0x130
[   33.728017]  el0t_64_sync+0x1a0/0x1a4
[   33.728142] Code: d2800001 d2800000 d503233f d50323bf (b900003f)
[   33.728357] ---[ end trace c0c8d81af7f9e514 ]---
Segmentation fault

Poky (Yocto Project Reference Distro) 4.0.24 qemuarm64 /dev/ttyAMA0
```
