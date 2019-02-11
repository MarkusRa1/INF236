import random

sz = 3
f= open("ProblemSet1/config2D_" + str(sz) + ".txt","w+")
f.write(str(sz) + "\n")
for i in range(sz):
    bistr = ""
    for j in range(sz):
        bistr = bistr + str(random.getrandbits(1))
    if i != sz-1:
        bistr = bistr + "\n"
    f.write(bistr)
f.close()