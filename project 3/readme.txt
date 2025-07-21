HOW TO RUN CODE 
Put these commands in bash

g++ prod_cons-sems-CO23BTECH11015.cpp -o SEM -std=c++20 
./SEM

g++ prod_cons-locks-CO23BTECH11015.cpp -o LOCK -std=c++20 
./LOCK



FOR EXPERIMENT 1 
(capacity, np ,nc, cntp , cntc, up , uc)  

100 10 10 100 100 10 1
100 10 10 100 100 10 2
100 10 10 100 100 10 10
100 10 10 100 100 10 20
100 10 10 100 100 10 100


FOR EXPERIMENT 2  
(capacity, np ,nc, cntp , cntc, up , uc) 

5000 50 500 100 10 10 10
5000 50 250 100 20 10 10
5000 50 50 100 100 10 10
5000 50 25 100 200 10 10
5000 50 5 100 1000 10 10




reading of experiments // Can be ignored by invigilators

experiment 1


sems
Final Average Producer Delay over 5 iterations: 10.909800 milliseconds
Final Average Producer Delay over 5 iterations: 11.096600 milliseconds
Final Average Producer Delay over 5 iterations: 11.135200 milliseconds
Final Average Producer Delay over 5 iterations: 18.497000 milliseconds
Final Average Producer Delay over 5 iterations: 86.747600 milliseconds

Final Average Consumer Delay over 5 iterations: 10.944000 milliseconds
Final Average Consumer Delay over 5 iterations: 11.006600 milliseconds
Final Average Consumer Delay over 5 iterations: 11.605000 milliseconds
Final Average Consumer Delay over 5 iterations: 21.605000 milliseconds
Final Average Consumer Delay over 5 iterations: 101.657200 microseconds

locks
Final Average Producer Delay over 5 iterations: 10.819400 milliseconds
Final Average Producer Delay over 5 iterations: 11.189600 milliseconds
Final Average Producer Delay over 5 iterations: 10.780000 milliseconds
Final Average Producer Delay over 5 iterations: 18.590400 milliseconds
Final Average Producer Delay over 5 iterations: 88.545600 milliseconds

Final Average Consumer Delay over 5 iterations: 11.497400 milliseconds
Final Average Consumer Delay over 5 iterations: 11.782600 milliseconds
Final Average Consumer Delay over 5 iterations: 11.257200 milliseconds
Final Average Consumer Delay over 5 iterations: 21.606200 milliseconds
Final Average Consumer Delay over 5 iterations: 100.825200 milliseconds


sems
Final Average Producer Delay over 5 iterations: 11.450160 milliseconds
Final Average Producer Delay over 5 iterations: 11.090720 milliseconds
Final Average Producer Delay over 5 iterations: 11.007400 milliseconds
Final Average Producer Delay over 5 iterations: 10.820440 milliseconds
Final Average Producer Delay over 5 iterations: 11.074120 milliseconds

Final Average Consumer Delay over 5 iterations: 95.673600 milliseconds
Final Average Consumer Delay over 5 iterations: 48.949800 milliseconds
Final Average Consumer Delay over 5 iterations: 11.100160 milliseconds
Final Average Consumer Delay over 5 iterations: 10.917400 milliseconds
Final Average Consumer Delay over 5 iterations: 11.151960 milliseconds

locks
Final Average Producer Delay over 5 iterations: 11.034160 milliseconds
Final Average Producer Delay over 5 iterations: 10.939120 milliseconds
Final Average Producer Delay over 5 iterations: 11.083560 milliseconds
Final Average Producer Delay over 5 iterations: 11.112440 milliseconds
Final Average Producer Delay over 5 iterations: 10.814160 milliseconds

Final Average Consumer Delay over 5 iterations: 101.097760 milliseconds
Final Average Consumer Delay over 5 iterations: 53.415080 milliseconds
Final Average Consumer Delay over 5 iterations: 11.315120 milliseconds
Final Average Consumer Delay over 5 iterations: 10.883240 milliseconds
Final Average Consumer Delay over 5 iterations: 10.996480 milliseconds