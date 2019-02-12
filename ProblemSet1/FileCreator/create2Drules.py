
f= open("ProblemSet1/gameOfLife.txt","w+")
get_bin = lambda x, n: format(x, 'b').zfill(n)
for i in range(512):
    bistr = get_bin(i, 9)
    numneig = bistr.count('1')

    if bistr[4] == '1':
        if numneig < 3 or numneig > 4:
            bistr = bistr + " 0\n"
        else:
            bistr = bistr + " 1\n"
    else:
        if numneig == 3:
            bistr = bistr + " 1\n"
        else:
            bistr = bistr + " 0\n"
    f.write(bistr)
f.close()