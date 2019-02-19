import matplotlib.pyplot as plt
# import os
import csv
import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression

data = list()
# fn = os.path.join(os.path.dirname(__file__), '/../Report/report.csv')
with open('c:\\Users\\marku\\Documents\\INF236\\ProblemSet1\\Report\\report.csv', 'rb') as csvfile:
    csv_reader = csv.reader(csvfile, delimiter=',')
    nr = 0
    for row in csv_reader:
        if nr != 0:
            row = np.array(map(float, row))
            row[0] = int(row[0])
            row[1] = int(row[1])
            row[2] = int(row[2])
            data.append(map(float,row))
        nr += 1

def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2., 0,
                '%ds' % int(height),
                ha='center', va='bottom')

ifigure = 0
dataTable = list()
for n in [1024, 2048, 4096, 8192, 16384, 32768, 65536]:
    x = list()
    y = list()
    Xall = list()
    yall = list()
    for sblst in data:
        if sblst[1] == n:
            if int(sblst[2]) == 10000:
                x.append(int(sblst[0]))
                y.append(round(sblst[3], 2))
    
    predproc = list()
    for proc in [1, 2, 4, 8, 16, 32, 64, 128]:
        if proc not in x:
            model = LinearRegression()
            xpred = list()
            ypred = list()
            for sblst in data:
                if proc == sblst[0]:
                    xpred.append([sblst[1]*sblst[1], int(sblst[2])])
                    ypred.append(sblst[3])
            model.fit(xpred, ypred)
            print(xpred)
            predval = model.predict([[n*n, 10000]])[0]
            y.append(round(predval, 2))
            x.append(proc)
            predproc.append(proc)

    y = [yval for _,yval in sorted(zip(x,y))]
    x = sorted(x)

    dataTable.append(y)

    for predx in predproc:
        x[x.index(predx)] = str(predx) + '*'
    
    plt.figure(ifigure)
    ifigure += 1
    index = np.arange(len(x))
    # print(index)
    # print(y)
    rect = plt.bar(index, y, width=0.8)
    autolabel(rect)
    plt.xlabel('Number of processes')
    plt.ylabel('Time in seconds')
    plt.xticks(index, x)
    plt.title('Time taken for n=' + str(n))
    plt.show()

fig, ax = plt.subplots()

# hide axes
fig.patch.set_visible(False)
ax.axis('off')
ax.axis('tight')

df = pd.DataFrame(np.array(dataTable))

ax.table(cellText=df.values, colLabels=df.columns, loc='center')


fig.tight_layout()

plt.show()