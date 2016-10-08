from scipy import optimize, special
from numpy import *
from pylab import *
 
x = arange(0,100,0.1)
print(arange(0.5,50.5)) 
for k in arange(0.5,50.5):
     y = special.jv(k,x)
     plot(x,y,'red')
#     f = lambda x: -special.jv(k,x)
#     plot([x], [special.jv(k,x)],'ro')
#     x_max = optimize.fminbound(f,0,6)
#     plot([x_max], [special.jv(k,x_max)],'ro')

title('Different Bessel functions and their local maxima')
show()
