Quick Readme for Lost Cities Player

------------ 
Writeup
------------

The writeup is included in this submission as writeup.html

------------
Compiling
------------

There are a number of compilation issues with this program, but I 
think I've worked most of them out. I've successfully compiled 
and ran this program with Metrowerks 6, as well as with 
relatively newer versions of g++ (2.95 or greater).

This program will NOT compile on cunix, because for some reason 
they are using an extremely old version of GCC like 2.7. In order 
to get it to run on CUNIX, what worked for me was to compile it 
on the CS cluster (cluster.cs.columbia.edu) and then to copy the 
executable over to CUNIX.

This worked at first, but later on when I tried to run the 
programs on CUNIX, I got an error message about missing the 
standard C++ library. After some experimentation I found that 
linking on the CS cluster with the -static option fixes this 
problem. I made a separate makefile to do this automatically 
called  makefile-static.

Another issue came up when I tried to compile it on cygwin. 
Cygwin binary file was inexplicable much much slower than the 
metrowerks binary (running both on my own computer). Running a 
game state on the metrowerks binary returns the move almost 
immediately even in debug mode (with no optimizations enabled and 
with assertions turned on). But in cygwin, even with O3 
optimizations, and no assertion checking the program would take 
around 30 seconds to compute a single move. Mysteriously, I found 
that when I passed the -pg flag to GCC the problem went away and 
the program would only take 1-2 seconds to complete, which is 
still slow, but reasonable. I made a separate makefile that does 
this automatically called makefile-profile.

------------
Summary
------------

To compile, just type

  make

on the cs cluster (or some other solaris machine.) If it gives 
you an error about the C++ standard library when you try to run 
it type:

  make -f makefile-static

If it runs really really slowly type

  make -f makefile-profile

If it still runs too slowly you can set the timeout and branching 
factor parameters for the alpha beta search code (there are 
comments) at the top of ry_alphabeta.cpp. Also, you can set the 
depth of the alpha-beta search by changing the number "12" in 
main.cpp on line 47:

  ABsearch ab(12);

Also if make doesn't work, you might try the same as above with 
gmake.

And if there are any other problems please contact me at 
rey4@columbia.edu. Sorry for all of this mess. I'm new to g++, 
and retarded in general when it comes to unix.

- Russ