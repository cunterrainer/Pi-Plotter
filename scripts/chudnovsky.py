from decimal import Decimal, getcontext
from pi_million import pi_million
from sys import stdout
from time import time

# Set the precision for Decimal operations
getcontext().prec = 150000

# Factorial function
def factorial(n):
    if n <= 1:
        return Decimal(1)
    else:
        result = Decimal(1)
        for i in range(2, n+1):
            result *= Decimal(i)
        return result

def calc(iterations):
    sm = Decimal(0)
    for i in range(iterations):
        stdout.write("Iteration: {}\r".format(i))
        num = factorial(6*i)*Decimal((545140134*i+13591409))
        denom = factorial(3*i)*pow(factorial(i), 3)*pow(Decimal(-262537412640768000), i)
        sm += num/denom
    print()
    return Decimal(426880)*Decimal(10005).sqrt()/sm

def Matches(decNum:Decimal, start):
    same = 0
    decStr = str(decNum)[2:]
    for i in range(start, len(decStr)):
        if decStr[i] != pi_million[i]:
            break
        same += 1
    return same

start = time()
print(Matches(calc(10000), 0))
print("Execution time: %.2f sec(s)" % (time()-start))