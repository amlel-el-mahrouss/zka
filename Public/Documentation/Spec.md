===================================
# 0: General Information
===================================

- ABI and Format: PEF/PE32+.
- Kernel architecture: Microkernel.
- Language: C++/(Assembly (AMD64, X64000, X86S, ARM64, PowerPC, RISCV))

===================================
# 1: The NewKernel
===================================

- Drive/Device Abstraction.
- SMP, Preemptive Multi Threading.
- Separation of Files/Devices.
- Networking.
- Hardware Abstraction Layer.
- Native Filesystem support (NewFS, FAT32 and HCFS).
- Program Loaders.
- Thread Local Storage.
- Semaphore, Locks, Timers.
- Canary mechanisms.
- Dynamic Libraries.
- Cross Platform.
- Permission Selectors.

===================================
# 2: The Filesystem
===================================

- Catalog based with forks.
- Large storage support.
- Long file names.
- UNIX path style.

==================================
# 3: Common naming conventions:
==================================

- Kernel -> ke_init_x
- RunTime -> rt_copy_mem
- Hal -> hal_foo_bar

===================================
# 4: The NewBoot
===================================

- Capable of booting from a network drive.
- Loads a PE file which is the kernel
- Sanity checks, based on the number of sections. 
- Handover compliant.
- Does check for a valid invalid of NewOS (useful in the case of recovering)
