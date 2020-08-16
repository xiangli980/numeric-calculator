# numeric-calculator

 A calculator which reads expressions from command line and returns calculated value, supporting integration and maximum/minmum solver. 

## structure

`numerics.cpp` : (main) Read in commands and initialize the input function expression. Commands: 1. `define` - define a funtion  2. `test` - test the pre-defined function with numeric numbers 3. `numint`/`mcint`/`max`/`min` - call the apis  to apply integration or gradient ascent methods<br>

`integral.cpp` : Perform numerical integration, using the
  trapezoid method or Monte Carlo integration <br>

`gradient.cpp` : Perform gradient ascent to find min/max of a given function 

`Makefile` : Generate executable file `./numerics` 

## example

Test examples are in `test.txt` 






