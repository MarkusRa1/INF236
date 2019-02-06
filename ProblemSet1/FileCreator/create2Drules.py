
f= open("ProblemSet1/gameOfLife.txt","w+")
get_bin = lambda x, n: format(x, 'b').zfill(n)
for i in range(512):
    bistr = get_bin(i, 9)
    numneig = bistr.count('1')
    if numneig > 1 and numneig < 4:
        bistr = bistr + " 1"
    else:
        bistr = bistr + " 0"
    if i != 511:
        bistr = bistr + "\n"
    f.write(bistr)
f.close()