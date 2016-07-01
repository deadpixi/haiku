Deadpixi Haiku
==============

This is a port of the Haiku Operating System, an open-source operating system that aims to clone and improve upon the class BeOS operating system.

Changes from Mainline Haiku
===========================

Deadpixi Haiku has one overarching goal:

* Reach stability. Once stability has been attained, enter maintenance mode and never leave it. Once maintenance mode has been entered, only the following changes are allowed:
    - Bugfixes
    - Changes to maintain standards compliance (POSIX, etc)
    - Translations to new human languages, or improvements to existing translations
    - Support for new common file formats
    - Documentation improvements
    - Accessibility improvements for users with differing physical and mental abilities

Specifically, once maintenance mode has been entered, no new APIs will be introduced and, barring bugfixes, no existing APIs will be changed. No fundamental UI changes will be made. Part of the charm of the original BeOS was that the entire sytem could fit inside a single programmer's head. Deadpixi Haiku will attempt to recapture that charm.

The Haiku OS team has done amazing work, and Deadpixi Haiku cannot hope to equal their work. However, in the interest of simplifying the code, several features added by the Haiku OS team will be removed from Deadpixi Haiku. In general, features that are not needed for a simple, single-user operating system with most data stored locally will be removed. Most importantly, the following features will be removed:

* The Package Filesystem and associated utilities, applications, and servers. PackageFS might be the most interesting thing to happen to package management on any platform in years, and its inclusion in Haiku OS is fascinating work. However, it is a large amount of code both inside and outside of the kernel. A user-level package manager for third-party applications is a better fit for Deadpixi Haiku.
* The bootloader. GRUB will be used in preference. The kernel will be modified to be Multiboot-compliant.
* Support for non-x86\_64 architectures. Haiku OS was stalled for years by the requirement to maintain binary compatibility with classic BeOS. In keeping with that goal, the 32-bit i386 version of Haiku requires a complex build system and customized versions of the compiler and related tools. Deadpixi Haiku will attempt to maintain source-level compatibility with Haiku as it exists today (2016), but makes no attempt to support any architecture other than x86\_64. This architecture is the "way of the future".
* The launch_server and its associated utilities and functionality. Again, this is amazing work and a great addition to Haiku, but Deadpixi Haiku targets users who prefer a simple startup script.
* The print\_server, to be replaced with CUPS. The standard printing APIs will communicate with CUPS under the hood.
* Custom decorator support, remote drawing support, and HTML5 support from the app server. This is interesting work, but it's too complicated for Deadpixi Haiku.
* Jam as a build system. A more mainstream system based on make would be simpler and easier to maintain.
* Spport for filesystems other than BFS, CDDAFS, EXFAT, FAT, IS9660, and UDF.
* POP3 support.
* Multiuser support. This is nascent in Haiku OS; Deadpixi Haiku will never include it.

A few features will be added in Deadpixi Haiku and will be backported to Haiku if possible:

* Encrypted block devices. This makes using Deadpixi Haiku safer on mobile computers, as data will be difficult or impossible to recover without a password.
* Encrypted swap file support.
* Support for installation with /boot/home on a separate partition, to facilitate installation on an encrypted block device.
* Better support for virtualized devices and running in paravirtualized environments (virtio, SPICE, etc).
