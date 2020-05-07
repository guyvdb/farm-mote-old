#pragma once





// The executor executes commands received on a queue. It continuously monitors
// the queue and executes as commands are received. A command is a piece of functionality
// within the system, such as "take a reading", "actuate a device", "connect to network",
// etc. By decoupling the commands into an executor, a command can be instantuated from a
// number of places such as via the console or through an http call.


