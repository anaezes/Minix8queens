# Minix 8Queens
![](https://github.com/anaezes/Minix8queens/blob/master/img/8queens.png?raw=true)

This project is an implementation of the 8 queens game/problem in the Minix operating system. It was developed with [Mariana Silva](https://www.github.com/up201506197) during "Computers Laboratory" course at Faculty of Engineering, University of Porto. The following features were implemented:

* Interruption handling for the following devices:
  * Mouse (ps/2);
  * Keyboard (ps/2);
  * Timer (i8254).

* Graphics:
  * Interact with VRAM;
  * Display all the game components, including the mouse pointer;
  * Implementation of a double buffer.

* Real Time Clock.

* State machine to keep track of the game state.