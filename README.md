# cacscading
This is a project for performance turning of cloud platform operating systems.

Currently, the primary goal is to test a load-aware thread scheduler.

This scheduler takes resource pressure into consideration, readjust scheduling period against RR(resource pressure). 

For the first step, memory will be the only measurement of resouce pressure. 

The CPU quanta Q is now a function of square root of one minus RR.

In this way, the scheduling period is increased so that overhead costed by context switch and memory swapping can be reduced. 

