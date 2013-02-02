# Arduino-XMega patches

1. Get the base software from https://github.com/akafugu/Xmegaduino
2. Under the installation directory in `hardware/xmegaduino/variants` symlink/copy the directories from the variants directory in this repo
3. Under the installation directory in `hardware/xmegaduino/` to `board.txt` add (to end of file) the contents of `add_to_boards.txt`

I (rambo) updated the AVR toolchain but I'm not 100% it's required.

