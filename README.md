# get6channels.c

This program was written during my PhD to study electronic noise in atomic junctions. The problem that it addresses is the following:

the conductance of the atomic junction G, is a sum of several conduction channels, each having a transmission of 0 <= x_i <= 1,
G = sum(x_i)

we are interesting to obtain information on the distribution of x_i. Another experimental parameter, available from noise measurements, is the sum of squares of x_i, which we call F:
F = sum(x_i^2)

given that i = 1..6, i.e. we have 6 target values, and only 2 measurements (F,G) - can we say something about the distribution of x_i? Surprisingly, the answer turns to be yes for many cases, and that has become a very valuable tool I used throughout my PhD.

To obtain the possible distributions of x_i, given F and G, we can enumerate of all possibilities of x_i, with a small step (say, dx=0.01), and find the solutions that agree with the measured F,G (within the experimental uncertainties DF,DG). But first, we decide on an order to x_i (to avoid solutions that are permutations of each other), such that x_1 >= x_2 >= ... >= x_n.

However, now there is a computational challenge - we have a lot of calculations to do - for example for dx=0.01 we (naively) have 100^6 = 10^12 possible combinations of x_i values to check. So it is important to write an efficient code, that can minimize the number of calculations. This is exactly the code shown here.

There are some nice tricks used here for efficiency (and thanks to Sagi Hed for the great suggestions, that helped a lot):

- **minimize number of loop iterations** - We can find in advance that some values of x_i cannot give the solution of the given F,G, so they do not need to be calcualted. We can reduce the number of loop iterations (over x_i), by calculating the possible min-max values for each x_i.

- **use Macros instead of functions** - for MIN/MAX it is much more efficient to work with a macro than a function, which has overheads such as copying the variables in memory.

- **perform calculations only when needed** - some calculations can be avoided, if we realize ahead that the current x_i cannot give a solution.

- **use integers instead of floats** - integers can be calculated quicker than floats, and help avoid precision errors. So the trick here, was to multiple F,G,x_i by one_over_dx so that there is no use of floats

- **use the _register_ keyword** - "register int" hints to the compiler to save the variable in the register of the processor, so all operations with it are faster.
