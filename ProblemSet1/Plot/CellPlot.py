import matplotlib.pyplot as plt
import os

fn = os.path.join(os.path.dirname(__file__), 'data.csv')
f = open(fn, "r")
data = f.read()
data = data.split('\n')
del data[-1]
data = map(lambda x: x.split(','), data)
data = map(lambda x: map(lambda y: int(y), x), data)
g = data

plt.plot(1)
plt.imshow(g, cmap='Greys',  interpolation='nearest')
plt.savefig('blkwht.svg')

plt.show()