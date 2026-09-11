BEGIN MAPPING TEST
rectangle: (10,10), (20,10), (20,20), (10,20)

CASE 1
same value for each corner, equally weighted, Voight, returns same value for each corner
input values: 10 10 10 10 
output values: 10 10 10 10

CASE 2
same value for each corner, equally weighted, Reuss, returns same value for each corner
input values: 10 10 10 10 
output values: 10 10 10 10

CASE 3
different values in each corner, equally weighted, Voight, returns for each corner input value
input values: 1 11 21 31 
output values: 1 11 21 31

CASE 4
different values in each corner, equally weighted, Reuss, returns for each corner input value
input values: 1 11 21 31 
output values: 1 11 21 31

CASE 5
different values in each corner, different weight factor in each corner, Voight, returns for each corner input value
input values: 1 11 21 31 
output values: 1 11 21 31

CASE 6
different values in each corner, different weight factor in each corner, Reuss, returns for each corner input value
input values: 1 11 21 31 
output values: 1 11 21 31

CASE 7
same value for each corner, same one in middle, equally weighted, Voight, returns same value for each corner
input values: 10 10 10 10 
output values: 10 10 10 10

CASE 8
same value for each corner, same one in middle, equally weighted, Reuss, returns same value for each corner
input values: 10 10 10 10 
output values: 10 10 10 10

CASE 9
same value for each corner, different one in middle, equally weighted, Voight,
returns plane located at one fifth distance between corner input an midpoint input above corner input values
input values: 10 10 10 10 
output values: 12 12 12 12

CASE 10
same value for each corner, different one in middle, equally weighted, Reuss
input values: 10 10 10 10 
output values: 11.1111 11.1111 11.1111 11.1111

CASE 11
same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Voight,
returns plane located at two sixth (1/3) distance between corner input an midpoint input above corner input values
input values: 10 10 10 10 
output values: 13.3333 13.3333 13.3333 13.3333

CASE 12
same value for each corner, equally weighted, different one in middle, weighted twice as heavy, Reuss
input values: 10 10 10 10 
output values: 16 16 16 16

END MAPPING TEST
